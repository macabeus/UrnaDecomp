import { readFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { type Module, parseModule } from '../src/parse';

/** The urna checkout this package is vendored in; URNA_ROOT points the suites at another one. */
export const URNA_ROOT = process.env.URNA_ROOT ?? resolve(dirname(fileURLToPath(import.meta.url)), '../..');

export const TARGET = join(URNA_ROOT, 'target/vota_web_wasm.wasm');

let cached: Module | null = null;
export function target(): Module {
  cached ??= parseModule(new Uint8Array(readFileSync(TARGET)));
  return cached;
}
