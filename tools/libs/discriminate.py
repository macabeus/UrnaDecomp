#!/usr/bin/env python3 -I
"""Which target functions does each probe tag match, and which functions tell the tags apart.

  python3 -I tools/libs/discriminate.py <label> <tag>=<census.json>:<probe.wasm.symbols> ...

Each census JSON comes from tools/libs/census_subset.ts (one probe build against the target's <label> slice).
Prints per-tag totals, then the target functions matched by SOME tags but not all (the discriminators), each with
the probe-side name of the matched function (from the tag's symbol map) so the source difference can be read.
"""
import json
import sys

label = sys.argv[1]
tags = {}
syms = {}
for arg in sys.argv[2:]:
    tag, _, rest = arg.partition('=')
    cj, _, sym = rest.partition(':')
    tags[tag] = json.load(open(cj))
    syms[tag] = {}
    if sym:
        for line in open(sym):
            k, _, v = line.rstrip('\n').partition(':')
            if k.isdigit():
                syms[tag][int(k)] = v

names = list(tags)
matched = {t: set(tags[t]['fingerprintIdx']) | set(tags[t]['shapeIdx']) for t in names}
fp = {t: set(tags[t]['fingerprintIdx']) for t in names}
print(f'label {label}:')
for t in names:
    c = tags[t]
    print(f"  {t:>22}: matched {c['matched']:4d} (fingerprint {c['byFingerprint']:4d}, shape {c['byShape']:3d}, byteExact {c['byteExact']:3d}), bytes {c['bytesMatched']}/{c['bytesLabelled']}")
every = set.intersection(*matched.values())
some = set.union(*matched.values())
print(f'  matched by every tag: {len(every)}; by some tag: {len(some)}; discriminators: {len(some - every)}')
print('  pairwise (fingerprint pairs): A-only / B-only')
for i, a in enumerate(names):
    for b in names[i + 1:]:
        print(f'    {a} vs {b}: {len(fp[a] - fp[b])} / {len(fp[b] - fp[a])}   (any match: {len(matched[a] - matched[b])} / {len(matched[b] - matched[a])})')
print('  discriminating target functions (idx: tags that match it [kind] -> probe name):')
for idx in sorted(some - every):
    parts = []
    name = None
    for t in names:
        if idx in matched[t]:
            kind = 'fp' if idx in fp[t] else 'shape'
            b = tags[t]['pairOf'][str(idx)]
            parts.append(f'{t}[{kind}]')
            name = name or syms[t].get(b, f'probe#{b}')
    print(f'    {idx}: {" ".join(parts)} -> {name}')
