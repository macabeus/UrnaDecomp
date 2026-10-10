"""Name the functions of a no-symbol-map probe build from its --emit-symbol-map twin.
usage: name-probe.py <probe-dir>   (expects probe.fns.jsonl, probe-map.fns.jsonl, probe-map.js.symbols; writes probe.names.tsv)
The two links differ only in function ORDER (names survive into wasm-opt's reorder when a map is emitted), so bodies
pair by wasmfn's normalised hash (call targets normalised); a hash shared by several identical bodies gets all their names.
"""
import json, sys, collections
d = sys.argv[1]
def load(p): return [json.loads(l) for l in open(p)]
plain = load(f'{d}/probe.fns.jsonl'); mapped = load(f'{d}/probe-map.fns.jsonl')
names = {}
for l in open(f'{d}/probe-map.js.symbols'):
    l = l.strip()
    if ':' in l:
        i, n = l.split(':', 1); names[int(i)] = n
byhash = collections.defaultdict(list)
for j in mapped: byhash[j['hash']].append(names.get(j['idx'], f'map{j["idx"]}'))
out = open(f'{d}/probe.names.tsv', 'w'); amb = 0; miss = 0
for j in plain:
    ns = byhash.get(j['hash'])
    if not ns: miss += 1; out.write(f"{j['idx']}\t?\t{j['n']}\n"); continue
    if len(ns) > 1: amb += 1
    out.write(f"{j['idx']}\t{'|'.join(sorted(set(ns)))}\t{j['n']}\n")
print(f'{len(plain)} functions named, {amb} ambiguous (identical bodies), {miss} unmatched')
