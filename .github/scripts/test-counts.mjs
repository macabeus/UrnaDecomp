// usage: node .github/scripts/test-counts.mjs <suite> <vitest JSON report> <skip budget>
//
// Prints a suite's passed, failed and skipped counts, names every skipped test, adds them to the job
// summary, and exits 1 when more tests skip than the budget allows. In CI the gated tests skip because
// there is no toolchain image and no build output; the budgets in ci.yml are those counts in a fresh
// clone (.github/README.md lists what each one covers). A count above the budget means a test lost an
// input it used to find, such as the target or the ledger, and passes by skipping.
import { appendFileSync, existsSync, readFileSync } from 'node:fs';

const [suite, reportPath, budgetArg] = process.argv.slice(2);
const budget = Number(budgetArg);
if (!suite || !reportPath || !Number.isInteger(budget)) {
  console.error('usage: node .github/scripts/test-counts.mjs <suite> <vitest JSON report> <skip budget>');
  process.exit(2);
}
if (!existsSync(reportPath)) {
  console.log(`::error title=${suite}::no test report at ${reportPath}; the run stopped before vitest wrote it`);
  process.exit(1);
}

const report = JSON.parse(readFileSync(reportPath, 'utf8'));
const counts = { passed: 0, failed: 0, skipped: 0, todo: 0 };
const skipped = [];
for (const file of report.testResults) {
  const rel = file.name.slice(file.name.indexOf('asmlift-wasm/'));
  for (const t of file.assertionResults) {
    const status = t.status === 'pending' ? 'skipped' : t.status;
    counts[status] = (counts[status] ?? 0) + 1;
    if (status === 'skipped') {
      skipped.push(`${rel} > ${t.fullName}`);
    }
  }
}

const total = Object.values(counts).reduce((a, b) => a + b, 0);
const line = `${suite}: ${counts.passed} passed, ${counts.failed} failed, ${counts.skipped} skipped of ${total} (skip budget ${budget})`;
const over = counts.skipped > budget;
console.log(`::${over ? 'error' : 'notice'} title=${suite} tests::${line}`);
console.log(line);
for (const s of skipped) {
  console.log(`  skipped: ${s}`);
}
if (counts.skipped < budget) {
  console.log(`::notice title=${suite} skip budget::${budget - counts.skipped} fewer skips than the budget; lower it in ci.yml`);
}

if (process.env.GITHUB_STEP_SUMMARY) {
  const md = [
    `### ${suite}`,
    '',
    '| passed | failed | skipped | skip budget | total |',
    '| ---: | ---: | ---: | ---: | ---: |',
    `| ${counts.passed} | ${counts.failed} | ${counts.skipped} | ${budget} | ${total} |`,
    '',
    ...(skipped.length > 0
      ? ['<details><summary>skipped tests</summary>', '', ...skipped.map((s) => `- ${s}`), '', '</details>', '']
      : []),
  ];
  appendFileSync(process.env.GITHUB_STEP_SUMMARY, `${md.join('\n')}\n`);
}

if (over) {
  console.error(`${suite}: ${counts.skipped} tests skipped, more than the budget of ${budget}`);
  process.exit(1);
}
