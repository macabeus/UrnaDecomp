// The census meter: functions.tsv tallied per owner (or per component) and by
// match kind, with the delta since a previous ledger. Snapshots live in `<ledger>/history/<stamp>.tsv`
// so every census leaves the number it was compared against.
import { copyFileSync, existsSync, mkdirSync, readdirSync } from 'node:fs';
import { join } from 'node:path';

import { type LedgerRow, type MatchKind } from './ledger';

export const KINDS: readonly MatchKind[] = ['byte', 'fingerprint', 'addr', 'shape', 'none'];

export interface Tally {
  n: Record<MatchKind, number>;
  bytes: Record<MatchKind, number>;
  total: number;
  totalBytes: number;
}

export function tally(rows: Iterable<LedgerRow>): Tally {
  const zero = (): Record<MatchKind, number> => ({ byte: 0, fingerprint: 0, addr: 0, shape: 0, none: 0 });
  const t: Tally = { n: zero(), bytes: zero(), total: 0, totalBytes: 0 };
  for (const r of rows) {
    const k = r.best?.kind ?? 'none';
    t.n[k]++;
    t.bytes[k] += r.size;
    t.total++;
    t.totalBytes += r.size;
  }
  return t;
}

export type GroupBy = 'owner' | 'component';

export function groupRows(rows: Iterable<LedgerRow>, by: GroupBy): Map<string, LedgerRow[]> {
  const g = new Map<string, LedgerRow[]>();
  for (const r of rows) {
    const k = r[by];
    const list = g.get(k);
    if (list === undefined) {
      g.set(k, [r]);
    } else {
      list.push(r);
    }
  }
  return g;
}

const signed = (d: number): string => (d === 0 ? '' : d > 0 ? ` (+${d})` : ` (${d})`);

/** A Markdown table: one row per group plus `all`, columns per kind with the delta against `prev`
 *  in parentheses, then the byte-exact share of the group's body bytes. */
export function renderProgress(
  cur: Map<number, LedgerRow>,
  prev: Map<number, LedgerRow> | null,
  by: GroupBy,
  opts: { minGroup?: number } = {},
): string {
  const minGroup = opts.minGroup ?? 1;
  const groups = groupRows(cur.values(), by);
  const prevGroups = prev === null ? null : groupRows(prev.values(), by);
  const lines: string[] = [];
  lines.push(
    `| ${by} | fns | ${KINDS.join(' | ')} | byte % | byte B % |`,
    `| --- | ---: | ${KINDS.map(() => '---:').join(' | ')} | ---: | ---: |`,
  );
  const line = (name: string, rows: LedgerRow[], prevRows: LedgerRow[] | null): string => {
    const t = tally(rows);
    const p = prevRows === null ? null : tally(prevRows);
    const cells = KINDS.map((k) => `${t.n[k]}${p === null ? '' : signed(t.n[k] - p.n[k])}`);
    const pct = t.total === 0 ? 0 : (100 * t.n.byte) / t.total;
    const bpct = t.totalBytes === 0 ? 0 : (100 * t.bytes.byte) / t.totalBytes;
    return `| ${name} | ${t.total} | ${cells.join(' | ')} | ${pct.toFixed(1)} | ${bpct.toFixed(1)} |`;
  };
  lines.push(line('**all**', [...cur.values()], prev === null ? null : [...prev.values()]));
  const sorted = [...groups.entries()].sort((a, b) => b[1].length - a[1].length);
  for (const [name, rows] of sorted) {
    if (rows.length < minGroup) {
      continue;
    }
    lines.push(line(name, rows, prevGroups === null ? null : (prevGroups.get(name) ?? [])));
  }
  return lines.join('\n');
}

export interface Movement {
  index: number;
  size: number;
  owner: string;
  from: MatchKind;
  to: MatchKind;
  fromScore: number | null;
  toScore: number | null;
}

const RANK: Record<MatchKind, number> = { byte: 0, fingerprint: 1, addr: 2, shape: 3, none: 4 };

/** Every function whose kind or score changed between two ledgers, strongest improvement first. */
export function movements(cur: Map<number, LedgerRow>, prev: Map<number, LedgerRow>): Movement[] {
  const out: Movement[] = [];
  for (const [index, r] of cur) {
    const p = prev.get(index);
    if (p === undefined) {
      continue;
    }
    const from = p.best?.kind ?? 'none';
    const to = r.best?.kind ?? 'none';
    const fromScore = p.best?.score ?? null;
    const toScore = r.best?.score ?? null;
    if (from !== to || fromScore !== toScore) {
      out.push({ index, size: r.size, owner: r.owner, from, to, fromScore, toScore });
    }
  }
  out.sort((a, b) => RANK[a.to] - RANK[b.to] || RANK[b.from] - RANK[a.from] || b.size - a.size);
  return out;
}

export function renderMovements(ms: readonly Movement[], limit: number): string {
  const up = ms.filter((m) => RANK[m.to] < RANK[m.from] || (m.to === m.from && (m.toScore ?? 0) < (m.fromScore ?? 0)));
  const down = ms.length - up.length;
  const lines = [`moved: ${ms.length} functions (${up.length} up, ${down} down or sideways)`];
  const fmt = (m: Movement): string =>
    `  ${String(m.index).padStart(6)} ${String(m.size).padStart(7)} B ${m.owner.padEnd(8)} ${m.from}${m.fromScore === null || m.fromScore === 0 ? '' : `/${m.fromScore}`} → ${m.to}${m.toScore === null || m.toScore === 0 ? '' : `/${m.toScore}`}`;
  for (const m of ms.slice(0, limit)) {
    lines.push(fmt(m));
  }
  if (ms.length > limit) {
    lines.push(`  … ${ms.length - limit} more`);
  }
  return lines.join('\n');
}

/** `history/<stamp>.tsv` files, newest last (stamps sort lexically). */
export function snapshots(historyDir: string): string[] {
  if (!existsSync(historyDir)) {
    return [];
  }
  return readdirSync(historyDir)
    .filter((f) => f.endsWith('.tsv'))
    .sort()
    .map((f) => join(historyDir, f));
}

export function stamp(d = new Date()): string {
  return d
    .toISOString()
    .replace(/[-:]/g, '')
    .replace(/\.\d{3}Z$/, 'Z')
    .replace('T', '-');
}

/** Copy the ledger into `history/<stamp>.tsv`; returns the snapshot path. */
export function takeSnapshot(ledgerTsv: string, historyDir: string, label?: string): string {
  mkdirSync(historyDir, { recursive: true });
  const path = join(historyDir, `${stamp()}${label ? `-${label.replace(/[^\w.-]+/g, '_')}` : ''}.tsv`);
  copyFileSync(ledgerTsv, path);
  return path;
}
