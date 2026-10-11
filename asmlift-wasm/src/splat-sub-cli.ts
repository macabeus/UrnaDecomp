// The substitution commands (cli.ts dispatches here):
//   splat-scan  <target.wasm> <plain.wasm> [--pieces <d>] [-o <scan.tsv>] [--match [<matches>]] [--symbols <f>]
//               [--ledger <d>] [--map-dir <d>] [--kinds <kinds.json> | --no-kinds]
//   splat-build <target.wasm> <plain.wasm> [--pieces <d>] [--matches <f>] [-o <out.wasm>] [--expect <sha256>]
//               [--symbols <f>] [--ledger <d>] [--map-dir <d>] [--kinds <kinds.json> | --no-kinds]
//               [--max-kind-unverified <n>]
//   splat-kinds <plain.wasm> [--shifted <dir>] [--map-dir <d>] [-o <kinds.json>] [--matches <f>]
// splat-scan reads the match list (--match's path, else <root>/splat/matched.txt) as evidence of names and, with
// --match, appends its new matches to it.
// --kinds is the operand-kind table (splat-kinds.ts, `make -C src link-kinds`), by default the one beside the plain
// link; the commands refuse to run without one unless given --no-kinds, which checks every match kind-unverified (each
// ambiguous operand keeps every reading).
// --max-kind-unverified fails splat-build (and removes its output) when more matches than that are kind-unverified;
// make -C src splat passes SPLAT_MAX_KIND_UNVERIFIED (src/mk/splat-sub.mk).
// splat-kinds compares the plain link with its shifted relink (<shifted dir>/vota_web_wasm.wasm, its names.tsv and
// names.sha256, tools/round/link-kinds.sh) and writes the table; it refuses a map dir or a names.tsv that does not
// describe the link it reads.
// Defaults follow the urna layout: <root>/target/x.wasm, <root>/build/out/splat/pieces, <root>/splat/matched.txt,
// <root>/splat/symbol_addrs.txt, <root>/splat/scan.tsv, <root>/ledger, <root>/build/out/splat/x.wasm,
// <root>/build/out/wasm-kinds/<plain dir>/kinds.json.
import { createHash } from 'node:crypto';
import { existsSync, readFileSync, unlinkSync, writeFileSync } from 'node:fs';
import { basename, dirname, join, resolve } from 'node:path';

import { readLedgerTsv } from './ledger';
import { parseModule } from './parse';
import { readManifest } from './splat';
import {
  type OperandKinds,
  classifyOperands,
  defaultKindsPath,
  kindsPathOf,
  kindsSide,
  kindsSummary,
  noMoreArgs,
  readOperandKinds,
  renderOperandKinds,
} from './splat-kinds';
import {
  MatchesFailed,
  type ReadingMeter,
  type ScanRow,
  addMatches,
  buildSplat,
  kindUnverifiedOverLimit,
  parseMatchList,
  readPieceBodies,
  readingMeterOf,
  renderScan,
  scanSplit,
} from './splat-sub';
import { defaultMapDir, loadOurSide, loadTargetSide, readOurNames, symbolMaps } from './splat-symbols';

function take(args: string[], name: string): string | undefined {
  const i = args.indexOf(name);
  if (i < 0) {
    return undefined;
  }
  const v = args[i + 1];
  args.splice(i, 2);
  return v;
}

/** A flag that takes a count: its value, undefined when the flag is absent; throws when its value is not a count (a
 *  flag left without one, such as an empty make variable). Taken before the other flags, so it cannot take theirs. */
function takeCount(args: string[], name: string): number | undefined {
  const i = args.indexOf(name);
  if (i < 0) {
    return undefined;
  }
  const v = args[i + 1];
  if (v === undefined || !/^\d+$/.test(v)) {
    throw new Error(`${name} takes a count, not '${v ?? ''}'`);
  }
  args.splice(i, 2);
  return Number(v);
}

/** `--match` with an optional path: true when bare */
function takeOptional(args: string[], name: string): string | true | undefined {
  const i = args.indexOf(name);
  if (i < 0) {
    return undefined;
  }
  const v = args[i + 1];
  if (v === undefined || v.startsWith('-') || v.endsWith('.wasm')) {
    args.splice(i, 1);
    return true;
  }
  args.splice(i, 2);
  return v;
}

