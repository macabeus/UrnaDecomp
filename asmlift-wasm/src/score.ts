// An objdiff-style row diff between two normalised instruction streams. Rows are compared as whole
// strings; the alignment is Myers' O(ND) diff, so two functions that differ by one inserted
// instruction align everything after it instead of comparing shifted rows pairwise.
//
// score = the number of rows that differ after alignment: a deleted row 1, an inserted row 1, and
// within one hunk a deletion paired with an insertion counts ONCE as a replaced row (the way a
// reader sees "this instruction changed"). 0 means every row is identical after normalisation.
// `byteExact` is the stronger claim that the two raw bodies are the same bytes.
import type { Func } from './parse';

export type RowKind = 'same' | 'replaced' | 'inserted' | 'deleted';

export interface DiffRow {
  kind: RowKind;
  left: string | null;
  right: string | null;
  /** for `replaced`: the two rows share an opcode (only an operand differs) */
  sameOpcode?: boolean;
}

export interface Breakdown {
  same: number;
  replaced: number;
  /** of the replaced rows, those whose opcode is the same */
  operandOnly: number;
  inserted: number;
  deleted: number;
  leftRows: number;
  rightRows: number;
  /** the alignment hit the edit-distance ceiling and fell back to positional pairing */
  approximate: boolean;
}

export interface Score {
  score: number;
  /** the alignment's row count: same + replaced + inserted + deleted (the denominator) */
  rows: number;
  byteExact: boolean;
  breakdown: Breakdown;
  diff: DiffRow[];
}

/** Myers' greedy diff with a ceiling on D. Returns the edit script as a list of ops over the two
 *  arrays, or null when D would exceed `maxD`. */
function myers(a: string[], b: string[], maxD: number): ('=' | '-' | '+')[] | null {
  const n = a.length;
  const max = n + b.length;
  if (max === 0) {
    return [];
  }
  const limit = Math.min(max, maxD);
  const offset = limit + 1;
  const size = 2 * limit + 3;
  let v = new Int32Array(size).fill(-1);
  v[offset + 1] = 0;
  const trace: Int32Array[] = [];
  for (let d = 0; d <= limit; d++) {
    const snapshot = v.slice();
    trace.push(snapshot);
    for (let k = -d; k <= d; k += 2) {
      let x: number;
      if (k === -d || (k !== d && v[offset + k - 1] < v[offset + k + 1])) {
        x = v[offset + k + 1];
      } else {
        x = v[offset + k - 1] + 1;
      }
      let y = x - k;
      while (x < n && y < b.length && a[x] === b[y]) {
        x++;
        y++;
      }
      v[offset + k] = x;
      if (x >= n && y >= b.length) {
        // walk back
        const ops: ('=' | '-' | '+')[] = [];
        let px = x;
        let py = y;
        for (let dd = d; dd > 0; dd--) {
          const vv = trace[dd];
          const kk = px - py;
          let prevK: number;
          if (kk === -dd || (kk !== dd && vv[offset + kk - 1] < vv[offset + kk + 1])) {
            prevK = kk + 1;
          } else {
            prevK = kk - 1;
          }
          const prevX = vv[offset + prevK];
          const prevY = prevX - prevK;
          while (px > prevX && py > prevY) {
            ops.push('=');
            px--;
            py--;
          }
          if (prevK === kk + 1) {
            ops.push('+');
            py--;
          } else {
            ops.push('-');
            px--;
          }
        }
        while (px > 0 && py > 0) {
          ops.push('=');
          px--;
          py--;
        }
        ops.reverse();
        return ops;
      }
    }
  }
  return null;
}

const DEFAULT_MAX_D = 6000;

function opcodeOf(row: string): string {
  const sp = row.indexOf(' ');
  return sp < 0 ? row : row.slice(0, sp);
}

/** Diff two row arrays. Common prefix and suffix are stripped first (cheap, and it keeps Myers'
 *  D small on a typical near-match); when the middle still needs more than `maxD` edits the rows
 *  are paired positionally and the result is flagged `approximate`. */
