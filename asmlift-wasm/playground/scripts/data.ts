// What the page loads besides the module and its glue: the ledger's names, components, owners, best
// matches and strings, distilled from the urna checkout into one JSON file. The inputs are the ones
// `liftInputs` reads, so the page lifts with the names and mangled spellings the CLI lifts with.
import { copyFileSync, existsSync, mkdirSync, readFileSync, statSync, writeFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { disassemble } from '@asmlift/wasm/disasm';
import { ledgerNames, readAttribution, readLedgerTsv, readMangledPairs } from '@asmlift/wasm/ledger';
import { parseModule } from '@asmlift/wasm/parse';
import { readStrings } from '@asmlift/wasm/readout';
import { type LedgerString, referencedStrings } from '@asmlift/wasm/strings';

import type { LedgerData } from '../src/lib/ledger-data';

export const PLAYGROUND_DIR = resolve(dirname(fileURLToPath(import.meta.url)), '..');
export const URNA_ROOT = process.env.URNA_ROOT ?? resolve(PLAYGROUND_DIR, '../..');
export const DATA_DIR = join(PLAYGROUND_DIR, 'public/data');

export const INPUTS = {
  wasm: join(URNA_ROOT, 'target/vota_web_wasm.wasm'),
  glue: join(URNA_ROOT, 'target/vota_web_wasm.js'),
  functions: join(URNA_ROOT, 'ledger/functions.tsv'),
  attribution: join(URNA_ROOT, 'ledger/func-attribution.tsv'),
  strings: join(URNA_ROOT, 'ledger/strings.tsv'),
  mangled: join(URNA_ROOT, 'build/out/wasm/vota_web/symbols.mangled.txt'),
  demangled: join(URNA_ROOT, 'build/out/wasm/vota_web/symbols.demangled.txt'),
};

/** The files the page fetches, under `public/data/` (served from `data/`). */
export const OUTPUTS = {
  wasm: 'vota_web_wasm.wasm',
  glue: 'vota_web_wasm.js',
  ledger: 'ledger.json',
};

function intern(table: string[], ids: Map<string, number>, s: string): number {
  let id = ids.get(s);
  if (id === undefined) {
    id = table.length;
    table.push(s);
    ids.set(s, id);
  }
  return id;
}

/** Every ledger string some defined function's `i32.const` points into: `stringAt` finds the one
 *  string holding an address, so keeping only those changes no function's references. */
export function referencedOnly(wasm: Uint8Array, strings: readonly LedgerString[]): LedgerString[] {
  const m = parseModule(wasm);
  const kept = new Set<LedgerString>();
  for (const fn of m.funcs) {
    for (const r of referencedStrings(disassemble(fn), strings)) {
      kept.add(r.s);
    }
  }
  return strings.filter((s) => kept.has(s));
}

export function distillLedger(inputs: typeof INPUTS = INPUTS): LedgerData {
  const wasm = new Uint8Array(readFileSync(inputs.wasm));
  const m = parseModule(wasm);
  const rows = readLedgerTsv(inputs.functions);
  const attribution = readAttribution(inputs.attribution);
  const names = ledgerNames(rows);
  const tables = { components: [] as string[], owners: [] as string[], evidence: [] as string[], labels: [] as string[] };
  const ids = { components: new Map(), owners: new Map(), evidence: new Map(), labels: new Map() };
  const data: LedgerData = {
    first: m.importedFuncCount,
    count: m.funcs.length,
    components: tables.components,
    owners: tables.owners,
    evidence: tables.evidence,
    labels: tables.labels,
    component: [],
    owner: [],
    evidenceOf: [],
    names: {},
    shapeNames: {},
    best: {},
    mangled: {},
    strings: [],
  };
  for (const fn of m.funcs) {
    const a = attribution.get(fn.index);
    const row = rows.get(fn.index);
    data.component.push(intern(tables.components, ids.components, a?.component ?? '-'));
    data.evidenceOf.push(intern(tables.evidence, ids.evidence, a?.evidence ?? '-'));
    data.owner.push(intern(tables.owners, ids.owners, row?.owner ?? '-'));
    const b = row?.best;
    if (b) {
      data.best[fn.index] = [b.kind, intern(tables.labels, ids.labels, b.label), b.index, b.score];
      if (b.kind === 'shape' && b.name !== null) {
        data.shapeNames[fn.index] = b.name;
      }
    }
  }
  for (const [index, name] of names) {
    data.names[index] = name;
  }
  if (existsSync(inputs.mangled) && existsSync(inputs.demangled)) {
    const pairs = readMangledPairs(inputs.mangled, inputs.demangled);
    for (const name of names.values()) {
      const mangled = pairs.get(name);
      if (mangled !== undefined) {
        data.mangled[name] = mangled;
      }
    }
  }
  data.strings = referencedOnly(wasm, readStrings(inputs.strings)).map((s) => [s.addr, s.len, s.enc, s.text]);
  return data;
}

function copyIfChanged(from: string, to: string): boolean {
  if (existsSync(to)) {
    const a = statSync(from);
    const b = statSync(to);
    if (a.size === b.size && a.mtimeMs <= b.mtimeMs) {
      return false;
    }
  }
  copyFileSync(from, to);
  return true;
}

/** Copy the module and its glue, write ledger.json; returns what it wrote, for the log. */
export function prepareData(dir = DATA_DIR): string[] {
  mkdirSync(dir, { recursive: true });
  const out: string[] = [];
  for (const [from, name] of [
    [INPUTS.wasm, OUTPUTS.wasm],
    [INPUTS.glue, OUTPUTS.glue],
  ] as const) {
    out.push(`${name} ${copyIfChanged(from, join(dir, name)) ? 'copied' : 'up to date'}`);
  }
  const json = JSON.stringify(distillLedger());
  writeFileSync(join(dir, OUTPUTS.ledger), json);
  out.push(`${OUTPUTS.ledger} ${(json.length / 1024).toFixed(0)} KiB`);
  return out;
}
