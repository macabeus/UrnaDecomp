// The per-function readout (`fn`) as a string, so one command (`bundle`) can write it for a whole
// TU cluster at once: whoever decompiles the cluster reads one Markdown file instead of running `fn` forty times
// (each run pays tsx start-up plus the whole-module normalisation, ~4 s; a 50-function bundle pays
// them once).
import { basename } from 'node:path';

import { instrText } from './disasm';
import { foldFunction } from './fold';
import type { ExportNames } from './imports';
import { type NormalizeOptions, type NormalizedFn, normalizeModule, signature } from './normalize';
import { extractFunction } from './object';
import { type Func, type Module, importedFunc } from './parse';
import { type Ledger, calleeNotes, vtableNotes } from './readout';
import { referencedStrings } from './strings';

export interface ReportOptions extends NormalizeOptions {
  importName: NonNullable<NormalizeOptions['importName']>;
  exports: ExportNames | null;
  ledger: Ledger | null;
  /** the module's normalisation, computed once for a bundle */
  norm?: NormalizedFn[];
  /** leave out the two-column disassembly (the folded readout and the tables stay) */
  noDisasm?: boolean;
}

/** The table slots whose elem entry names the function. */
export function tableSlotsOf(m: Module, index: number): number[] {
  const slots: number[] = [];
  for (const e of m.elems) {
    const base = e.offset?.op === 'i32.const' ? (e.offset.value as number) : 0;
    e.funcIndices.forEach((f, i) => {
      if (f === index) {
        slots.push(base + i);
      }
    });
  }
  return slots;
}

/** `<kind> <label>:<index> [name] [score N]` from a ledger row, or null. */
export function bestText(ledger: Ledger | null, index: number): string | null {
  const b = ledger?.functions.get(index)?.best;
  if (!b) {
    return null;
  }
  return `${b.kind} ${b.label}:${b.index}${b.name === null ? '' : ` ${b.name}`}${b.score === 0 ? '' : ` score ${b.score}`}`;
}

/** Everything `fn` prints for one function except the optional wasm-decompile block. */
export function renderFunctionReport(m: Module, fn: Func, opts: ReportOptions): string {
  const out: string[] = [];
  const depth = opts.depth ?? 2;
  const x = extractFunction(m, { kind: 'index', index: fn.index }, opts);
  const ledger = opts.ledger;
  out.push(
    `;; function ${fn.index} ${signature(fn)} body@0x${fn.bodyOffset.toString(16)} size ${fn.size} instrs ${x.instrs.length}`,
  );
  if (x.normalized) {
    out.push(`;; fingerprint ${x.normalized.fingerprint} shape ${x.normalized.shape} (depth ${depth})`);
  }
  const exp = m.exports
    .filter((e) => e.kind === 'func' && e.index === fn.index)
    .map((e) => (opts.exports?.[e.name] === undefined ? e.name : `${e.name} (${opts.exports[e.name]})`));
  if (exp.length > 0) {
    out.push(`;; exported as ${exp.join(', ')}`);
  }
  const slots = tableSlotsOf(m, fn.index);
  if (slots.length > 0) {
    out.push(`;; table slots ${slots.join(', ')}`);
  }
  if (ledger !== null) {
    out.push(`;; ledger ${ledger.dir} (${ledger.loaded.map((f) => basename(f)).join(', ')})`);
    const row = ledger.functions.get(fn.index);
    const attr = ledger.attribution.get(fn.index);
    const owner = row?.owner ?? ledger.owners.get(fn.index) ?? '-';
    out.push(
      `;; owner ${owner}  component ${attr?.component ?? '-'}${attr && attr.evidence !== '-' ? ` (${attr.evidence}: ${attr.detail})` : ''}${row ? `  uses ${row.uses}` : ''}`,
    );
    const best = bestText(ledger, fn.index);
    if (best !== null) {
      out.push(`;; best pair ${best}`);
    }
    const callers = ledger.callers.get(fn.index);
    if (callers !== undefined) {
      out.push(`;; callers (${callers.length}): ${callers.slice(0, 20).join(' ')}${callers.length > 20 ? ' …' : ''}`);
    }
    for (const v of vtableNotes(ledger, fn.index)) {
      out.push(`;; vtable ${v}`);
    }
    const strs = referencedStrings(x.instrs, ledger.strings);
    if (strs.length > 0) {
      out.push('', ';; --- strings referenced (body offset, address, text)');
      for (const r of strs) {
        out.push(
          `${r.offset.toString(16).padStart(6, '0')}  ${r.addr}${r.into === 0 ? '' : ` (+${r.into} into ${r.s.addr})`}  ${JSON.stringify(r.s.text)}`,
        );
      }
    }
    const callees = calleeNotes(m, x.instrs, ledger, opts.importName);
    if (callees.length > 0) {
      out.push('', ';; --- callees (index, call sites, owner, component, vtable | best pair)');
      for (const c of callees) {
        out.push(
          `${String(c.index).padStart(6)}  ×${String(c.sites).padEnd(4)} ${c.owner.padEnd(28)} ${c.component.padEnd(10)} ${c.vtable ?? ''}${c.best === null ? '' : ` | ${c.best}`}`,
        );
      }
    }
  }
  if (!opts.noDisasm) {
    out.push('', ';; --- disassembly (offset  raw) | normalised');
    x.instrs.forEach((i, k) => {
      out.push(`${i.offset.toString(16).padStart(6, '0')}  ${instrText(i, m).padEnd(48)} | ${x.rows[k]}`);
    });
  }
  out.push('', ';; --- folded readout (approximate)');
  const funcName = (idx: number): string => {
    const imp = importedFunc(m, idx);
    if (imp !== null) {
      return opts.importName(imp.module, imp.name, idx);
    }
    return m.funcNames.get(idx) ?? `f${idx}`;
  };
  out.push(foldFunction(m, fn, x.instrs, { funcName }));
  return out.join('\n');
}

