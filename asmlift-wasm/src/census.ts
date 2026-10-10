// Match every function of module A to module B: by exact fingerprint, then address-blind, then by
// shape (normalize.ts defines the three; a pair is tagged with the first tier that joined it). Each
// hash may name several functions (identical bodies are common: thunks, destructors); those are
// paired by count, so an A hash held by 3 functions and a B hash held by 2 yields 2 matches and
// 1 unmatched. The size histogram buckets a function by the floor of log2 of its body size.
import type { NormalizedFn } from './normalize';
import type { Module } from './parse';

export interface Pair {
  a: number;
  b: number;
  by: 'fingerprint' | 'addr' | 'shape';
}

export interface Census {
  pairs: Pair[];
  unmatchedA: number[];
  unmatchedB: number[];
  /** histogram rows: bucket label, functions of A in it, how many of those matched per tier */
  histogram: {
    bucket: string;
    total: number;
    byFingerprint: number;
    byAddr: number;
    byShape: number;
    bytes: number;
    matchedBytes: number;
  }[];
  counts: { a: number; b: number; byFingerprint: number; byAddr: number; byShape: number };
}

function groups(
  norm: NormalizedFn[],
  key: (n: NormalizedFn) => string,
  only: Set<number> | null,
): Map<string, number[]> {
  const g = new Map<string, number[]>();
  for (const n of norm) {
    if (only !== null && !only.has(n.index)) {
      continue;
    }
    const k = key(n);
    const list = g.get(k);
    if (list === undefined) {
      g.set(k, [n.index]);
    } else {
      list.push(n.index);
    }
  }
  return g;
}

function pairUp(
  ga: Map<string, number[]>,
  gb: Map<string, number[]>,
  by: Pair['by'],
  pairs: Pair[],
  leftA: Set<number>,
  leftB: Set<number>,
): void {
  for (const [k, as] of ga) {
    const bs = gb.get(k);
    if (bs === undefined) {
      continue;
    }
    const n = Math.min(as.length, bs.length);
    for (let i = 0; i < n; i++) {
      pairs.push({ a: as[i], b: bs[i], by });
      leftA.delete(as[i]);
      leftB.delete(bs[i]);
    }
  }
}

export function census(a: Module, na: NormalizedFn[], b: Module, nb: NormalizedFn[]): Census {
  const pairs: Pair[] = [];
  const leftA = new Set(na.map((n) => n.index));
  const leftB = new Set(nb.map((n) => n.index));
  pairUp(
    groups(na, (n) => n.fingerprint, null),
    groups(nb, (n) => n.fingerprint, null),
    'fingerprint',
    pairs,
    leftA,
    leftB,
  );
  pairUp(
    groups(na, (n) => n.addr, leftA),
    groups(nb, (n) => n.addr, leftB),
    'addr',
    pairs,
    leftA,
    leftB,
  );
  pairUp(
    groups(na, (n) => n.shape, leftA),
    groups(nb, (n) => n.shape, leftB),
    'shape',
    pairs,
    leftA,
    leftB,
  );
  pairs.sort((x, y) => x.a - y.a);

  const byA = new Map(pairs.map((p) => [p.a, p]));
  const buckets = new Map<number, Census['histogram'][number]>();
  for (const fn of a.funcs) {
    const k = Math.floor(Math.log2(Math.max(1, fn.size)));
    let row = buckets.get(k);
    if (row === undefined) {
      const lo = 2 ** k;
      row = {
        bucket: `${lo}-${2 * lo - 1} B`,
        total: 0,
        byFingerprint: 0,
        byAddr: 0,
        byShape: 0,
        bytes: 0,
        matchedBytes: 0,
      };
      buckets.set(k, row);
    }
    row.total++;
    row.bytes += fn.size;
    const p = byA.get(fn.index);
    if (p !== undefined) {
      row.matchedBytes += fn.size;
      if (p.by === 'fingerprint') {
        row.byFingerprint++;
      } else if (p.by === 'addr') {
        row.byAddr++;
      } else {
        row.byShape++;
      }
    }
  }
  const histogram = [...buckets.entries()].sort((x, y) => x[0] - y[0]).map(([, r]) => r);
  void b;
  return {
    pairs,
    unmatchedA: [...leftA].sort((x, y) => x - y),
    unmatchedB: [...leftB].sort((x, y) => x - y),
    histogram,
    counts: {
      a: na.length,
      b: nb.length,
      byFingerprint: pairs.filter((p) => p.by === 'fingerprint').length,
      byAddr: pairs.filter((p) => p.by === 'addr').length,
      byShape: pairs.filter((p) => p.by === 'shape').length,
    },
  };
}

export interface RenderCensusOptions {
  listUnmatched?: number;
  /** list every pair as `a b tier [size] [name of b]` */
  listPairs?: boolean;
  nameB?: (index: number) => string | undefined;
  sizeA?: (index: number) => number | undefined;
}

export function renderCensus(c: Census, opts: RenderCensusOptions = {}): string {
  const lines: string[] = [];
  lines.push(
    `A: ${c.counts.a} functions, B: ${c.counts.b}. Matched ${c.counts.byFingerprint} by fingerprint + ${c.counts.byAddr} by addr + ${c.counts.byShape} by shape = ${c.pairs.length}; unmatched A ${c.unmatchedA.length}, B ${c.unmatchedB.length}.`,
  );
  lines.push('');
  lines.push(
    `${'size bucket'.padEnd(18)} ${'fns'.padStart(6)} ${'fp'.padStart(6)} ${'addr'.padStart(6)} ${'shape'.padStart(6)} ${'unmatched'.padStart(9)} ${'bytes'.padStart(9)} ${'matched%'.padStart(8)}`,
  );
  for (const r of c.histogram) {
    const un = r.total - r.byFingerprint - r.byAddr - r.byShape;
    const pct = r.bytes === 0 ? 0 : (100 * r.matchedBytes) / r.bytes;
    lines.push(
      `${r.bucket.padEnd(18)} ${String(r.total).padStart(6)} ${String(r.byFingerprint).padStart(6)} ${String(r.byAddr).padStart(6)} ${String(r.byShape).padStart(6)} ${String(un).padStart(9)} ${String(r.bytes).padStart(9)} ${pct.toFixed(1).padStart(8)}`,
    );
  }
  if (opts.listPairs) {
    lines.push('');
    lines.push('pairs (a, b, tier, size of a, name of b):');
    for (const p of c.pairs) {
      lines.push(
        `${String(p.a).padStart(6)} ${String(p.b).padStart(6)} ${p.by.padEnd(11)} ${String(opts.sizeA?.(p.a) ?? '').padStart(6)} ${opts.nameB?.(p.b) ?? ''}`,
      );
    }
  }
  const limit = opts.listUnmatched ?? 40;
  if (c.unmatchedA.length > 0) {
    lines.push('');
    lines.push(
      `unmatched in A (first ${Math.min(limit, c.unmatchedA.length)}): ${c.unmatchedA.slice(0, limit).join(' ')}`,
    );
  }
  if (c.unmatchedB.length > 0) {
    lines.push(
      `unmatched in B (first ${Math.min(limit, c.unmatchedB.length)}): ${c.unmatchedB.slice(0, limit).join(' ')}`,
    );
  }
  return lines.join('\n');
}
