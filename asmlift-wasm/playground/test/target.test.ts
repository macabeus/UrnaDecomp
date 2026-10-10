import { readFileSync } from 'node:fs';
import { describe, expect, it } from 'vitest';

import { liftFunction } from '@asmlift/wasm/lift';
import { liftInputs } from '@asmlift/wasm/lift-inputs';
import { readCallgraph } from '@asmlift/wasm/readout';
import { liftCompileFlags } from '@asmlift/wasm-benchmark/flags';
import type { DatasetRow } from '@asmlift/wasm-benchmark/schema';

import { INPUTS, URNA_ROOT } from '../scripts/data';
import {
  calleesOf,
  defaultLiftFlags,
  directCallers,
  displayName,
  foldText,
  instructions,
  lift,
  searchFunctions,
  watText,
} from '../src/lib/target';
import { pageTarget } from './helpers';

const rows = JSON.parse(
  readFileSync(`${URNA_ROOT}/asmlift-wasm/benchmark/dataset/rows.json`, 'utf8'),
) as DatasetRow[];

describe('openTarget', () => {
  it('names every function as liftInputs names it for the CLI', () => {
    const cli = liftInputs(INPUTS.wasm, {}, () => {});
    expect(new Map(pageTarget().names)).toEqual(cli.names);
  });
});

describe('lift', () => {
  it('writes the source the benchmark compiles for every benchmark row', () => {
    const cli = liftInputs(INPUTS.wasm, {}, () => {});
    const t = pageTarget();
    for (const r of rows) {
      const flags = liftCompileFlags(r.flags);
      const expected = liftFunction(cli.m, r.index, { names: cli.names, ...(cli.mangled ? { mangled: cli.mangled } : {}), flags });
      expect(lift(t, r.index, flags).source, r.id).toBe(expected.source);
    }
  });

  it('writes the component flags the CLI lift uses when no row gives them', () => {
    const t = pageTarget();
    const sqlite = rows.find((r) => r.setup === 'sqlite')!;
    expect(defaultLiftFlags(t, sqlite.index)).toBe('-O3 -fPIC');
    expect(lift(t, sqlite.index, defaultLiftFlags(t, sqlite.index)).source).toContain('// em++ -O3 -fPIC');
  });
});

describe('watText', () => {
  it('writes one line per instruction under the func header, the last end closing the func', () => {
    const t = pageTarget();
    for (const r of rows) {
      const instrs = instructions(t, r.index);
      const wat = watText(t, r.index, instrs);
      const lines = wat.text.split('\n');
      const header = lines.length - instrs.length;
      expect(header === 1 || header === 2, r.id).toBe(true);
      expect(lines[0]).toMatch(new RegExp(`^\\(func \\$${r.index} \\(type \\d+\\)`));
      expect(lines.at(-1)).toBe(')');
      expect(wat.offsets.slice(header)).toEqual(instrs.map((i) => i.offset));
    }
  });

  it('names the callee of every direct call', () => {
    const t = pageTarget();
    const row = rows.find((r) => r.id === 'uenux2/1527')!;
    const wat = watText(t, row.index).text;
    for (const c of calleesOf(t, instructions(t, row.index))) {
      expect(wat).toContain(`call ${c.index}  ;; `);
    }
  });
});

describe('foldText', () => {
  it('folds a body into statements that call callees by name', () => {
    const t = pageTarget();
    const caller = rows.find((r) => !r.features.includes('leaf'))!;
    const callees = calleesOf(t, instructions(t, caller.index)).filter((c) => !c.import);
    const text = foldText(t, caller.index);
    expect(callees.length).toBeGreaterThan(0);
    for (const c of callees) {
      expect(text).toContain(`${c.name}(`);
    }
  });
});

describe('directCallers', () => {
  it('finds the callers ledger/callgraph.tsv lists for every function', () => {
    const t = pageTarget();
    const ledger = readCallgraph(`${URNA_ROOT}/ledger/callgraph.tsv`);
    const ours = directCallers(t);
    let compared = 0;
    for (const [callee, callers] of ledger) {
      expect([...(ours.get(callee) ?? [])].sort((a, b) => a - b), String(callee)).toEqual(
        [...new Set(callers)].sort((a, b) => a - b),
      );
      compared++;
    }
    expect(compared).toBe(ledger.size);
    expect(ours.size).toBe(ledger.size);
  });
});

describe('searchFunctions', () => {
  it('puts the exact index first, then indices starting with the digits', () => {
    const hits = searchFunctions(pageTarget(), '2219');
    expect(hits[0]).toEqual({ index: 2219, name: displayName(pageTarget(), 2219) });
    expect(hits.slice(1).every((h) => String(h.index).startsWith('2219'))).toBe(true);
  });

  it('finds a function by part of its name, exact names first', () => {
    const hits = searchFunctions(pageTarget(), 'atoi');
    expect(hits[0].name).toBe('atoi');
    expect(hits.every((h) => h.name.toLowerCase().includes('atoi'))).toBe(true);
  });

  it('does not list imports, which have no body', () => {
    const t = pageTarget();
    expect(searchFunctions(t, '__cxa_throw').every((h) => h.index >= t.m.importedFuncCount)).toBe(true);
    expect(searchFunctions(t, '12')[0].index).toBeGreaterThanOrEqual(t.m.importedFuncCount);
  });
});