export interface BundleRow {
  index: number;
  size: number;
  uses: number | null;
  owner: string;
  component: string;
  vtable: string;
  best: string;
}

export function bundleRow(fn: Func, ledger: Ledger | null): BundleRow {
  const row = ledger?.functions.get(fn.index);
  const vt = ledger?.vtables.get(fn.index);
  return {
    index: fn.index,
    size: fn.size,
    uses: row?.uses ?? null,
    owner: row?.owner ?? ledger?.owners.get(fn.index) ?? '-',
    component: ledger?.attribution.get(fn.index)?.component ?? row?.component ?? '-',
    vtable: vt === undefined ? '' : vt.map((v) => `${v.class} slot ${v.slot}`).join('; '),
    best: bestText(ledger, fn.index) ?? 'none',
  };
}

const cell = (s: string | number | null): string => String(s ?? '-').replace(/\|/g, '\\|');

/** One Markdown file: a header table over every function, then each readout in a code block. */
export function renderBundle(name: string, m: Module, fns: readonly Func[], opts: ReportOptions): string {
  const norm = opts.norm ?? normalizeModule(m, opts);
  const rows = fns.map((fn) => bundleRow(fn, opts.ledger));
  const totalBytes = rows.reduce((n, r) => n + r.size, 0);
  const out: string[] = [
    `# bundle ${name}`,
    '',
    `${fns.length} functions, ${totalBytes} B of body. Each section is what \`fn\` prints; the table is the round's work list.`,
    '',
    '| index | size | uses | owner | component | vtable class / slot | best pair so far |',
    '| ---: | ---: | ---: | --- | --- | --- | --- |',
    ...rows.map(
      (r) =>
        `| [${r.index}](#fn-${r.index}) | ${r.size} | ${cell(r.uses)} | ${cell(r.owner)} | ${cell(r.component)} | ${cell(r.vtable)} | ${cell(r.best)} |`,
    ),
    '',
  ];
  for (const fn of fns) {
    const row = rows.find((r) => r.index === fn.index)!;
    out.push(
      `## fn ${fn.index}`,
      '',
      `<a id="fn-${fn.index}"></a>${row.owner}/${row.component}, ${fn.size} B${row.vtable === '' ? '' : `, ${row.vtable}`} — best: ${row.best}`,
      '',
      '```',
      renderFunctionReport(m, fn, { ...opts, norm }),
      '```',
      '',
    );
  }
  return out.join('\n');
}
