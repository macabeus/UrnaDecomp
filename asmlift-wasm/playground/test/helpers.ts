import { readFileSync } from 'node:fs';

import { INPUTS, distillLedger } from '../scripts/data';
import type { LedgerData } from '../src/lib/ledger-data';
import { type Target, openTarget } from '../src/lib/target';

let data: LedgerData | null = null;
let target: Target | null = null;

export function ledgerData(): LedgerData {
  data ??= distillLedger();
  return data;
}

/** The target as the page opens it: module bytes, glue text and the distilled ledger. */
export function pageTarget(): Target {
  target ??= openTarget(new Uint8Array(readFileSync(INPUTS.wasm)), readFileSync(INPUTS.glue, 'utf8'), ledgerData());
  return target;
}