export function diffRows(
  left: string[],
  right: string[],
  maxD = DEFAULT_MAX_D,
): { diff: DiffRow[]; approximate: boolean } {
  let pre = 0;
  while (pre < left.length && pre < right.length && left[pre] === right[pre]) {
    pre++;
  }
  let suf = 0;
  while (
    suf < left.length - pre &&
    suf < right.length - pre &&
    left[left.length - 1 - suf] === right[right.length - 1 - suf]
  ) {
    suf++;
  }
  const a = left.slice(pre, left.length - suf);
  const b = right.slice(pre, right.length - suf);
  const out: DiffRow[] = [];
  for (let i = 0; i < pre; i++) {
    out.push({ kind: 'same', left: left[i], right: right[i] });
  }
  const ops = myers(a, b, maxD);
  let approximate = false;
  if (ops === null) {
    approximate = true;
    const n = Math.min(a.length, b.length);
    for (let i = 0; i < n; i++) {
      if (a[i] === b[i]) {
        out.push({ kind: 'same', left: a[i], right: b[i] });
      } else {
        out.push({ kind: 'replaced', left: a[i], right: b[i], sameOpcode: opcodeOf(a[i]) === opcodeOf(b[i]) });
      }
    }
    for (let i = n; i < a.length; i++) {
      out.push({ kind: 'deleted', left: a[i], right: null });
    }
    for (let i = n; i < b.length; i++) {
      out.push({ kind: 'inserted', left: null, right: b[i] });
    }
  } else {
    // pair each hunk's deletions with its insertions as replacements
    let ia = 0;
    let ib = 0;
    let k = 0;
    while (k < ops.length) {
      if (ops[k] === '=') {
        out.push({ kind: 'same', left: a[ia], right: b[ib] });
        ia++;
        ib++;
        k++;
        continue;
      }
      const dels: string[] = [];
      const ins: string[] = [];
      while (k < ops.length && ops[k] !== '=') {
        if (ops[k] === '-') {
          dels.push(a[ia++]);
        } else {
          ins.push(b[ib++]);
        }
        k++;
      }
      const paired = Math.min(dels.length, ins.length);
      for (let i = 0; i < paired; i++) {
        out.push({
          kind: 'replaced',
          left: dels[i],
          right: ins[i],
          sameOpcode: opcodeOf(dels[i]) === opcodeOf(ins[i]),
        });
      }
      for (let i = paired; i < dels.length; i++) {
        out.push({ kind: 'deleted', left: dels[i], right: null });
      }
      for (let i = paired; i < ins.length; i++) {
        out.push({ kind: 'inserted', left: null, right: ins[i] });
      }
    }
  }
  for (let i = left.length - suf; i < left.length; i++) {
    out.push({ kind: 'same', left: left[i], right: right[i - left.length + right.length] });
  }
  return { diff: out, approximate };
}

export interface ScoreInput {
  rows: string[];
  fn?: Func;
  signature?: string;
}

/** Score `candidate` against `target`. When both carry a `Func` the raw bodies decide `byteExact`.
 *  A differing signature (type or locals) adds one row, as its own line above the body. */
export function score(target: ScoreInput, candidate: ScoreInput, maxD = DEFAULT_MAX_D): Score {
  const left = target.signature === undefined ? target.rows : [`;; ${target.signature}`, ...target.rows];
  const right = candidate.signature === undefined ? candidate.rows : [`;; ${candidate.signature}`, ...candidate.rows];
  const { diff, approximate } = diffRows(left, right, maxD);
  const breakdown: Breakdown = {
    same: 0,
    replaced: 0,
    operandOnly: 0,
    inserted: 0,
    deleted: 0,
    leftRows: left.length,
    rightRows: right.length,
    approximate,
  };
  for (const r of diff) {
    breakdown[r.kind]++;
    if (r.kind === 'replaced' && r.sameOpcode) {
      breakdown.operandOnly++;
    }
  }
  const byteExact =
    target.fn !== undefined &&
    candidate.fn !== undefined &&
    Buffer.from(target.fn.bodyBytes).equals(Buffer.from(candidate.fn.bodyBytes));
  return {
    score: breakdown.replaced + breakdown.inserted + breakdown.deleted,
    rows: diff.length,
    byteExact,
    breakdown,
    diff,
  };
}

/** A two-column text rendering: `|` same, `~` replaced, `<` only in the target, `>` only in the candidate. */
export function renderSideBySide(
  s: Score,
  opts: { width?: number; onlyDiff?: boolean; context?: number } = {},
): string {
  const width = opts.width ?? 56;
  const mark: Record<RowKind, string> = { same: ' | ', replaced: ' ~ ', deleted: ' < ', inserted: ' > ' };
  const cut = (t: string | null): string => {
    const s = t ?? '';
    return (s.length > width ? `${s.slice(0, width - 1)}…` : s).padEnd(width);
  };
  const lines: string[] = [];
  const b = s.breakdown;
  lines.push(
    `score ${s.score} over ${s.rows} rows (same ${b.same}, replaced ${b.replaced} [${b.operandOnly} operand-only], ` +
      `inserted ${b.inserted}, deleted ${b.deleted})${s.byteExact ? ' BYTE-EXACT' : ''}${b.approximate ? ' APPROXIMATE alignment' : ''}`,
  );
  lines.push(`${'target'.padEnd(width)}   ${'candidate'}`);
  const show = new Array<boolean>(s.diff.length).fill(!opts.onlyDiff);
  if (opts.onlyDiff) {
    const ctx = opts.context ?? 2;
    s.diff.forEach((r, i) => {
      if (r.kind === 'same') {
        return;
      }
      for (let k = Math.max(0, i - ctx); k <= Math.min(s.diff.length - 1, i + ctx); k++) {
        show[k] = true;
      }
    });
  }
  let skipping = false;
  s.diff.forEach((r, i) => {
    if (!show[i]) {
      if (!skipping) {
        lines.push('   …');
      }
      skipping = true;
      return;
    }
    skipping = false;
    lines.push(`${cut(r.left)}${mark[r.kind]}${r.right ?? ''}`);
  });
  return lines.join('\n');
}