interface Setup {
  root: string;
  targetPath: string;
  plainPath: string;
  pieces: string;
  ledgerDir: string;
  symbolsPath: string;
  mapDir: string;
  /** the operand-kind table, or null to run without one */
  kindsPath: string | null;
}

function setup(cmd: string, args: string[]): Setup {
  const pieces = take(args, '--pieces');
  const symbols = take(args, '--symbols');
  const ledger = take(args, '--ledger');
  const mapDir = take(args, '--map-dir');
  const kindsArg = take(args, '--kinds');
  const noKinds = args.indexOf('--no-kinds');
  if (noKinds >= 0) {
    args.splice(noKinds, 1);
  }
  const [targetPath, plainPath] = args.splice(0, 2);
  if (targetPath === undefined || plainPath === undefined) {
    throw new Error('expected <target.wasm> <plain.wasm>');
  }
  const root = resolve(dirname(targetPath), '..');
  const s: Setup = {
    root,
    targetPath,
    plainPath,
    pieces: pieces ?? join(root, 'build', 'out', 'splat', 'pieces'),
    ledgerDir: ledger ?? join(root, 'ledger'),
    symbolsPath: symbols ?? join(root, 'splat', 'symbol_addrs.txt'),
    mapDir: mapDir ?? defaultMapDir(plainPath),
    kindsPath: kindsPathOf(plainPath, kindsArg, noKinds >= 0),
  };
  noMoreArgs(cmd, args);
  for (const [what, p] of [
    ['the split (make -C src splat-split)', join(s.pieces, 'manifest.json')],
    ['the symbol file (splat-symbols)', s.symbolsPath],
  ] as const) {
    if (!existsSync(p)) {
      throw new Error(`${p} does not exist: ${what}`);
    }
  }
  return s;
}

function loadMaps(s: Setup) {
  const ours = loadOurSide(s.plainPath, s.mapDir);
  const target = loadTargetSide(s.targetPath, s.symbolsPath, s.ledgerDir, ours.stackSize);
  const kinds: OperandKinds | undefined =
    s.kindsPath === null ? undefined : readOperandKinds(s.kindsPath, ours.module.bytes);
  return symbolMaps(ours, target, kinds);
}

/** a reason with its particulars blanked, to count rows per kind of failure */
export function reasonKind(r: ScanRow): string {
  if (r.verdict === 'differs') {
    const operand = r.detail.match(/\[ours \d+(?: = (\S+?)\+\d+)?: (.*)\]$/);
    if (operand !== null) {
      const why = operand[2].startsWith('address ')
        ? `the address resolves (${operand[2].split(' ')[1]}) to another place`
        : operand[2]
            .replace(/^\S+ \((\.\w+)\) has no target symbol/, 'object in $1 has no target symbol')
            .replace(/^\S+: no target symbol; /, 'read-only object: ')
            .replace(/\d+/g, 'N');
      return `differs at an address: ${why}`;
    }
    const m = r.detail.match(
      /target (?:\+0x[0-9a-f]+ )?(\S+(?: locals)?).*\| ours (?:\+0x[0-9a-f]+ )?(\S+(?: locals)?)/,
    );
    if (r.detail.startsWith('ours is longer')) {
      return 'differs: ours is longer';
    }
    return m === null ? `differs: ${r.detail}` : `differs: target ${m[1]} / ours ${m[2]}`;
  }
  if (r.verdict === 'unresolved' && r.detail.startsWith('self-voted')) {
    return `unresolved: self-voted data name, our object's bytes ${r.detail.includes('say nothing') ? 'say nothing' : 'differ'}`;
  }
  if (r.verdict === 'unresolved') {
    return `unresolved: ${r.detail
      .replace(/^ours \+0x[0-9a-f]+ /, '')
      .replace(/^(\S+)[^:]*: /, '$1: ')
      .replace(/0x[0-9a-f]+/g, 'ADDR')
      .replace(/\b\d+\b/g, 'N')
      .replace(/: \S+ has no target (function|symbol)/, ': callee has no target $1')
      .replace(/: \S+ \((\.\w+)\) has no target symbol/, ': data object ($1) has no target symbol')
      .replace(/^(\S+: )offset N: \S+:/, '$1offset N: OBJ:')}`;
  }
  return `${r.verdict}: ${r.detail}`;
}

