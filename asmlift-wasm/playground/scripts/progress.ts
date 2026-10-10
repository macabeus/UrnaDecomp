// The Progress page's data: every target function's verdict, component, size and name from splat/scan.tsv (what
// `make -C src status` sums into docs/status.md's meter), a name for it, and the matches of splat/matched.txt at every
// point of main's history that changed them, summed per component by today's scan. The points that main's history
// does not hold are read from matches-earlier.json, the commits from git, so a shallow clone has fewer points.
import { execFileSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { join } from 'node:path';

import { ledgerNames, readLedgerTsv } from '@asmlift/wasm/ledger';

import { type ProgressData, type ProgressPoint, VERDICTS, type Verdict } from '../src/lib/progress-data';
import { PLAYGROUND_DIR, URNA_ROOT } from './data';

export const PROGRESS_INPUTS = {
  scan: join(URNA_ROOT, 'splat/scan.tsv'),
  matches: join(URNA_ROOT, 'splat/matched.txt'),
  functions: join(URNA_ROOT, 'ledger/functions.tsv'),
  earlier: join(PLAYGROUND_DIR, 'scripts/matches-earlier.json'),
};

const DETAIL_MAX = 160;

export interface ScanRow {
  index: number;
  symbol: string;
  verdict: Verdict;
  detail: string;
  size: number;
  group: string;
}

export function readScan(path = PROGRESS_INPUTS.scan): ScanRow[] {
  const [head, ...lines] = readFileSync(path, 'utf8').split('\n');
  const cols = head.split('\t');
  const at = (name: string): number => {
    const i = cols.indexOf(name);
    if (i < 0) {
      throw new Error(`${path}: no ${name} column`);
    }
    return i;
  };
  const c = { index: at('index'), symbol: at('symbol'), verdict: at('verdict'), detail: at('detail'), size: at('size'), group: at('group') };
  return lines
    .filter((l) => l !== '')
    .map((l) => {
      const f = l.split('\t');
      const verdict = f[c.verdict] as Verdict;
      if (!VERDICTS.includes(verdict)) {
        throw new Error(`${path}: function ${f[c.index]} has verdict ${verdict}`);
      }
      return { index: Number(f[c.index]), symbol: f[c.symbol], verdict, detail: f[c.detail], size: Number(f[c.size]), group: f[c.group] };
    });
}

/** The matched indices of a matched.txt's text: the first field of each line, comments dropped. */
export function parseMatches(text: string): number[] {
  return text
    .split('\n')
    .map((l) => l.split('#', 1)[0].trim())
    .filter((l) => l !== '')
    .map((l) => Number(l.split(/\s+/, 1)[0]));
}

/** `c++filt` over many symbols in one process, line for line; the symbols unchanged when it is missing. */
export function demangle(symbols: readonly string[]): string[] {
  if (symbols.length === 0) {
    return [];
  }
  try {
    const out = execFileSync('c++filt', { input: symbols.join('\n') + '\n', encoding: 'utf8', maxBuffer: 1 << 28 }).split('\n');
    return symbols.map((s, i) => out[i] ?? s);
  } catch {
    return [...symbols];
  }
}

function git(args: string[]): string | null {
  try {
    return execFileSync('git', ['-C', URNA_ROOT, ...args], { encoding: 'utf8', maxBuffer: 1 << 28, stdio: ['ignore', 'pipe', 'ignore'] });
  } catch {
    return null;
  }
}

const utc = (iso: string): string => new Date(iso).toISOString().replace(/\.\d{3}Z$/, 'Z');

interface Snapshot {
  date: string;
  commit: string | null;
  subject: string | null;
  matches: Iterable<number>;
}

/** The matches at the points main's history does not hold, oldest first, replayed from their deltas. */
export function earlierMatches(path = PROGRESS_INPUTS.earlier): Snapshot[] {
  const seed = JSON.parse(readFileSync(path, 'utf8')) as { points: { date: string; add: number[]; remove: number[] }[] };
  const cur = new Set<number>();
  return seed.points.map((p) => {
    for (const i of p.remove) {
      cur.delete(i);
    }
    for (const i of p.add) {
      cur.add(i);
    }
    return { date: p.date, commit: null, subject: null, matches: [...cur] };
  });
}

/** The matches at every first-parent commit of HEAD that changed splat/matched.txt, oldest first. */
export function fromGit(): Snapshot[] {
  const log = git(['log', '--first-parent', '--reverse', '--format=%h%x09%cI%x09%s', 'HEAD', '--', 'splat/matched.txt']);
  if (log === null) {
    return [];
  }
  return log
    .split('\n')
    .filter((l) => l !== '')
    .map((l) => {
      const [commit, date, ...subject] = l.split('\t');
      const text = git(['show', `${commit}:splat/matched.txt`]);
      if (text === null) {
        throw new Error(`git show ${commit}:splat/matched.txt failed`);
      }
      return { date: utc(date), commit, subject: subject.join('\t'), matches: parseMatches(text) };
    });
}

export function distillProgress(): ProgressData {
  const rows = readScan();
  const first = rows[0].index;
  rows.forEach((r, i) => {
    if (r.index !== first + i) {
      throw new Error(`splat/scan.tsv: row ${i + 2} is function ${r.index}, expected ${first + i}`);
    }
  });
  const names = ledgerNames(readLedgerTsv(PROGRESS_INPUTS.functions));
  const symbols = rows.filter((r) => r.symbol !== '-');
  const demangled = new Map(demangle(symbols.map((r) => r.symbol)).map((d, i) => [symbols[i].index, d]));

  const components: string[] = [];
  const ids = new Map<string, number>();
  const componentOf = rows.map((r) => {
    let id = ids.get(r.group);
    if (id === undefined) {
      id = components.length;
      components.push(r.group);
      ids.set(r.group, id);
    }
    return id;
  });
  const detail: Record<string, string> = {};
  for (const r of rows) {
    if (r.verdict === 'differs' || r.verdict === 'unresolved') {
      detail[r.index] = r.detail.length > DETAIL_MAX ? `${r.detail.slice(0, DETAIL_MAX - 1)}…` : r.detail;
    }
  }

  const point = (s: Snapshot): ProgressPoint => {
    const matched = components.map((): [number, number] => [0, 0]);
    for (const i of s.matches) {
      const k = i - first;
      if (k < 0 || k >= rows.length) {
        throw new Error(`matches at ${s.commit ?? s.date}: function ${i} is not in splat/scan.tsv`);
      }
      matched[componentOf[k]][0]++;
      matched[componentOf[k]][1] += rows[k].size;
    }
    return { date: s.date, commit: s.commit, subject: s.subject, matched };
  };
  const history = [...earlierMatches(), ...fromGit()].map(point);
  const head = git(['log', '-1', '--format=%h%x09%cI']);
  const [commit, date] = head === null ? [null, null] : head.trim().split('\t');
  return {
    commit,
    date: date === null ? null : utc(date),
    first,
    components,
    component: componentOf,
    verdict: rows.map((r) => VERDICTS.indexOf(r.verdict)),
    size: rows.map((r) => r.size),
    name: rows.map((r) => demangled.get(r.index) ?? names.get(r.index) ?? ''),
    detail,
    history,
  };
}
