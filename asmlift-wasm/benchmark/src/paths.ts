import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

/** `asmlift-wasm/benchmark` */
export const BENCH_DIR = resolve(dirname(fileURLToPath(import.meta.url)), '..');
/** The urna checkout the toolkit is vendored in; URNA_ROOT points the benchmark at another one. */
export const URNA_ROOT = process.env.URNA_ROOT ?? resolve(BENCH_DIR, '../..');
export const TARGET = join(URNA_ROOT, 'target/vota_web_wasm.wasm');
export const LEDGER_TSV = join(URNA_ROOT, 'ledger/functions.tsv');
export const SETUPS_JSON = join(BENCH_DIR, 'dataset/setups.json');
export const ROWS_JSON = join(BENCH_DIR, 'dataset/rows.json');
/** The dataset's version and what its selected rows were selected from. */
export const SELECTION_JSON = join(BENCH_DIR, 'dataset/selection.json');
/** The build's definitions of the names that selection looked up, as it read them. */
export const SELECTION_DEFINITIONS_JSON = join(BENCH_DIR, 'dataset/selection-definitions.json');
export const RESULTS_DIR = join(BENCH_DIR, 'results');
/** Scratch under the checkout's gitignored build tree: lifted sources, objects, the definitions cache.
 *  Docker mounts it, so it must not live under macOS's per-user `$TMPDIR`. */
export const WORK_DIR = join(URNA_ROOT, 'build/out/bench');
export const DEFINITIONS_JSON = join(WORK_DIR, 'definitions.json');
