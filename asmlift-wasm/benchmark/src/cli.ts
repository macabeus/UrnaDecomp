// pnpm bench <command> — see benchmark/README.md.
import { existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { join, resolve } from 'node:path';

import { DEFAULT_PER_SETUP, buildRow, loadContext, readRows, selectDataset, sortRows, writeRows } from './dataset';
import { liftCompileFlags } from './flags';
import { RESULTS_DIR, ROWS_JSON, SELECTION_JSON } from './paths';
import { renderBenchReport, renderDiff } from './report';
import { runBench } from './run';
import type { BenchResults, DatasetRow } from './schema';
import { parseMoves, readSelection, rereferencedRows, uncommittedInputs, writeReferencesUpdate, writeSelection } from './selection';
import { classifyObjects, loadSetups } from './setups';

function usage(): never {
  process.stderr.write(`usage: pnpm bench <command> …
  setups [--check]                 the compile-setup registry with each setup's objects; --check fails on drift
                                   from src/Makefile, the library builds' recorded flags and emscripten
  select [--per N]                 what selection from today's ledger and build would change in the frozen dataset
                                   (N rows per setup, default the recorded one); writes nothing
  select --write [--per N]         re-select: a new dataset version in rows.json and selection.json, its
                                   definitions snapshot and input copies; hand-added rows stay; then re-run the baseline
  select --references              the committed rows' references as selection gives them today on the recorded
         [--move <old>=<new>]…     inputs (the same rows); --move takes a link input the build has moved at its new
                                   path; writes nothing
  select --references --write      write them as a new dataset version, recording why; then re-run the baseline
         [--move <old>=<new>]…
         --note "<why>"
  add <index>…                     append hand-picked target functions as rows; then re-run the baseline
  run [--label L] [--setup k,…]    lift, compile and score every row; writes results/<L>.json and results/<L>.md
      [--image I] [--no-check]     (default label: a timestamp; the setup check runs first unless --no-check)
  report <label|results.json>      render a results file's Markdown report again
  diff <a> <b> [-o out.md]         compare two runs (labels or paths)
`);
  process.exit(2);
}

function flag(args: string[], name: string): string | undefined {
  const i = args.indexOf(name);
  if (i < 0) {
    return undefined;
  }
  const v = args[i + 1];
  args.splice(i, 2);
  return v;
}

/** Every value of a repeatable flag. */
function flags(args: string[], name: string): string[] {
  const values: string[] = [];
  for (let v = flag(args, name); v !== undefined; v = flag(args, name)) {
    values.push(v);
  }
  return values;
}

function has(args: string[], name: string): boolean {
  const i = args.indexOf(name);
  if (i < 0) {
    return false;
  }
  args.splice(i, 1);
  return true;
}

function resultsPath(labelOrPath: string): string {
  return labelOrPath.endsWith('.json') ? resolve(labelOrPath) : join(RESULTS_DIR, `${labelOrPath}.json`);
}

function readResults(labelOrPath: string): BenchResults {
  return JSON.parse(readFileSync(resultsPath(labelOrPath), 'utf8')) as BenchResults;
}

function titles(): Map<string, string> {
  return new Map(loadSetups().map((s) => [s.key, s.title]));
}

function countBySetup(rows: readonly DatasetRow[]): string {
  const counts = new Map<string, number>();
  rows.forEach((r) => counts.set(r.setup, (counts.get(r.setup) ?? 0) + 1));
  return [...counts].map(([k, n]) => `${k} ${n}`).join(', ');
}

function main(argv: string[]): void {
  const [cmd, ...args] = argv;
  switch (cmd) {
    case 'setups': {
      const check = has(args, '--check');
      const setups = loadSetups();
      const { checks, standIns } = classifyObjects(setups, setups[0].image);
      for (const s of setups) {
        const c = checks.find((x) => x.key === s.key)!;
        process.stdout.write(`${s.key}  [${c.status}, ${c.objects} object${c.objects === 1 ? '' : 's'}]  ${s.title}\n`);
        for (const [lang, f] of Object.entries(s.flags ?? {})) {
          process.stdout.write(`    ${lang}: ${f}\n    ${' '.repeat(lang.length)}  lifted with: ${liftCompileFlags(f)}\n`);
        }
        process.stdout.write(`    source: ${s.source}\n`);
        c.detail.forEach((d) => process.stdout.write(`    ${c.status}: ${d}\n`));
      }
      process.stdout.write(`stand-ins (link inputs no setup covers): ${standIns.length}\n`);
      if (check && checks.some((c) => c.status === 'drift')) {
        process.stderr.write(`drift: ${checks.filter((c) => c.status === 'drift').map((c) => c.key).join(', ')}\n`);
        process.exitCode = 1;
      }
      return;
    }
    case 'select': {
      const list = (ids: string[]): string => (ids.length === 0 ? '' : ` (${ids.join(', ')})`);
      if (has(args, '--references')) {
        const write = has(args, '--write');
        const note = flag(args, '--note');
        const moves = parseMoves(flags(args, '--move'));
        const previous = readSelection();
        const committed = readRows();
        const rows = rereferencedRows(previous, committed, moves);
        const changed = (field: 'reference' | 'object'): string[] =>
          rows.filter((r, i) => JSON.stringify(r[field]) !== JSON.stringify(committed[i][field])).map((r) => r.id);
        const references = changed('reference');
        const objects = changed('object');
        process.stdout.write(
          `the inputs of dataset version ${previous.version} select the committed ${rows.length} rows; ` +
            `${references.length} take another reference${list(references)}; ${objects.length} another object path${list(objects)}\n`,
        );
        if (!write || references.length + objects.length === 0) {
          return;
        }
        if (note === undefined) {
          throw new Error('a references update records why: --note "<why>"');
        }
        writeRows(rows);
        const sel = writeReferencesUpdate(previous, note, moves);
        process.stdout.write(
          `dataset version ${sel.version}: the rows of version ${sel.references!.rowsOf} with today's references\n` +
            `now commit rows.json, selection.json${moves.size > 0 ? ' and selection-definitions.json' : ''}, run \`pnpm bench run --label baseline\` ` +
            'from that commit, and commit results/baseline.{json,md}\n',
        );
        return;
      }
      const write = has(args, '--write');
      const previous = existsSync(SELECTION_JSON) ? readSelection() : null;
      const per = Number(flag(args, '--per') ?? previous?.per ?? DEFAULT_PER_SETUP);
      const dirty = write ? uncommittedInputs() : [];
      if (dirty.length > 0) {
        throw new Error(`a re-selection records the commit it read its inputs at; commit these first:\n${dirty.join('\n')}`);
      }
      const committed = existsSync(ROWS_JSON) ? readRows() : [];
      const loaded = loadContext({ rebuildDefinitions: true });
      const reads = new Set<string>();
      const ctx = {
        ...loaded,
        read: (path: string): string | null => {
          reads.add(path);
          return loaded.read(path);
        },
      };
      const rows = selectDataset(ctx, committed, per);
      const before = new Map(committed.map((r) => [r.id, JSON.stringify(r)]));
      const after = new Set(rows.map((r) => r.id));
      const dropped = committed.filter((r) => !after.has(r.id)).map((r) => r.id);
      const added = rows.filter((r) => !before.has(r.id)).map((r) => r.id);
      const changed = rows.filter((r) => before.has(r.id) && before.get(r.id) !== JSON.stringify(r)).map((r) => r.id);
      process.stdout.write(
        `today's ledger and build select ${rows.length} rows: ${dropped.length} dropped${list(dropped)}, ${added.length} added${list(added)}, ${changed.length} kept with other fields${list(changed)}\n`,
      );
      if (!write) {
        process.stdout.write(
          `rows.json stays dataset version ${previous?.version ?? '–'}; \`pnpm bench select --write\` re-selects it as a new version\n`,
        );
        return;
      }
      writeRows(rows);
      const sel = writeSelection(ctx, per, previous, reads);
      const manual = rows.filter((r) => r.origin === 'manual').length;
      process.stdout.write(
        `dataset version ${sel.version}: ${rows.length} rows (${manual} hand-added): ${countBySetup(rows)}; selected at ${sel.commit.slice(0, 7)}\n` +
          'now `pnpm bench run --label baseline`, and commit rows.json, selection.json, selection-definitions.json, selection-inputs/ and results/baseline.{json,md} together\n',
      );
      return;
    }
    case 'add': {
      if (args.length === 0) {
        usage();
      }
      const ctx = loadContext();
      const rows = existsSync(ROWS_JSON) ? readRows() : [];
      for (const a of args) {
        const row = buildRow(ctx, Number(a), 'manual');
        rows.push(row);
        process.stdout.write(`added ${row.id} (${row.name ?? 'unnamed'}, ${row.size} B, ${row.features.join(' ') || 'no features'})\n`);
      }
      writeRows(sortRows(rows));
      process.stdout.write('now `pnpm bench run --label baseline`, and commit rows.json and results/baseline.{json,md} together\n');
      return;
    }
    case 'run': {
      const label = flag(args, '--label') ?? new Date().toISOString().replace(/[:.]/g, '-');
      const only = flag(args, '--setup')?.split(',');
      const noCheck = has(args, '--no-check');
      const setups = loadSetups();
      const image = flag(args, '--image') ?? setups[0].image;
      if (!noCheck) {
        const drift = classifyObjects(setups, image).checks.filter((c) => c.status === 'drift');
        if (drift.length > 0) {
          throw new Error(
            `the setup registry drifted from the build (${drift.map((c) => c.key).join(', ')}): \`pnpm bench setups\` says how`,
          );
        }
      }
      const rows = readRows().filter((r) => only === undefined || only.includes(r.setup));
      const results = runBench({ label, rows, image });
      mkdirSync(RESULTS_DIR, { recursive: true });
      writeFileSync(join(RESULTS_DIR, `${label}.json`), `${JSON.stringify(results, null, 1)}\n`);
      writeFileSync(join(RESULTS_DIR, `${label}.md`), renderBenchReport(results, titles()));
      const t = results.totals.overall;
      process.stdout.write(
        `${label}: ${t.rows} rows, ${t.compiled} compiled, ${t.exact} exact at -Oz, median -Oz ratio ${t.medianOptRatio ?? '–'} → results/${label}.{json,md}\n`,
      );
      return;
    }
    case 'report': {
      const r = readResults(args[0] ?? usage());
      process.stdout.write(`${renderBenchReport(r, titles())}\n`);
      return;
    }
    case 'diff': {
      const out = flag(args, '-o');
      if (args.length !== 2) {
        usage();
      }
      const text = renderDiff(readResults(args[0]), readResults(args[1]));
      if (out === undefined) {
        process.stdout.write(`${text}\n`);
      } else {
        writeFileSync(out, `${text}\n`);
      }
      return;
    }
    default:
      usage();
  }
}

main(process.argv.slice(2));