/** The meter's second axis: how much of a match the target's bytes chose among our readings. */
function readingLines(r: ReadingMeter): string[] {
  const kinds = Object.entries(r.other.byKind)
    .sort((a, b) => b[1] - a[1])
    .map(([k, n]) => `${k} ${n}`)
    .join(', ');
  return [
    'readings the target chose (ambiguous operands: address, table slot or literal; which copy of identical bytes):',
    `  re-pointing alone   ${String(r.preferred.matches).padStart(6)} matches (${r.preferred.bytes} B)`,
    `  another reading     ${String(r.other.matches).padStart(6)} matches (${r.other.bytes} B), ${r.other.operands} operands: ${kinds || '-'}`,
    `  which copy          ${String(r.copy.matches).padStart(6)} matches (${r.copy.bytes} B), ${r.copy.operands} operands`,
  ];
}

function scan(args: string[]): void {
  const out = take(args, '-o');
  const append = takeOptional(args, '--match');
  const s = setup('splat-scan', args);
  const t0 = Date.now();
  const maps = loadMaps(s);
  const manifest = readManifest(s.pieces);
  const bodies = readPieceBodies(s.pieces, manifest);
  const ledgerPath = join(s.ledgerDir, 'functions.tsv');
  const ledger = existsSync(ledgerPath) ? readLedgerTsv(ledgerPath) : new Map();
  const listPath = append === true || append === undefined ? join(s.root, 'splat', 'matched.txt') : append;
  const { matches } = parseMatchList(existsSync(listPath) ? readFileSync(listPath, 'utf8') : '');
  const rows = scanSplit(maps, manifest, bodies, ledger, matches, (pass, n) =>
    process.stderr.write(`pass ${pass}: ${n} matches\n`),
  );
  const outPath = out ?? join(s.root, 'splat', 'scan.tsv');
  writeFileSync(outPath, renderScan(rows, (ours) => maps.ours.fns.get(ours)?.object));
  const w = (x: string): boolean => process.stdout.write(`${x}\n`);
  const count = (f: (r: ScanRow) => boolean): number => rows.filter(f).length;
  const bytes = (f: (r: ScanRow) => boolean): number => rows.filter(f).reduce((n, r) => n + r.size, 0);
  const verdicts = ['match', 'differs', 'unresolved', 'no-candidate'] as const;
  w(`${outPath}: ${rows.length} target functions scanned in ${((Date.now() - t0) / 1000).toFixed(1)} s`);
  for (const v of verdicts) {
    w(`  ${v.padEnd(13)} ${String(count((r) => r.verdict === v)).padStart(6)}  (${bytes((r) => r.verdict === v)} B)`);
  }
  const matched = rows.filter((r) => r.verdict === 'match');
  for (const l of readingLines(readingMeterOf(manifest, new Map(matched.map((r) => [r.index, r.chosen ?? []]))))) {
    w(l);
  }
  w(
    `  matches by source: ${(['symbol', 'ledger', 'content'] as const).map((k) => `${k} ${matched.filter((r) => r.source === k).length}`).join(', ')}`,
  );
  const unverified = matched.filter((r) => r.kindUnverified !== undefined);
  w(
    `  kind-unverified matches: ${unverified.length} (${unverified.reduce((n, r) => n + r.size, 0)} B)${maps.kinds === undefined ? ', no operand-kind table' : ''}`,
  );
  const usedBy = new Map<number, number>();
  for (const r of matched) {
    usedBy.set(r.ours!, (usedBy.get(r.ours!) ?? 0) + 1);
  }
  const shared = [...usedBy.values()].filter((n) => n > 1);
  w(
    `  our functions matching 2+ target functions: ${shared.length} (covering ${shared.reduce((n, k) => n + k, 0)} target functions)`,
  );
  w('ledger best kind: rows → match / differs / unresolved / no-candidate');
  for (const k of ['byte', 'fingerprint', 'addr', 'shape', 'none']) {
    const of = rows.filter((r) => r.ledgerKind === k);
    w(
      `  ${k.padEnd(11)} ${String(of.length).padStart(5)} → ${verdicts.map((v) => of.filter((r) => r.verdict === v).length).join(' / ')}`,
    );
  }
  const reasons = new Map<string, number>();
  for (const r of rows) {
    if (r.verdict === 'differs' || r.verdict === 'unresolved') {
      const k = reasonKind(r);
      reasons.set(k, (reasons.get(k) ?? 0) + 1);
    }
  }
  w('why the rest (differs / unresolved), top 20:');
  for (const [k, n] of [...reasons].sort((a, b) => b[1] - a[1]).slice(0, 20)) {
    w(`  ${String(n).padStart(6)}  ${k}`);
  }
  if (append !== undefined) {
    const before = existsSync(listPath) ? readFileSync(listPath, 'utf8') : '';
    const r = addMatches(before, rows);
    writeFileSync(listPath, r.text);
    w(`${listPath}: ${r.added} matches added`);
  }
}

