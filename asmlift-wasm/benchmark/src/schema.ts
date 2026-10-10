// The benchmark's data: the compile-setup registry (dataset/setups.json), the dataset rows
// (dataset/rows.json) and a run's results (results/<label>.json). benchmark/README.md documents
// every field; the playground imports these types from `@asmlift/wasm-benchmark/schema`.

export type Lang = 'c' | 'c++';

/** How the checker re-derives a setup's flags and objects from the build. */
export type SetupTruth =
  /** objects from `make -C src print-<var>`, each object's flags from `make -n -B` of it */
  | { kind: 'make'; vars: string[]; match?: string }
  /** objects from `make -C src print-<var>`; flags recorded by the library build in `file`
   *  (`nul`: NUL-separated, `lines`: one per line, `make-var`: `VAR=…` lines of a Makefile, joined in `vars` order) */
  | {
      kind: 'artefact';
      vars: string[];
      file: string;
      format: 'nul' | 'lines' | 'make-var';
      fileVars?: string[];
      /** flags the library build adds to its C++ files only (7-Zip's `-DPROG_VARIANT_R`) */
      cxxOnly?: string[];
    }
  /** an emscripten system library: its archive in the image's sysroot, its flags from tools/system_libs.py */
  | { kind: 'emscripten'; library: string; archive: string }
  /** header-only: compiled by whichever setup's TU includes it */
  | { kind: 'header-only'; namespace: string };

export interface Setup {
  key: string;
  title: string;
  /** the flags per source language, as the build passes them (include paths, path maps and
   *  warnings left out); null for a header-only library */
  flags: Partial<Record<Lang, string>> | null;
  image: string;
  /** where these flags live in the repo, for a reader */
  source: string;
  truth: SetupTruth;
  /** the ledger components whose functions are this setup's when no symbol says otherwise */
  components?: string[];
  note?: string;
}

export type PairKind = 'byte' | 'fingerprint' | 'addr' | 'shape' | 'none';

export const FEATURES = [
  'leaf',
  'loop',
  'switch',
  'call_indirect',
  'invoke',
  'throw',
  'frame',
  'alloca',
  'inlined-frame',
  'i64',
  'float',
  'bulk-memory',
  'select',
] as const;
export type Feature = (typeof FEATURES)[number];

export interface Reference {
  /** repo-relative, or `emsdk:<path>` inside the pinned image */
  path: string;
  /** 1-based line of the definition, when found */
  line?: number;
}

export interface DatasetRow {
  /** `<setup>/<index>`, stable for the pinned target */
  id: string;
  /** function index in the target (imports first) */
  index: number;
  /** body bytes */
  size: number;
  band: string;
  setup: string;
  lang: Lang;
  /** the setup's flags for this function's language (the including setup's for a header-only one) */
  flags: string;
  /** a header-only library's function: the setup whose TU instantiated it */
  instantiatedBy?: string;
  /** the ledger's attribution (`unknown` when it has none) and owner class (`-` when none) */
  component: string;
  owner: string;
  /** the ledger's name for it, from a byte, fingerprint or addr pairing */
  name: string | null;
  /** the linker symbol that name resolved to */
  symbol: string | null;
  features: Feature[];
  /** the ledger's current best pair */
  ledger: { kind: PairKind; score: number | null };
  /** how the setup was decided: `symbol` = the build object defining the ledger's name;
   *  `component` = the ledger's attribution (no reproducing object yet) */
  evidence: 'symbol' | 'component';
  /** the object that defines the symbol (repo-relative, `emsdk:<archive>:<member>` for a system library) */
  object: string | null;
  reference: Reference | null;
  origin: 'selected' | 'manual';
}

/** dataset/selection.json: the dataset's version and the inputs its selected rows were selected from,
 *  enough to run that selection again. Repo paths are relative to the urna root. */
export interface Selection {
  /** one more on every re-selection or references update, each of which comes with a fresh baseline */
  version: number;
  /** rows per setup */
  per: number;
  /** the urna commit selection ran at, whose files the ledger, the registry and the sources below are */
  commit: string;
  inputs: {
    target: { path: string; sha256: string };
    glue: { path: string; sha256: string };
    /** git blob ids at `commit`; dataset/selection-inputs/ keeps a copy of each file at its repo path */
    ledger: { path: string; blob: string };
    setups: { path: string; blob: string };
    /** the other files `commit` tracked that selection read, the sources references were looked up in: repo
     *  path → git blob id, each kept the same way */
    sources: Record<string, string>;
    /** the build's definitions of the names selection looked up: `names` of them, read with llvm-nm
     *  from the objects built in the checkout at `build`; `moved`: link inputs the build has moved since
     *  (old path → new path), which the snapshot names at their new path */
    definitions: { path: string; sha256: string; names: number; build: string; note?: string; moved?: Record<string, string> };
  };
  /** set by a references update (`pnpm bench select --references --write`): the rows are those of version
   *  `rowsOf`, with the references selection gives them today on the same recorded inputs (and the objects
   *  of `inputs.definitions.moved` at their new paths); `note` says why */
  references?: { rowsOf: number; note: string };
}

export type Outcome = 'exact' | 'differs' | 'lift-threw' | 'compile-failed' | 'extract-failed' | 'link-failed';

export interface RowResult {
  id: string;
  index: number;
  setup: string;
  size: number;
  band: string;
  features: Feature[];
  lifted: boolean;
  compiled: boolean;
  /** rows of the target function after canonicalisation */
  targetRows: number;
  /** the `em++ -c` object against the target, depth 0, canonicalised rows */
  raw: { score: number; ratio: number } | null;
  /** the object linked alone and put through the target link's wasm-opt -Oz passes */
  opt: { score: number; ratio: number } | null;
  /** opt score 0 */
  exact: boolean;
  outcome: Outcome;
  failure: { class: string; line: string } | null;
  /** the lifter's stand-in notes in this function's source */
  standIns: string[];
}

export interface Totals {
  rows: number;
  lifted: number;
  compiled: number;
  /** linked and optimised */
  optimised: number;
  exactRaw: number;
  exact: number;
  medianRatio: number | null;
  medianOptRatio: number | null;
  meanOptRatio: number | null;
  outcomes: Partial<Record<Outcome, number>>;
}

export interface BenchResults {
  schema: 1;
  label: string;
  createdAt: string;
  target: { path: string; sha256: string };
  image: string;
  /** sha256 of rows.json and setups.json as run */
  dataset: { rows: number; rowsSha256: string; setupsSha256: string };
  /** the urna commit the run started from (`+dirty` when the tree had changes) */
  commit: string | null;
  durationMs: { lift: number; compileAndOptimise: number; total: number };
  totals: { overall: Totals; bySetup: Record<string, Totals> };
  rows: RowResult[];
}
