// The substitution commands (cli.ts dispatches here):
//   splat-scan  <target.wasm> <plain.wasm> [--pieces <d>] [-o <scan.tsv>] [--match [<matches>]] [--symbols <f>]
//               [--ledger <d>] [--map-dir <d>]
//   splat-build <target.wasm> <plain.wasm> [--pieces <d>] [--matches <f>] [-o <out.wasm>] [--expect <sha256>]
//               [--symbols <f>] [--ledger <d>] [--map-dir <d>]
// splat-scan reads the match list (--match's path, else <root>/splat/matched.txt) as evidence of names and, with
// --match, appends its new matches to it.
// Defaults follow the urna layout: <root>/target/x.wasm, <root>/build/out/splat/pieces, <root>/splat/matched.txt,
// <root>/splat/symbol_addrs.txt, <root>/splat/scan.tsv, <root>/ledger, <root>/build/out/splat/x.wasm.
import { existsSync, readFileSync, unlinkSync, writeFileSync } from 'node:fs';
import { basename, dirname, join, resolve } from 'node:path';

import { readLedgerTsv } from './ledger';
import { readManifest } from './splat';
import {
  MatchesFailed,
  type ReadingMeter,
  type ScanRow,
  addMatches,
  buildSplat,
  parseMatchList,
  readPieceBodies,
  readingMeterOf,
  renderScan,
  scanSplit,
} from './splat-sub';
import { defaultMapDir, loadOurSide, loadTargetSide, symbolMaps } from './splat-symbols';

function take(args: string[], name: string): string | undefined {
  const i = args.indexOf(name);
  if (i < 0) {
    return undefined;
  }
  const v = args[i + 1];
  args.splice(i, 2);
  return v;
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
}

function setup(args: string[]): Setup {
  const pieces = take(args, '--pieces');
  const symbols = take(args, '--symbols');
  const ledger = take(args, '--ledger');
  const mapDir = take(args, '--map-dir');
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
  };
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
  return symbolMaps(ours, target);
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
  const s = setup(args);
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
  const listArg = take(args, '--matches');
  const out = take(args, '-o');
  const expect = take(args, '--expect');
  const s = setup(args);
  const listPath = listArg ?? join(s.root, 'splat', 'matched.txt');
  const outPath = out ?? join(s.root, 'build', 'out', 'splat', basename(s.targetPath));
  const { matches } = parseMatchList(existsSync(listPath) ? readFileSync(listPath, 'utf8') : '');
  const expected = expect ?? readManifest(s.pieces).source.sha256;
  const maps = loadMaps(s);
  let r: ReturnType<typeof buildSplat>;
  try {
    r = buildSplat(maps, s.pieces, matches, expected);
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
    default:
      return false;
  }
}