function build(args: string[]): void {
  const maxKind = takeCount(args, '--max-kind-unverified');
  const listArg = take(args, '--matches');
  const out = take(args, '-o');
  const expect = take(args, '--expect');
  const s = setup('splat-build', args);
  const listPath = listArg ?? join(s.root, 'splat', 'matched.txt');
  const outPath = out ?? join(s.root, 'build', 'out', 'splat', basename(s.targetPath));
  const { matches } = parseMatchList(existsSync(listPath) ? readFileSync(listPath, 'utf8') : '');
  const expected = expect ?? readManifest(s.pieces).source.sha256;
  const maps = loadMaps(s);
  let r: ReturnType<typeof buildSplat>;
  try {
    r = buildSplat(maps, s.pieces, matches, expected);
    const over = maxKind === undefined ? null : kindUnverifiedOverLimit(r, maxKind);
    if (over !== null) {
      throw new Error(`${listPath}: ${over}`);
    }
  } catch (e) {
    if (existsSync(outPath)) {
      unlinkSync(outPath);
    }
    if (e instanceof MatchesFailed) {
      throw new Error(`${listPath}: ${e.message}`);
    }
    throw e;
  }
  writeFileSync(outPath, r.assembled.bytes);
  const w = (x: string): boolean => process.stdout.write(`${x}\n`);
  const pct = (a: number, b: number): string => `${((100 * a) / Math.max(b, 1)).toFixed(2)}%`;
  w(
    `${outPath}: ${r.assembled.bytes.length} bytes, sha256 ${r.assembled.sha256} (expected, ${matches.length} matches from ${listPath})`,
  );
  w(
    `substituted: ${r.matched}/${r.functions} functions (${pct(r.matched, r.functions)}), ${r.matchedBytes}/${r.bytes} code bytes (${pct(r.matchedBytes, r.bytes)})`,
  );
  w(`${'component'.padEnd(24)} ${'functions'.padStart(14)} ${'code bytes'.padStart(20)}`);
  for (const c of r.meter) {
    w(
      `${c.group.padEnd(24)} ${`${c.matched}/${c.functions}`.padStart(14)} ${`${c.matchedBytes}/${c.bytes}`.padStart(20)}  ${pct(c.matchedBytes, c.bytes)}`,
    );
  }
  for (const l of readingLines(r.readings)) {
    w(l);
  }
  w(
    `kind-unverified: ${r.kindUnverified.length}/${r.matched} matches (${r.kindUnverifiedBytes} B) whose operand kinds the shifted link could not tell; their operands kept every reading`,
  );
  if (maps.kinds === undefined) {
    w('  no operand-kind table (--no-kinds)');
  } else {
    for (const u of r.kindUnverified) {
      w(`  line ${u.match.line}: function ${u.match.index} ${u.match.symbol} (ours ${u.ours}): ${u.why}`);
    }
  }
  const kinds = ['past-data', 'target-object', 'past-table'] as const;
  w(
    `review warnings: ${r.reviewWarnings.length} literal operands of matches that may be a target number the source copied (the check cannot tell): ${kinds.map((k) => `${r.reviewWarnings.filter((x) => x.why === k).length} ${k}`).join(', ')}`,
  );
  for (const x of r.reviewWarnings) {
    w(
      `  ${x.why.padEnd(13)} line ${x.match.line}: function ${x.match.index} ${x.match.symbol} (ours ${x.ours}) +0x${x.at.toString(16)} ${x.instr}${x.object === undefined ? '' : ` = ${x.object}`}`,
    );
  }
}

