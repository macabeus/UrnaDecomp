// Compare the target's vtable slot functions of one class (tools/libs/vtables.json, RTTI-named) with the same class's
// functions in one or more probe builds (named through their emcc --emit-symbol-map), objdiff-style score per pair.
//
//   pnpm exec tsx ../tools/libs/vtable_compare.ts \
//        <target.wasm> <vtables.json> '<class regex>' <probeA.wasm>=<probeA.js.symbols> [<probeB.wasm>=<symbols> ...]
//
// For every target class matching the regex and every slot function of it, prints the function's size and, per probe,
// the best-scoring (fewest differing rows at depth 0) probe function whose symbol name starts with that class name,
// so a version-specific body shows as 0 in one probe and > 0 in the others. Run from asmlift-wasm/.
import { readFileSync } from 'node:fs';

import { normalizeModule, type NormalizedFn } from '../../asmlift-wasm/src/normalize';
import { type Module, funcAt, parseModule } from '../../asmlift-wasm/src/parse';
import { score } from '../../asmlift-wasm/src/score';

const [targetPath, vtablesPath, classRe, ...probeArgs] = process.argv.slice(2);
if (probeArgs.length === 0) {
  process.stderr.write("usage: vtable_compare.ts <target.wasm> <vtables.json> '<class regex>' <probe.wasm>=<symbols> ...\n");
  process.exit(2);
}
const re = new RegExp(classRe);
const target: Module = parseModule(new Uint8Array(readFileSync(targetPath)));
const tn = normalizeModule(target, { depth: 0 });
const tByIdx = new Map(tn.map((n) => [n.index, n]));
const vtables: { dem: string; funcs: number[] }[] = JSON.parse(readFileSync(vtablesPath, 'utf8'));

interface Probe {
  label: string;
  m: Module;
  byIdx: Map<number, NormalizedFn>;
  names: Map<number, string>;
}
const probes: Probe[] = probeArgs.map((a) => {
  const [wasm, syms] = a.split('=');
  const m = parseModule(new Uint8Array(readFileSync(wasm)));
  const norm = normalizeModule(m, { depth: 0 });
  const names = new Map<number, string>();
  for (const line of readFileSync(syms, 'utf8').split('\n')) {
    const i = line.indexOf(':');
    if (i > 0 && /^\d+$/.test(line.slice(0, i))) names.set(Number(line.slice(0, i)), line.slice(i + 1));
  }
  return { label: wasm.replace(/.*\/build\/out\//, '').replace(/\/probe\.wasm$/, ''), m, byIdx: new Map(norm.map((n) => [n.index, n])), names };
});

function rows(n: NormalizedFn) {
  return { rows: n.rows, instrs: n.instrs };
}

for (const vt of vtables) {
  if (!re.test(vt.dem)) continue;
  const cls = vt.dem.replace(/ >/g, '>');
  process.stdout.write(`\n== ${vt.dem}\n`);
  for (const slot of vt.funcs) {
    const tfn = funcAt(target, slot);
    const t = tByIdx.get(slot);
    if (!tfn || !t) continue;
    let line = `  target ${slot} (${tfn.size} B, ${t.rows.length} rows):`;
    for (const p of probes) {
      let best: { idx: number; s: number; rows: number; name: string } | null = null;
      for (const [idx, name] of p.names) {
        if (!name.replace(/ >/g, '>').startsWith(cls)) continue;
        const pn = p.byIdx.get(idx);
        if (!pn) continue;
        const sc = score(rows(t) as never, rows(pn) as never);
        if (best === null || sc.score < best.s) best = { idx, s: sc.score, rows: sc.rows, name: name.slice(cls.length) };
      }
      line += best ? `  ${p.label}: ${best.s}/${best.rows} ${best.name.slice(0, 50)}` : `  ${p.label}: -`;
    }
    process.stdout.write(`${line}\n`);
  }
}
