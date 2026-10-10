// The shape of public/data/progress.json, written by scripts/progress.ts from splat/scan.tsv (the verdict of
// every target function, the file `make -C src status` sums into the README's meter) and the history of
// splat/matched.txt. Per-function arrays are indexed by `index - first`.

export const VERDICTS = ['match', 'differs', 'unresolved', 'no-candidate'] as const;
export type Verdict = (typeof VERDICTS)[number];

export interface ProgressPoint {
  /** the commit's committer date, UTC ISO */
  date: string;
  /** the commit's short hash and subject; null for the points main's history does not hold (scripts/matches-earlier.json) */
  commit: string | null;
  subject: string | null;
  /** matched functions and code bytes per component (parallel to `components`), by today's scan */
  matched: [number, number][];
}

export interface ProgressData {
  /** the commit the data was made at (short hash), null outside a git checkout */
  commit: string | null;
  /** its committer date */
  date: string | null;
  /** the first target function index (imports come first); the scan has one row per index from it */
  first: number;
  /** the scan's `group` column values, in first-seen order */
  components: string[];
  /** per function: an id into `components` */
  component: number[];
  /** per function: an id into `VERDICTS` */
  verdict: number[];
  /** per function: the body's size in bytes */
  size: number[];
  /** per function: the symbol of our body for it, demangled (the scan's `symbol`), else the ledger's name, else '' */
  name: string[];
  /** index → the scan's `detail` for a differs or unresolved function (the first difference), cut short */
  detail: Record<string, string>;
  /** oldest first; the last point is the commit above */
  history: ProgressPoint[];
}
