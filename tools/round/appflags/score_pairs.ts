// Batch scorer for the APPFLAGS probes: normalises the target and one probe once, then scores every pair of
// tools/round/appflags/pairs.tsv (target index <-> probe function found by a substring of its symbol-map name).
// usage (from asmlift-wasm/):
//   pnpm --silent exec tsx <this file> <target.wasm> <probe.wasm> <pairs.tsv> [--full <idx>]
import { readFileSync } from 'node:fs';
import { glueNamesFor } from '../../../asmlift-wasm/src/glue-file';
import { importResolver } from '../../../asmlift-wasm/src/imports';
import { normalizeModule, signature } from '../../../asmlift-wasm/src/normalize';
import { parseModule } from '../../../asmlift-wasm/src/parse';
import { disassemble } from '../../../asmlift-wasm/src/disasm';
import { renderSideBySide, score } from '../../../asmlift-wasm/src/score';

const [targetPath, probePath, pairsPath, ...rest] = process.argv.slice(2);
const full = rest[0] === '--full' ? Number(rest[1]) : null;
// depth 0 = callee-blind rows (every call is `call fn:?`): two links never share callee fingerprints, so the
// structural comparison of ONE body must not depend on them
const DEPTH = Number(process.env.APPFLAGS_DEPTH ?? 0);
function load(path: string) {
  const m = parseModule(new Uint8Array(readFileSync(path)));
  const glue = glueNamesFor(path);
  const norm = normalizeModule(m, { depth: DEPTH, importName: importResolver(glue?.imports ?? null) });
  return { m, norm };
}
const T = load(targetPath);
const P = load(probePath);
const symbols = new Map<number, string>();
for (const line of readFileSync(probePath.replace(/\.wasm$/, '.js.symbols'), 'utf8').split('\n')) {
  const i = line.indexOf(':');
  if (i > 0) symbols.set(Number(line.slice(0, i)), line.slice(i + 1));
}
function rowsOf(L: ReturnType<typeof load>, idx: number) {
  const fn = L.m.funcs[idx - L.m.importedFuncCount];
  const n = L.norm[idx - L.m.importedFuncCount];
  return { rows: n.rows, fn, signature: signature(fn) };
}
let total = 0;
let exact = 0;
for (const line of readFileSync(pairsPath, 'utf8').split('\n')) {
  if (!line || line.startsWith('#')) continue;
  const [tf, sub, what] = line.split('\t');
  if (full !== null && Number(tf) !== full) continue;
  // "callee:<substring>" names the first function the matched function calls (binaryen's merged bodies have no name)
  const viaCallee = sub.startsWith('callee:');
  const needle = viaCallee ? sub.slice(7) : sub;
  const hits = [...symbols.entries()].filter(([, name]) => name.includes(needle));
  if (hits.length === 0) {
    console.log(`${tf.padEnd(6)} ${what.padEnd(44)} ABSENT`);
    continue;
  }
  // several copies share a demangled name (D0/D1 destructors, duplicates): prefer the one with the target's signature
  const tsig = signature(T.m.funcs[Number(tf) - T.m.importedFuncCount]);
  const bySig = hits.find(([i]) => signature(P.m.funcs[i - P.m.importedFuncCount]) === tsig);
  let ci = (bySig ?? hits[0])[0];
  if (viaCallee) {
    const fn = P.m.funcs[ci - P.m.importedFuncCount];
    const call = disassemble(fn).find((i) => i.name === "call" && (i.imms[0] as { value: number }).value >= P.m.importedFuncCount);
    if (!call) {
      console.log(`${tf.padEnd(6)} ${what.padEnd(44)} ABSENT (no call in ${symbols.get(ci)})`);
      continue;
    }
    ci = (call.imms[0] as { value: number }).value;
  }
  const s = score(rowsOf(T, Number(tf)), rowsOf(P, ci));
  const b = s.breakdown;
  const struct = s.score - b.operandOnly;
  total++;
  if (struct === 0) exact++;
  const note = hits.length > 1 ? ` (${hits.length} name hits)` : '';
  console.log(
    `${tf.padEnd(6)} ${what.padEnd(44)} cand ${String(ci).padEnd(5)} score ${String(s.score).padStart(4)} rows ${String(s.rows).padStart(4)} operand-only ${String(b.operandOnly).padStart(3)} struct ${String(struct).padStart(4)}${s.byteExact ? ' BYTE-EXACT' : ''}${note}`,
  );
  if (full !== null) console.log(renderSideBySide(s, { onlyDiff: false }));
}
if (full === null) console.log(`structurally identical: ${exact} / ${total}`);
