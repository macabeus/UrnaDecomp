// Copies the target module and its glue next to the page, distills the ledger and writes the Progress page's
// data: run before `vite` and `vite build` (the package's dev and build scripts do).
import { writeFileSync } from 'node:fs';
import { join } from 'node:path';

import { DATA_DIR, prepareData } from './data';
import { distillProgress } from './progress';

for (const line of prepareData()) {
  process.stdout.write(`${DATA_DIR}/${line}\n`);
}
const progress = JSON.stringify(distillProgress());
writeFileSync(join(DATA_DIR, 'progress.json'), progress);
process.stdout.write(`${DATA_DIR}/progress.json ${(progress.length / 1024).toFixed(0)} KiB\n`);