/** The operand-kind table: the plain link against its shifted relink (tools/round/link-kinds.sh). */
function kinds(args: string[]): void {
  const shiftedArg = take(args, '--shifted');
  const mapDir = take(args, '--map-dir');
  const out = take(args, '-o');
  const matchesPath = take(args, '--matches');
  const [plainPath] = args.splice(0, 1);
  if (plainPath === undefined) {
    throw new Error('expected <plain.wasm>');
  }
  noMoreArgs('splat-kinds', args);
  const outPath = out ?? defaultKindsPath(plainPath);
  const shiftedDir = shiftedArg ?? dirname(defaultKindsPath(plainPath));
  const plainMapDir = mapDir ?? defaultMapDir(plainPath);
  const plainBytes = new Uint8Array(readFileSync(plainPath));
  const shiftedBytes = new Uint8Array(readFileSync(join(shiftedDir, 'vota_web_wasm.wasm')));
  // the names must be the two links': the plain map dir's scratch link is the plain link (as loadOurSide checks), and
  // link-kinds.sh records the sha256 of the wasm it named
  const scratch = join(plainMapDir, 'vota_web_wasm.wasm');
  if (!existsSync(scratch) || !Buffer.from(readFileSync(scratch)).equals(Buffer.from(plainBytes))) {
    throw new Error(`${plainMapDir} does not describe ${plainPath}: run make -C src link-map`);
  }
  const namedSha = join(shiftedDir, 'names.sha256');
  const shiftedSha = createHash('sha256').update(shiftedBytes).digest('hex');
  if (!existsSync(namedSha) || readFileSync(namedSha, 'utf8').trim() !== shiftedSha) {
    throw new Error(`${shiftedDir}/names.tsv does not describe its vota_web_wasm.wasm: run make -C src link-kinds`);
  }
  const plain = parseModule(plainBytes);
  const shifted = parseModule(shiftedBytes);
  const t0 = Date.now();
  const k = classifyOperands(
    kindsSide(plain, join(plainMapDir, 'names.tsv')),
    kindsSide(shifted, join(shiftedDir, 'names.tsv')),
  );
  writeFileSync(outPath, renderOperandKinds(k));
  const w = (x: string): boolean => process.stdout.write(`${x}\n`);
  w(
    `${outPath}: data moved by ${k.dataShift}, table by ${k.tableShift}; ${k.functions.size} functions in ${((Date.now() - t0) / 1000).toFixed(1)} s`,
  );
  const show = (label: string, among?: number[]): void => {
    const s = kindsSummary(k, plain, among);
    w(
      `  ${label}: ${s.verified}/${s.functions} kind-verified; operands: ${s.operands.literal} literal, ${s.operands.address} address, ${s.operands.slot} table slot, ${s.operands['table-base']} table base + ${s.operands['table-rel']} relative to it`,
    );
    for (const [why, n] of [...s.unverified].sort((a, b) => b[1] - a[1]).slice(0, 10)) {
      w(`    ${String(n).padStart(6)} kind-unverified: ${why}`);
    }
  };
  show('every function');
  if (matchesPath !== undefined && existsSync(matchesPath)) {
    const byKey = new Map(readOurNames(join(plainMapDir, 'names.tsv')).map((f) => [f.key, f.index]));
    const { matches } = parseMatchList(readFileSync(matchesPath, 'utf8'));
    show(
      `the ${matches.length} matches`,
      matches.map((m) => byKey.get(m.symbol)).filter((x): x is number => x !== undefined),
    );
  }
}

/** true when `cmd` is one of these commands (and it ran) */
export function runSplatSubCommand(cmd: string, args: string[]): boolean {
  switch (cmd) {
    case 'splat-scan':
      scan(args);
      return true;
    case 'splat-build':
      build(args);
      return true;
    case 'splat-kinds':
      kinds(args);
      return true;
    default:
      return false;
  }
}
