import rowsJson from '@asmlift/wasm-benchmark/dataset/rows.json';
import setupsJson from '@asmlift/wasm-benchmark/dataset/setups.json';
import type { DatasetRow, Setup } from '@asmlift/wasm-benchmark/schema';

export const DATASET_ROWS = rowsJson as DatasetRow[];
export const SETUPS = setupsJson as Setup[];
export const SETUP_BY_KEY = new Map(SETUPS.map((s) => [s.key, s]));
