// Copies the target module and its glue next to the page and distills the ledger: run before
// `vite` and `vite build` (the package's dev and build scripts do).
import { DATA_DIR, prepareData } from './data';

for (const line of prepareData()) {
  process.stdout.write(`${DATA_DIR}/${line}\n`);
}
