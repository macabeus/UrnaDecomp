// Census of ONE library's slice of the target against a probe module built from that library's
// sources: which of the target functions labelled <label> (tools/libs/cls2.json) have a
// fingerprint / shape / byte-exact twin in the probe.
//
//   pnpm exec tsx ../tools/libs/census_subset.ts \
//        <target.wasm> <probe.wasm> <cls2.json> <label> [--depth N] [--out census.json] [--tag NAME]
//
// Run from asmlift-wasm/ (the imports below point at its sources).
// Pairing is the toolkit's census (fingerprint first, then shape, each paired by count); this script
// only slices the result by the target label and adds the byte-exact check, so its numbers are
// comparable with `cli.ts census`. byteExact compares raw body bytes: it can only be true for a
// function whose body holds no call index and no absolute address that moved.
import { readFileSync, writeFileSync } from 'node:fs';

import { census } from '../../asmlift-wasm/src/census';
import { normalizeModule } from '../../asmlift-wasm/src/normalize';
import { type Module, funcAt, parseModule } from '../../asmlift-wasm/src/parse';

function flag(args: string[], name: string): string | undefined {
  const i = args.indexOf(name);
  if (i < 0) return undefined;
  const v = args[i + 1];
  args.splice(i, 2);
  return v;
}

function sameBytes(a: Uint8Array, b: Uint8Array): boolean {
  if (a.length !== b.length) return false;
  for (let i = 0; i < a.length; i++) if (a[i] !== b[i]) return false;
  return true;
}

const args = process.argv.slice(2);
const depth = Number(flag(args, '--depth') ?? 2);
const out = flag(args, '--out');
const tag = flag(args, '--tag') ?? '';
const [targetPath, probePath, labelsPath, label] = args;
if (label === undefined) {
  process.stderr.write('usage: census_subset.ts <target.wasm> <probe.wasm> <cls2.json> <label> [--depth N] [--out f] [--tag t]\n');
  process.exit(2);
}

const target: Module = parseModule(new Uint8Array(readFileSync(targetPath)));
const probe: Module = parseModule(new Uint8Array(readFileSync(probePath)));
const labels: Record<string, string> = JSON.parse(readFileSync(labelsPath, 'utf8'));
const inLabel = new Set<number>();
for (const [k, v] of Object.entries(labels)) if (v === label) inLabel.add(Number(k));

const c = census(target, normalizeModule(target, { depth }), probe, normalizeModule(probe, { depth }));

const byFingerprint: number[] = [];
const byShape: number[] = [];
const byteExact: number[] = [];
const pairOf: Record<number, number> = {};
const outsideLabel: Record<string, number> = {};
let bytesLabelled = 0;
let bytesMatched = 0;
for (const i of inLabel) bytesLabelled += funcAt(target, i)?.size ?? 0;
for (const p of c.pairs) {
  if (!inLabel.has(p.a)) {
    const l = labels[String(p.a)] ?? 'unknown';
    outsideLabel[l] = (outsideLabel[l] ?? 0) + 1;
    continue;
  }
  pairOf[p.a] = p.b;
  const fa = funcAt(target, p.a)!;
  const fb = funcAt(probe, p.b)!;
  bytesMatched += fa.size;
  if (p.by === 'fingerprint') byFingerprint.push(p.a);
  else byShape.push(p.a);
  if (sameBytes(fa.bodyBytes, fb.bodyBytes)) byteExact.push(p.a);
}
const unmatched = [...inLabel].filter((i) => pairOf[i] === undefined).sort((x, y) => x - y);

const result = {
  tag,
  target: targetPath,
  probe: probePath,
  label,
  depth,
  probeFunctions: probe.funcs.length,
  labelled: inLabel.size,
  byFingerprint: byFingerprint.length,
  byShape: byShape.length,
  matched: byFingerprint.length + byShape.length,
  byteExact: byteExact.length,
  bytesLabelled,
  bytesMatched,
  probeMatchedOutsideLabel: outsideLabel,
  probeUnmatched: c.unmatchedB.length,
  fingerprintIdx: byFingerprint.sort((x, y) => x - y),
  shapeIdx: byShape.sort((x, y) => x - y),
  byteExactIdx: byteExact.sort((x, y) => x - y),
  unmatchedIdx: unmatched,
  pairOf,
};
process.stdout.write(
  `${tag} ${label}: labelled ${inLabel.size}, matched ${result.matched} (fingerprint ${byFingerprint.length}, shape ${byShape.length}, byteExact ${byteExact.length}), bytes ${bytesMatched}/${bytesLabelled} (${((100 * bytesMatched) / Math.max(1, bytesLabelled)).toFixed(1)}%); probe fns ${probe.funcs.length}, probe matched outside label ${JSON.stringify(outsideLabel)}, probe unmatched ${c.unmatchedB.length}\n`,
);
if (out !== undefined) writeFileSync(out, `${JSON.stringify(result)}\n`);
