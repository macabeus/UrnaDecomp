// What the `fn` readout adds for a human or an agent decompiling an APP function: the strings its
// constants point at, who owns each callee, and the vtable slot the function fills. All of it comes
// from the project's ledger directory (strings.tsv, vtables.json, func-attribution.tsv, callgraph.tsv,
// functions.tsv) plus the class map cls2.json; every file is optional and read once per command.
import { existsSync, readFileSync } from 'node:fs';
import { join } from 'node:path';

import type { Instr } from './disasm';
import { type Attribution, type LedgerRow, readAttribution, readLedgerTsv, readOwners } from './ledger';
import { type Module, importedFunc } from './parse';
import type { LedgerString } from './strings';

export interface VtableEntry {
  address: number;
  class: string;
  offsetToTop: number;
  slot: number;
  tableIndex: number;
}

export interface Ledger {
  dir: string;
  strings: LedgerString[];
  /** function index → the vtable slots it fills */
  vtables: Map<number, VtableEntry[]>;
  attribution: Map<number, Attribution>;
  owners: Map<number, string>;
  /** callee → distinct callers, from callgraph.tsv */
  callers: Map<number, number[]>;
  functions: Map<number, LedgerRow>;
  loaded: string[];
}

/** `addr len enc text` rows; text has `\t`/`\n` escaped by the writer, left as written here. */
export function readStrings(path: string): LedgerString[] {
  const out: LedgerString[] = [];
  for (const line of readFileSync(path, 'utf8').split('\n').slice(1)) {
    if (line === '') {
      continue;
    }
    const tab1 = line.indexOf('\t');
    const tab2 = line.indexOf('\t', tab1 + 1);
    const tab3 = line.indexOf('\t', tab2 + 1);
    out.push({
      addr: Number(line.slice(0, tab1)),
      len: Number(line.slice(tab1 + 1, tab2)),
      enc: line.slice(tab2 + 1, tab3),
      text: line.slice(tab3 + 1),
    });
  }
  out.sort((a, b) => a.addr - b.addr);
  return out;
}

export function readVtables(path: string): Map<number, VtableEntry[]> {
  const parsed = JSON.parse(readFileSync(path, 'utf8')) as {
    address: number;
    class: string;
    offset_to_top: number;
    table_indices: number[];
    functions: number[];
  }[];
  const out = new Map<number, VtableEntry[]>();
  for (const v of parsed) {
    v.functions.forEach((f, slot) => {
      const e: VtableEntry = {
        address: v.address,
        class: v.class,
        offsetToTop: v.offset_to_top,
        slot,
        tableIndex: v.table_indices[slot],
      };
      const list = out.get(f);
      if (list === undefined) {
        out.set(f, [e]);
      } else {
        list.push(e);
      }
    });
  }
  return out;
}

export function readCallgraph(path: string): Map<number, number[]> {
  const out = new Map<number, number[]>();
  for (const line of readFileSync(path, 'utf8').split('\n').slice(1)) {
    if (line === '') {
      continue;
    }
    const tab = line.indexOf('\t');
    const caller = Number(line.slice(0, tab));
    const callee = Number(line.slice(tab + 1));
    const list = out.get(callee);
    if (list === undefined) {
      out.set(callee, [caller]);
    } else if (list[list.length - 1] !== caller) {
      list.push(caller);
    }
  }
  return out;
}

/** Load whatever the ledger directory has; `ownersPath` is the class map (tools/libs/cls2.json). */
export function loadLedger(dir: string, ownersPath?: string): Ledger {
  const loaded: string[] = [];
  const opt = <T>(name: string, read: (p: string) => T, empty: T): T => {
    const p = join(dir, name);
    if (!existsSync(p)) {
      return empty;
    }
    loaded.push(name);
    return read(p);
  };
  let owners = new Map<number, string>();
  if (ownersPath !== undefined && existsSync(ownersPath)) {
    owners = readOwners(ownersPath);
    loaded.push(ownersPath);
  }
  return {
    dir,
    strings: opt('strings.tsv', readStrings, []),
    vtables: opt('vtables.json', readVtables, new Map()),
    attribution: opt('func-attribution.tsv', readAttribution, new Map()),
    owners,
    callers: opt('callgraph.tsv', readCallgraph, new Map()),
    functions: opt('functions.tsv', readLedgerTsv, new Map()),
    loaded,
  };
}

/** The owner label of a function, from the ledger's functions.tsv, else the class map, else '-'. */
export function ownerOf(ledger: Ledger, index: number): string {
  return ledger.functions.get(index)?.owner ?? ledger.owners.get(index) ?? '-';
}

export interface CalleeNote {
  index: number;
  sites: number;
  /** `import:<name>` for an import, else the owner label */
  owner: string;
  component: string;
  vtable: string | null;
  /** `<kind> <label>:<index> <name>` from functions.tsv */
  best: string | null;
}

/** Each distinct direct callee with its site count, owner and component. */
export function calleeNotes(
  m: Module,
  instrs: readonly Instr[],
  ledger: Ledger,
  importName: (module: string, field: string, funcIndex: number) => string,
): CalleeNote[] {
  const sites = new Map<number, number>();
  for (const i of instrs) {
    if (i.opcode === 0x10 || i.opcode === 0x12) {
      const callee = (i.imms[0] as { value: number }).value;
      sites.set(callee, (sites.get(callee) ?? 0) + 1);
    }
  }
  return [...sites.entries()].map(([index, n]) => {
    const imp = importedFunc(m, index);
    if (imp !== null) {
      return {
        index,
        sites: n,
        owner: `import:${importName(imp.module, imp.name, index)}`,
        component: '-',
        vtable: null,
        best: null,
      };
    }
    const vt = ledger.vtables.get(index);
    const b = ledger.functions.get(index)?.best ?? null;
    return {
      index,
      sites: n,
      owner: ownerOf(ledger, index),
      component: ledger.attribution.get(index)?.component ?? '-',
      vtable: vt === undefined ? null : `${vt[0].class} slot ${vt[0].slot}`,
      best: b === null ? null : `${b.kind} ${b.label}:${b.index}${b.name === null ? '' : ` ${b.name}`}`,
    };
  });
}

export function vtableNotes(ledger: Ledger, index: number): string[] {
  return (ledger.vtables.get(index) ?? []).map(
    (v) =>
      `${v.class}${v.offsetToTop === 0 ? '' : ` (offset_to_top ${v.offsetToTop})`} slot ${v.slot} (vtable @${v.address}, table[${v.tableIndex}])`,
  );
}
