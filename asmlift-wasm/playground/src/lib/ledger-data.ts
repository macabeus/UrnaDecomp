// The shape of public/data/ledger.json, written by scripts/data.ts. Per-function arrays are indexed
// by `index - first`; strings repeat often, so they are ids into the tables.
import type { PairKind } from '@asmlift/wasm-benchmark/schema';

export interface LedgerData {
  /** the first defined function's index (imports come first) */
  first: number;
  count: number;
  components: string[];
  owners: string[];
  evidence: string[];
  /** the candidate modules a best pair was found in (`skeleton`, `probe`) */
  labels: string[];
  /** func-attribution.tsv's component per function, an id into `components` ('-' when none) */
  component: number[];
  /** functions.tsv's owner class per function, an id into `owners` */
  owner: number[];
  /** func-attribution.tsv's evidence per function, an id into `evidence` */
  evidenceOf: number[];
  /** index → the ledger's name, from a byte, fingerprint or addr pairing (`ledgerNames`) */
  names: Record<string, string>;
  /** index → the name of a shape pairing, which the lifter does not use */
  shapeNames: Record<string, string>;
  /** index → [kind, label id, the paired function's index, score] */
  best: Record<string, [PairKind, number, number, number]>;
  /** demangled → mangled, for the names above that a build lists */
  mangled: Record<string, string>;
  /** [addr, len, enc, text] of every ledger string a function's `i32.const` points into */
  strings: [number, number, string, string][];
}
