// The command line: pnpm wasm <command> …   (README lists them)
import { execFileSync, spawnSync } from 'node:child_process';
import { existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { type Census, census, renderCensus } from './census';
import { disassemble, instrText } from './disasm';
import { glueNamesFor, readImportNames } from './glue-file';
import { type ExportNames, exportResolver, importNamesFromGlue, importResolver } from './imports';
import {
  type CandidateModule,
  type LedgerRow,
  ledgerNames,
  ledgerRows,
  ledgerSummary,
  namesViaTwin,
  readAttribution,
  readLedgerTsv,
  readOwners,
  renderLedgerTsv,
  siblingSymbolMap,
} from './ledger';
import { componentFlags, liftFunction, liftNames } from './lift';
import { type LiftInputs, liftInputs as loadLiftInputs, projectDefaults } from './lift-inputs';
import {
  evaluateLift,
  readResultsTsv,
  readSampleTsv,
  renderReport,
  renderResultsTsv,
  renderSampleTsv,
  sampleFunctions,
  summarize,
} from './lift-eval';
import { type NormalizeOptions, normalizeModule, signature } from './normalize';
import { type Extract, extractFunction, findFunction } from './object';
import { type Module, funcAt, parseModule, typeText } from './parse';
import { type GroupBy, movements, renderMovements, renderProgress, snapshots, takeSnapshot } from './progress';
import { DEFAULT_IMAGE, DEFAULT_LINK_FLAGS, candidatesFrom, compileLifted, defaultTemplate, rankWasm } from './rank';
import { type Ledger, loadLedger } from './readout';
import { renderBundle, renderFunctionReport } from './report';
import { renderSideBySide, score } from './score';
import { assembleSplit, pieceGroups, readSubstitutes, splitModule } from './splat';
import { runSplatSubCommand } from './splat-sub-cli';
import { runSplatSymbolsCommand } from './splat-symbols-cli';

function usage(): never {
  process.stderr.write(`usage: cli.ts <command> …
  parse <wasm> [--json] [--functions]        section counts; --functions prints one JSON line per function
  disasm <wasm> <fn>                         instruction stream with raw immediates and body-relative offsets
  fingerprint <wasm> [--depth N] [<fn>]      index, size, fingerprint, addr, shape (TSV); one function or all
  score <wasmA> <fnA> <wasmB> <fnB> [--depth N] [--full] [--json]
                                             row diff of B's function against A's (A is the target)
  census <wasmA> <wasmB> [--depth N] [--json] [--unmatched N] [--pairs]
                                             match A's functions to B's by fingerprint, then addr, then shape;
                                             --pairs lists them with B's names from its x.js.symbols
  fn <wasm> <fn> [--depth N] [--ledger <dir>] [--owners <cls2.json>]
                                             disasm + normalised rows + folded readout, with the ledger's strings,
                                             callee owners and vtable slots (+ wasm-decompile when on PATH)
  rank <target.wasm> <fn> --extract export:<name>|sym:<name>|index:<n> [--template '<sh>'] [--image <docker image>]
       [--link-flags '…'] [--flags '<f> <f>'] [--ext cpp] [--cwd dir] [--depth N] <candidate.cpp>…
  bundle <wasm> --name <n> [--out-dir <dir>] [--file <indices.txt>] [--no-disasm] [--depth N] [--ledger <dir>] <fn>…
                                             one Markdown file (<ledger>/bundles/<n>.md) with a header table and the
                                             'fn' readout of every listed function
  ledger <target.wasm> [-o functions.tsv] [--owners <cls2.json>] [--attribution <func-attribution.tsv>] [--depth N]
         <label>=<candidate.wasm>[@<named-twin.wasm>]…
                                             one row per target function with its best match (the census);
                                             a plain link gets its names from its --emit-symbol-map twin via @
  progress [--ledger <dir>] [--tsv <functions.tsv>] [--against <prev.tsv>] [--by owner|component] [--moves N]
           [--snapshot [<label>]]            the census tiers per owner (or component) with the delta since the previous
                                             snapshot (<ledger>/history/<stamp>.tsv); --snapshot saves the current one
  targets <target.wasm> --tsv <new.tsv> [--against <prev.tsv>] [--file <indices.txt>] [--rows N] [--depth N]
         [<label>=<candidate.wasm>…] <fn>…   per listed function: previous → new kind/score and the first differing
                                             rows against the best candidate function
  import-names <glue.js> [-o names.json]     { minified field: real name } from the glue's wasmImports table
  lift <wasm> (<fn>…|--all) [-o <dir>] [--names <functions.tsv>] [--attribution <func-attribution.tsv>]
       [--mangled <mangled.txt>:<demangled.txt>] [--check] [--image <docker image>]
                                             rough C++ for each function (<dir>/<index>.cpp, or stdout); --all lifts
                                             every defined function and reports the throws; --check compiles each
                                             written file with em++ -c at its component's flags
  lift-eval <wasm> [--label <l>] [--out <dir>] [--size N] [--seed N] [--sample <sample.tsv>] [--against <dir>]
            [--image <docker image>] [--names …] [--attribution …] [--mangled …]
                                             lift a stratified sample (component × size band), compile it in one
                                             container, score each at depth 0; writes report.md, results.tsv and
                                             sample.tsv to <out> (default <ledger>/lift-eval/<label>); --against
                                             a previous run's dir adds its movement
  splat-split <wasm> -o <dir> [--names <functions.tsv>] [--attribution <func-attribution.tsv>] [--no-wat]
                                             the module as pieces: asm/<group>/<index>[-<name>].bin (+ .wat),
                                             data/<n>-<addr>.bin, custom/…, manifest.json; then reassembles them
                                             and fails unless the result is the input's bytes
  splat-assemble <dir> -o <wasm> [--substitute <dir|<index>[-<name>].bin>]…
                                             the module from the pieces alone, each substitute's body in place of
                                             its function's piece
  splat-symbols <target.wasm> <plain.wasm> [-o <symbol_addrs.txt>]   the target's symbol file from our link's maps
  splat-maps <target.wasm> <plain.wasm> (--fn <ours> | --data <addr> | --target-fn <n> | --summary)
  splat-remap <target.wasm> <plain.wasm> <ours> [--against <target fn>]   one body re-pointed into the target
  splat-sym-coverage <target.wasm> <plain.wasm> [--smoke N]          symbol coverage and ledger pairs re-pointed
  splat-twin-check <plain.wasm> <twin.wasm>   the --emit-symbol-map twin's bodies against the plain link's
                                             (src/splat-symbols-cli.ts; urna layout defaults)
  splat-scan <target.wasm> <plain.wasm> [-o <scan.tsv>] [--claim [<claims>]]   every target function against our
                                             re-pointed candidate: match / differs / unresolved / no-candidate
  splat-build <target.wasm> <plain.wasm> [--claims <f>] [-o <wasm>] [--expect <sha256>]   the split assembled with
                                             our bytes for every claim; fails on a claim that no longer verifies
                                             (src/splat-sub-cli.ts; urna layout defaults)
Every command that normalises takes --import-names <names.json> (module A / the target) and --import-names-b
<names.json> (module B / a candidate); without them the glue beside the module (x.wasm → x.js) is read when present.
<fn> is a function index, export:<name> or sym:<name> (relocatable objects).
`);
  process.exit(2);
}

function readModule(path: string): Module {
  return parseModule(new Uint8Array(readFileSync(path)));
}

function selector(s: string): Extract {
  if (s.startsWith('export:')) {
    return { kind: 'export', name: s.slice(7) };
  }
  if (s.startsWith('sym:')) {
    return { kind: 'symbol', name: s.slice(4) };
  }
  if (s.startsWith('index:')) {
    s = s.slice(6);
  }
  if (!/^\d+$/.test(s)) {
    throw new Error(`bad function selector '${s}'`);
  }
  return { kind: 'index', index: Number(s) };
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

function has(args: string[], name: string): boolean {
  const i = args.indexOf(name);
  if (i < 0) {
    return false;
  }
  args.splice(i, 1);
  return true;
}

interface ModuleOpts extends NormalizeOptions {
  importName: NonNullable<NormalizeOptions['importName']>;
  exportName: (realName: string) => string | undefined;
  exports: ExportNames | null;
}

/** Normalisation and lookup options for the module at `path`: import names from an explicit file or
 *  the sibling glue, export names from the glue. */
function moduleOpts(path: string, depth: number, explicitImports: string | undefined): ModuleOpts {
  const glue = glueNamesFor(path);
  const imports = explicitImports !== undefined ? readImportNames(explicitImports) : (glue?.imports ?? null);
  const from = explicitImports ?? glue?.from;
  if (from !== undefined) {
    process.stderr.write(`names for ${basename(path)}: ${from}\n`);
  }
  const exports = glue?.exports ?? null;
  return { depth, importName: importResolver(imports), exportName: exportResolver(exports), exports };
}

/** `docker run -v /tmp:/tmp` sees only /tmp; the runner creates its compile dirs under os.tmpdir(),
 *  which on macOS is a per-user /var/folders path the daemon cannot reach. */
function pointTmpdirAtDocker(template: string): void {
  if (template.includes('docker') && !tmpdir().startsWith('/tmp')) {
    process.env.TMPDIR = '/tmp';
  }
}

/** The lifter's naming inputs for the module at `wasmPath`, from `--names`, `--attribution` and
 *  `--mangled`, each defaulting to the urna layout around the module. */
function liftInputs(wasmPath: string, args: string[]): LiftInputs {
  const names = flag(args, '--names');
  const attribution = flag(args, '--attribution');
  const mangled = flag(args, '--mangled');
  return loadLiftInputs(wasmPath, {
    ...(names !== undefined ? { names } : {}),
    ...(attribution !== undefined ? { attribution } : {}),
    ...(mangled !== undefined ? { mangled } : {}),
  });
}

/** Function selectors from a file: one per line, `#` comments, extra columns ignored (a ledger TSV works). */
function indexList(path: string): string[] {
  const out: string[] = [];
  for (const raw of readFileSync(path, 'utf8').split('\n')) {
    const line = raw.replace(/#.*/, '').trim();
    if (line === '') {
      continue;
    }
    const first = line.split(/[\s,]+/)[0];
    if (/^(\d+|index:\d+|export:.+|sym:.+)$/.test(first)) {
      out.push(first);
    }
  }
  return out;
}

/** `<label>=<path.wasm>[@<named-twin.wasm>]` */
function candidateSpec(spec: string): { label: string; path: string; twin?: string } {
  const eq = spec.indexOf('=');
  if (eq < 0) {
    throw new Error(`candidate '${spec}' is <label>=<path.wasm>[@<twin.wasm>]`);
  }
  const label = spec.slice(0, eq);
  const rest = spec.slice(eq + 1);
  const at = rest.indexOf('@');
  if (at < 0) {
    return { label, path: rest };
  }
  return { label, path: rest.slice(0, at), twin: rest.slice(at + 1) };
}

async function main(argv: string[]): Promise<void> {
  const [cmd, ...args] = argv;
  switch (cmd) {
    case 'parse': {
      const json = has(args, '--json');
      const functions = has(args, '--functions');
      const m = readModule(args[0] ?? usage());
      const summary = {
        types: m.types.length,
        imports: m.imports.length,
        importedFuncs: m.importedFuncCount,
        functions: m.funcs.length,
        tables: m.tables.length,
        memories: m.memories.length,
        globals: m.globals.length,
        exports: m.exports.length,
        elemSegments: m.elems.length,
        tableEntries: m.elems.reduce((n, e) => n + e.funcIndices.length, 0),
        dataSegments: m.datas.length,
        dataBytes: m.datas.reduce((n, d) => n + d.bytes.length, 0),
        codeBytes: m.funcs.reduce((n, f) => n + f.size, 0),
        customSections: m.customSections,
        symbols: m.symbols.length,
        relocs: m.relocs.reduce((n, r) => n + r.relocs.length, 0),
      };
      if (functions) {
        for (const f of m.funcs) {
          process.stdout.write(
            `${JSON.stringify({ index: f.index, typeIndex: f.typeIndex, params: f.params, results: f.results, locals: f.locals, bodyOffset: f.bodyOffset, size: f.size })}\n`,
          );
        }
        return;
      }
      if (json) {
        process.stdout.write(
          `${JSON.stringify({ ...summary, sections: m.sections, types: m.types.map(typeText), imports: m.imports, exports: m.exports, globals: m.globals }, null, 2)}\n`,
        );
        return;
      }
      for (const [k, v] of Object.entries(summary)) {
        process.stdout.write(`${k.padEnd(16)} ${Array.isArray(v) ? v.join(',') : v}\n`);
      }
      process.stdout.write('sections:\n');
      for (const s of m.sections) {
        process.stdout.write(
          `  ${s.name.padEnd(10)} start=0x${s.start.toString(16).padStart(8, '0')} size=${s.end - s.start}\n`,
        );
      }
      return;
    }
    case 'disasm': {
      const m = readModule(args[0] ?? usage());
      const fn = findFunction(m, selector(args[1] ?? usage()));
      process.stdout.write(
        `;; function ${fn.index} ${signature(fn)} body@0x${fn.bodyOffset.toString(16)} size ${fn.size}\n`,
      );
      for (const i of disassemble(fn)) {
        process.stdout.write(`${i.offset.toString(16).padStart(6, '0')}  ${instrText(i, m)}\n`);
      }
      return;
    }
    case 'import-names': {
      const out = flag(args, '-o');
      const glue = args[0] ?? usage();
      const names = importNamesFromGlue(readFileSync(glue, 'utf8'));
      if (names === null) {
        throw new Error(`${glue}: no 'var wasmImports={…}' table`);
      }
      const text = `${JSON.stringify(names, null, 1)}\n`;
      if (out === undefined) {
        process.stdout.write(text);
      } else {
        writeFileSync(out, text);
        process.stderr.write(`${Object.keys(names).length} import names → ${out}\n`);
      }
      return;
    }
    case 'fingerprint': {
      const depth = Number(flag(args, '--depth') ?? 2);
      const names = flag(args, '--import-names');
      const path = args[0] ?? usage();
      const m = readModule(path);
      const mo = moduleOpts(path, depth, names);
      const norm = normalizeModule(m, mo);
      const only = args[1] === undefined ? null : findFunction(m, selector(args[1]), mo).index;
      process.stdout.write('index\tsize\tfingerprint\taddr\tshape\n');
      for (const n of norm) {
        if (only !== null && n.index !== only) {
          continue;
        }
        process.stdout.write(`${n.index}\t${funcAt(m, n.index)!.size}\t${n.fingerprint}\t${n.addr}\t${n.shape}\n`);
      }
      return;
    }
    case 'score': {
      const depth = Number(flag(args, '--depth') ?? 2);
      const full = has(args, '--full');
      const json = has(args, '--json');
      const namesA = flag(args, '--import-names');
      const namesB = flag(args, '--import-names-b');
      const [pa, sa, pb, sb] = args;
      if (pb === undefined || sb === undefined) {
        usage();
      }
      const a = extractFunction(readModule(pa), selector(sa), moduleOpts(pa, depth, namesA));
      const b = extractFunction(readModule(pb), selector(sb), moduleOpts(pb, depth, namesB));
      const s = score(
        { rows: a.rows, fn: a.fn, signature: a.signature },
        { rows: b.rows, fn: b.fn, signature: b.signature },
      );
      if (json) {
        process.stdout.write(
          `${JSON.stringify({ score: s.score, rows: s.rows, byteExact: s.byteExact, breakdown: s.breakdown })}\n`,
        );
        return;
      }
      process.stdout.write(`${renderSideBySide(s, { onlyDiff: !full })}\n`);
      return;
    }
    case 'census': {
      const depth = Number(flag(args, '--depth') ?? 2);
      const json = has(args, '--json');
      const unmatched = Number(flag(args, '--unmatched') ?? 40);
      const pairs = has(args, '--pairs');
      const namesA = flag(args, '--import-names');
      const namesB = flag(args, '--import-names-b');
      const [pa, pb] = args;
      if (pb === undefined) {
        usage();
      }
      const ma = readModule(pa);
      const mb = readModule(pb);
      const c: Census = census(
        ma,
        normalizeModule(ma, moduleOpts(pa, depth, namesA)),
        mb,
        normalizeModule(mb, moduleOpts(pb, depth, namesB)),
      );
      if (json) {
        process.stdout.write(`${JSON.stringify(c)}\n`);
        return;
      }
      const symbolsB = siblingSymbolMap(pb);
      process.stdout.write(
        `${renderCensus(c, {
          listUnmatched: unmatched,
          listPairs: pairs,
          nameB: (i) => symbolsB?.get(i),
          sizeA: (i) => funcAt(ma, i)?.size,
        })}\n`,
      );
      return;
    }
    case 'fn': {
      const depth = Number(flag(args, '--depth') ?? 2);
      const names = flag(args, '--import-names');
      const path = args[0] ?? usage();
      const defaults = projectDefaults(path);
      const ledgerDir = flag(args, '--ledger') ?? defaults.ledger;
      const ownersPath = flag(args, '--owners') ?? defaults.owners;
      const m = readModule(path);
      const normOpts = moduleOpts(path, depth, names);
      const fn = findFunction(m, selector(args[1] ?? usage()), normOpts);
      const ledger: Ledger | null = ledgerDir === undefined ? null : loadLedger(ledgerDir, ownersPath);
      process.stdout.write(`${renderFunctionReport(m, fn, { ...normOpts, ledger })}\n`);
      const which = spawnSync('sh', ['-c', 'command -v wasm-decompile'], { encoding: 'utf8' });
      if (which.status === 0) {
        process.stdout.write('\n;; --- wasm-decompile\n');
        try {
          const text = execFileSync('wasm-decompile', [path], { encoding: 'utf8', maxBuffer: 1 << 30 });
          const name = m.funcNames.get(fn.index) ?? `f_${fn.index}`;
          const re = new RegExp(`^(export )?function ${name.replace(/[^\w]/g, '.')}\\b[\\s\\S]*?^}`, 'm');
          const hit = re.exec(text);
          process.stdout.write(hit ? `${hit[0]}\n` : `(function ${name} not found in wasm-decompile output)\n`);
        } catch (e) {
          process.stdout.write(`(wasm-decompile failed: ${e instanceof Error ? e.message.split('\n')[0] : e})\n`);
        }
      } else {
        process.stdout.write('\n;; (wasm-decompile is not on PATH: Homebrew wabt does not ship it)\n');
      }
      return;
    }
    case 'bundle': {
      const depth = Number(flag(args, '--depth') ?? 2);
      const names = flag(args, '--import-names');
      const name = flag(args, '--name');
      const outDir = flag(args, '--out-dir');
      const file = flag(args, '--file');
      const noDisasm = has(args, '--no-disasm');
      const path = args[0] ?? usage();
      if (name === undefined) {
        usage();
      }
      const defaults = projectDefaults(path);
      const ledgerDir = flag(args, '--ledger') ?? defaults.ledger;
      const ownersPath = flag(args, '--owners') ?? defaults.owners;
      const indices = [...args.slice(1), ...(file === undefined ? [] : indexList(file))];
      if (indices.length === 0) {
        throw new Error('bundle: no function indices (arguments or --file)');
      }
      const m = readModule(path);
      const normOpts = moduleOpts(path, depth, names);
      const fns = indices.map((i) => findFunction(m, selector(i), normOpts));
      const ledger: Ledger | null = ledgerDir === undefined ? null : loadLedger(ledgerDir, ownersPath);
      const dir = outDir ?? (ledgerDir === undefined ? '.' : join(ledgerDir, 'bundles'));
      mkdirSync(dir, { recursive: true });
      const out = join(dir, `${name}.md`);
      writeFileSync(out, `${renderBundle(name, m, fns, { ...normOpts, ledger, noDisasm })}\n`);
      process.stderr.write(`${fns.length} functions → ${out}\n`);
      return;
    }
    case 'progress': {
      const by = (flag(args, '--by') ?? 'owner') as GroupBy;
      if (by !== 'owner' && by !== 'component') {
        throw new Error(`--by is owner or component, not '${by}'`);
      }
      const moves = Number(flag(args, '--moves') ?? 25);
      const snapI = args.indexOf('--snapshot');
      let snapshotLabel: string | undefined | false = false;
      if (snapI >= 0) {
        const next = args[snapI + 1];
        snapshotLabel = next !== undefined && !next.startsWith('--') ? next : undefined;
        args.splice(snapI, snapshotLabel === undefined ? 1 : 2);
      }
      const ledgerDir = flag(args, '--ledger') ?? join(dirname(fileURLToPath(import.meta.url)), '../../ledger');
      const tsvPath = flag(args, '--tsv') ?? join(ledgerDir, 'functions.tsv');
      const historyDir = join(ledgerDir, 'history');
      const history = snapshots(historyDir);
      const against = flag(args, '--against') ?? history[history.length - 1];
      const cur = readLedgerTsv(tsvPath);
      const prev = against === undefined ? null : readLedgerTsv(against);
      process.stdout.write(
        `ledger ${tsvPath} (${cur.size} rows)${against === undefined ? ' — no previous snapshot' : ` vs ${against}`}\n\n`,
      );
      process.stdout.write(`${renderProgress(cur, prev, by)}\n`);
      if (prev !== null && moves > 0) {
        process.stdout.write(`\n${renderMovements(movements(cur, prev), moves)}\n`);
      }
      if (snapshotLabel !== false) {
        const saved = takeSnapshot(tsvPath, historyDir, snapshotLabel);
        process.stdout.write(`\nsnapshot → ${saved}\n`);
      }
      return;
    }
    case 'targets': {
      const depth = Number(flag(args, '--depth') ?? 2);
      const rowsN = Number(flag(args, '--rows') ?? 12);
      const namesA = flag(args, '--import-names');
      const tsvPath = flag(args, '--tsv');
      const against = flag(args, '--against');
      const file = flag(args, '--file');
      const [targetPath, ...rest] = args;
      if (targetPath === undefined || tsvPath === undefined) {
        usage();
      }
      const specs = rest.filter((a) => a.includes('='));
      const indices = [...rest.filter((a) => !a.includes('=')), ...(file === undefined ? [] : indexList(file))];
      const cur = readLedgerTsv(tsvPath);
      const prev = against === undefined ? null : readLedgerTsv(against);
      const target = readModule(targetPath);
      const targetOpts = moduleOpts(targetPath, depth, namesA);
      const targetNorm = normalizeModule(target, targetOpts);
      const cands = new Map<string, { module: Module; norm: ReturnType<typeof normalizeModule> }>();
      for (const spec of specs) {
        const c = candidateSpec(spec);
        const module = readModule(c.path);
        cands.set(c.label, { module, norm: normalizeModule(module, moduleOpts(c.path, depth, undefined)) });
      }
      const fmt = (r: LedgerRow | undefined): string =>
        r === undefined
          ? '?'
          : r.best === null
            ? 'none'
            : `${r.best.kind}${r.best.score === 0 ? '' : `/${r.best.score}`}`;
      for (const sel of indices) {
        const fn = findFunction(target, selector(sel), targetOpts);
        const row = cur.get(fn.index);
        const head = `${fn.index} ${fn.size} B ${row?.owner ?? '-'}/${row?.component ?? '-'}: ${prev === null ? '' : `${fmt(prev.get(fn.index))} → `}${fmt(row)}${row?.best ? ` ${row.best.label}:${row.best.index}${row.best.name === null ? '' : ` ${row.best.name}`}` : ''}`;
        process.stdout.write(`${head}\n`);
        const b = row?.best;
        if (!b || b.kind === 'byte' || b.kind === 'fingerprint') {
          continue;
        }
        const cand = cands.get(b.label);
        if (cand === undefined) {
          continue;
        }
        const na = targetNorm[fn.index - target.importedFuncCount];
        const bfn = funcAt(cand.module, b.index)!;
        const nb = cand.norm[b.index - cand.module.importedFuncCount];
        const s = score(
          { rows: na.rows, fn, signature: signature(fn) },
          { rows: nb.rows, fn: bfn, signature: signature(bfn) },
        );
        const diff = renderSideBySide(s, { onlyDiff: true }).split('\n');
        process.stdout.write(
          `${diff
            .slice(0, rowsN + 1)
            .map((l) => `    ${l}`)
            .join('\n')}${diff.length > rowsN + 1 ? `\n    … ${diff.length - rowsN - 1} more rows` : ''}\n`,
        );
      }
      return;
    }
    case 'splat-split': {
      const out = flag(args, '-o');
      const wat = !has(args, '--no-wat');
      const ledgerDir = args[0] === undefined ? undefined : projectDefaults(args[0]).ledger;
      const namesPath = flag(args, '--names') ?? (ledgerDir && join(ledgerDir, 'functions.tsv'));
      const attributionPath = flag(args, '--attribution') ?? (ledgerDir && join(ledgerDir, 'func-attribution.tsv'));
      const wasmPath = args[0];
      if (wasmPath === undefined || out === undefined) {
        usage();
      }
      const input = new Uint8Array(readFileSync(wasmPath));
      const m = parseModule(input);
      const glue = glueNamesFor(wasmPath);
      const rows = namesPath !== undefined && existsSync(namesPath) ? readLedgerTsv(namesPath) : undefined;
      const attribution =
        attributionPath !== undefined && existsSync(attributionPath) ? readAttribution(attributionPath) : undefined;
      const names = liftNames(m, {
        imports: glue?.imports ?? null,
        exports: glue?.exports ?? null,
        ...(rows ? { ledger: ledgerNames(rows) } : {}),
      });
      const groups = pieceGroups(
        m.funcs.map((f) => f.index),
        new Map([...(attribution ?? [])].map(([i, a]) => [i, a.component])),
        new Map([...(rows ?? [])].map(([i, r]) => [i, r.owner])),
      );
      const manifest = splitModule(input, out, { names, groups, wat });
      const named = manifest.functions.filter((f) => f.name !== null).length;
      process.stderr.write(
        `${manifest.functions.length} functions (${named} named, ${groups.size} grouped; names ${glue?.from ?? 'no glue'} + ${namesPath ?? 'no ledger'}), ` +
          `${manifest.data.length} data segments, ${manifest.sections.length} sections → ${out}\n`,
      );
      const back = assembleSplit(out);
      if (!back.identical) {
        throw new Error(
          `${out}: the pieces reassemble to sha256 ${back.sha256}, not the input's ${manifest.source.sha256}`,
        );
      }
      process.stderr.write(
        `round trip: the pieces reassemble to sha256 ${back.sha256} (${back.bytes.length} bytes), the input's\n`,
      );
      return;
    }
    case 'splat-assemble': {
      const out = flag(args, '-o');
      const subs: string[] = [];
      for (let s = flag(args, '--substitute'); s !== undefined; s = flag(args, '--substitute')) {
        subs.push(s);
      }
      const dir = args[0];
      if (dir === undefined || out === undefined) {
        usage();
      }
      const substitutes = readSubstitutes(subs);
      const r = assembleSplit(dir, substitutes);
      writeFileSync(out, r.bytes);
      process.stderr.write(
        `${out}: ${r.bytes.length} bytes, sha256 ${r.sha256} (${r.identical ? 'identical to' : 'differs from'} the split's source)\n` +
          `substitutes: ${substitutes.size} read, ${r.substituted.length} differ from their piece (${r.substitutedBytes} body bytes), ${r.unchanged.length} equal it\n`,
      );
      return;
    }
    case 'ledger': {
      const depth = Number(flag(args, '--depth') ?? 2);
      const out = flag(args, '-o');
      const namesA = flag(args, '--import-names');
      const [targetPath, ...cands] = args;
      if (targetPath === undefined) {
        usage();
      }
      const defaults = projectDefaults(targetPath);
      const ownersPath = flag(args, '--owners') ?? defaults.owners;
      const attributionPath =
        flag(args, '--attribution') ??
        (defaults.ledger !== undefined && existsSync(join(defaults.ledger, 'func-attribution.tsv'))
          ? join(defaults.ledger, 'func-attribution.tsv')
          : undefined);
      const outPath = out ?? (defaults.ledger === undefined ? undefined : join(defaults.ledger, 'functions.tsv'));
      const target = readModule(targetPath);
      const targetNorm = normalizeModule(target, moduleOpts(targetPath, depth, namesA));
      const candidates: CandidateModule[] = cands.map((spec) => {
        const { label, path, twin } = candidateSpec(spec);
        const module = readModule(path);
        const norm = normalizeModule(module, moduleOpts(path, depth, undefined));
        let names = siblingSymbolMap(path);
        if (twin !== undefined) {
          const twinNames = siblingSymbolMap(twin);
          if (twinNames === null) {
            throw new Error(`${twin}: no .js.symbols beside it (link the twin with --emit-symbol-map)`);
          }
          const tm = readModule(twin);
          names = namesViaTwin(module, norm, tm, normalizeModule(tm, moduleOpts(twin, depth, undefined)), twinNames);
          process.stderr.write(
            `${label}: ${names.size}/${module.funcs.length} functions named through ${basename(twin)}\n`,
          );
        }
        return { label, module, norm, ...(names === null ? {} : { names }) };
      });
      const { rows, censuses } = ledgerRows(target, targetNorm, candidates, {
        ...(ownersPath !== undefined && existsSync(ownersPath) ? { owners: readOwners(ownersPath) } : {}),
        ...(attributionPath !== undefined ? { attribution: readAttribution(attributionPath) } : {}),
      });
      const tsv = renderLedgerTsv(rows);
      if (outPath === undefined) {
        process.stdout.write(tsv);
      } else {
        writeFileSync(outPath, tsv);
        process.stderr.write(`${rows.length} rows → ${outPath}\n`);
      }
      for (const [label, c] of censuses) {
        process.stderr.write(
          `${label}: ${c.counts.b} functions; ${c.counts.byFingerprint} by fingerprint + ${c.counts.byShape} by shape\n`,
        );
      }
      process.stderr.write(`${ledgerSummary(rows)}\n`);
      return;
    }
    case 'lift': {
      const out = flag(args, '-o');
      const all = has(args, '--all');
      const check = has(args, '--check');
      const image = flag(args, '--image') ?? DEFAULT_IMAGE;
      const wasmPath = args[0] ?? usage();
      const { m, names, mangled, attribution } = liftInputs(wasmPath, args);
      const indices = all ? m.funcs.map((f) => f.index) : args.slice(1).map((s) => findFunction(m, selector(s)).index);
      if (indices.length === 0) {
        usage();
      }
      if (out !== undefined) {
        mkdirSync(out, { recursive: true });
      }
      const notes = new Map<string, number>();
      const throws: string[] = [];
      const written: string[] = [];
      const t0 = Date.now();
      for (const index of indices) {
        const component = attribution?.get(index)?.component;
        try {
          const lifted = liftFunction(m, index, {
            names,
            ...(mangled ? { mangled } : {}),
            flags: componentFlags(component),
          });
          for (const n of lifted.notes) {
            notes.set(n, (notes.get(n) ?? 0) + 1);
          }
          if (out !== undefined) {
            const file = join(out, `${index}.cpp`);
            writeFileSync(file, lifted.source);
            written.push(file);
          } else if (!all) {
            process.stdout.write(lifted.source);
          }
        } catch (e) {
          throws.push(`${index}: ${e instanceof Error ? e.message : String(e)}`);
        }
      }
      process.stderr.write(
        `lifted ${indices.length - throws.length}/${indices.length} in ${Date.now() - t0} ms, ${throws.length} threw\n`,
      );
      for (const t of throws.slice(0, 20)) {
        process.stderr.write(`  threw ${t}\n`);
      }
      for (const [n, c] of [...notes].sort((a, b) => b[1] - a[1])) {
        process.stderr.write(`  ${String(c).padStart(6)} functions: ${n}\n`);
      }
      if (check) {
        if (out === undefined) {
          throw new Error('--check compiles the files -o writes; pass -o <dir>');
        }
        const failed = compileLifted(
          resolve(out),
          written.map((f) => basename(f)),
          image,
        );
        process.stderr.write(`compiled ${written.length - failed.length}/${written.length} with em++ -c\n`);
        for (const f of failed.slice(0, 20)) {
          process.stderr.write(`  failed ${f} (${join(out, `${f}.err`)})\n`);
        }
      }
      if (throws.length > 0) {
        process.exitCode = 1;
      }
      return;
    }
    case 'lift-eval': {
      const label = flag(args, '--label') ?? 'baseline';
      const outFlag = flag(args, '--out');
      const size = Number(flag(args, '--size') ?? 150);
      const seed = Number(flag(args, '--seed') ?? 1);
      const samplePath = flag(args, '--sample');
      const againstDir = flag(args, '--against');
      const image = flag(args, '--image') ?? DEFAULT_IMAGE;
      const wasmPath = args[0] ?? usage();
      const { m, names, mangled, attribution } = liftInputs(wasmPath, args);
      const ledgerDir = projectDefaults(wasmPath).ledger;
      const out = resolve(outFlag ?? (ledgerDir === undefined ? usage() : join(ledgerDir, 'lift-eval', label)));
      const lifted = join(out, 'lifted');
      mkdirSync(lifted, { recursive: true });
      const sample =
        samplePath === undefined ? sampleFunctions(m, attribution, { size, seed }) : readSampleTsv(samplePath);
      writeFileSync(join(out, 'sample.tsv'), renderSampleTsv(sample));
      const glue = glueNamesFor(wasmPath);
      process.stderr.write(`lift-eval ${label}: ${sample.length} functions → ${out}\n`);
      let result;
      try {
        result = evaluateLift(m, sample, {
          lift: { names, ...(mangled ? { mangled } : {}) },
          attribution,
          dir: lifted,
          image,
          importName: importResolver(glue?.imports ?? null),
        });
      } catch (e) {
        if (e instanceof Error && e.message.startsWith('docker could not run')) {
          throw new Error(
            `${e.message}\n(OrbStack keeps its data on /Volumes/Extreme Pro: mount that drive and start OrbStack)`,
          );
        }
        throw e;
      }
      const againstResults = againstDir === undefined ? undefined : join(resolve(againstDir), 'results.tsv');
      writeFileSync(join(out, 'results.tsv'), renderResultsTsv(result.rows));
      writeFileSync(
        join(out, 'report.md'),
        renderReport(result.rows, {
          label,
          target: wasmPath,
          image,
          seed: samplePath === undefined ? seed : null,
          sampleFrom: samplePath ?? 'sample.tsv',
          liftMs: result.liftMs,
          compileMs: result.compileMs,
          ...(againstDir !== undefined && againstResults !== undefined
            ? { against: { label: basename(resolve(againstDir)), rows: readResultsTsv(againstResults) } }
            : {}),
        }),
      );
      process.stdout.write(`${JSON.stringify({ label, out, ...summarize(result.rows) })}\n`);
      return;
    }
    case 'rank': {
      const image = flag(args, '--image') ?? DEFAULT_IMAGE;
      const linkFlags = flag(args, '--link-flags') ?? DEFAULT_LINK_FLAGS;
      const template = flag(args, '--template') ?? defaultTemplate(image, linkFlags);
      const extractS = flag(args, '--extract');
      const flagsS = flag(args, '--flags');
      const ext = flag(args, '--ext') ?? 'cpp';
      const cwd = flag(args, '--cwd');
      const depth = Number(flag(args, '--depth') ?? 2);
      const json = has(args, '--json');
      const namesA = flag(args, '--import-names');
      const [targetPath, sel, ...files] = args;
      if (extractS === undefined || files.length === 0) {
        usage();
      }
      pointTmpdirAtDocker(template);
      const target = readModule(targetPath);
      const targetOpts = moduleOpts(targetPath, depth, namesA);
      const fn = findFunction(target, selector(sel), targetOpts);
      const candidates = candidatesFrom(files.map((f) => ({ name: basename(f), source: readFileSync(f, 'utf8') })));
      const { ranked, compiled } = await rankWasm({ module: target, index: fn.index }, candidates, {
        template,
        extract: selector(extractS),
        ext,
        depth,
        importName: targetOpts.importName,
        ...(flagsS ? { flags: flagsS.split(/\s+/).filter(Boolean) } : {}),
        ...(cwd ? { cwd } : {}),
      });
      if (json) {
        process.stdout.write(
          `${JSON.stringify({
            winner: {
              variations: ranked.winner.variations,
              score: ranked.winner.score.score,
              rows: ranked.winner.score.rows,
              byteExact: ranked.winner.score.byteExact,
            },
            candidates: ranked.candidates.map((c) => ({
              variations: c.variations,
              score: c.score.score,
              rows: c.score.rows,
              byteExact: c.score.byteExact,
            })),
            dropped: ranked.dropped,
          })}\n`,
        );
        return;
      }
      for (const c of ranked.candidates) {
        const r = compiled.get(c.source);
        process.stdout.write(
          `${String(c.score.score).padStart(6)} / ${String(c.score.rows).padEnd(6)} ${c.score.byteExact ? 'BYTE-EXACT ' : ''}${r?.relocatable ? '(object) ' : ''}${c.variations.join('/')}\n`,
        );
      }
      for (const d of ranked.dropped) {
        process.stdout.write(`dropped ${d.variations.join('/')}: ${d.error}\n`);
      }
      process.stdout.write(
        `\nwinner ${ranked.winner.variations.join('/')}\n${renderSideBySide(ranked.winner.score, { onlyDiff: true })}\n`,
      );
      return;
    }
    default:
      if (!runSplatSymbolsCommand(cmd, args) && !runSplatSubCommand(cmd, args)) {
        usage();
      }
  }
}

// `| head` closes the pipe early; that is not an error worth a stack trace
process.stdout.on('error', (e: NodeJS.ErrnoException) => {
  if (e.code === 'EPIPE') {
    process.exit(0);
  }
  throw e;
});

main(process.argv.slice(2)).catch((e: unknown) => {
  process.stderr.write(`${e instanceof Error ? (e.stack ?? e.message) : String(e)}\n`);
  process.exit(1);
});
