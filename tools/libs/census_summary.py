#!/usr/bin/env python3 -I
"""Fold the per-build census JSONs of one library (tools/libs/census_subset.ts output) into a compact census.json.

  python3 -I tools/libs/census_summary.py <out.json> <label> <verdict text> <tag>=<census.json> ... [--notes <file>]

Keeps every count and the matched/unmatched target indices per build, drops the pairOf map (rebuild it with
census_subset.ts when needed), and records the commands that produced the numbers.
"""
import json
import sys

args = sys.argv[1:]
notes = None
if '--notes' in args:
    i = args.index('--notes')
    notes = open(args[i + 1]).read()
    del args[i:i + 2]
out, label, verdict, *pairs = args
builds = {}
for p in pairs:
    tag, _, path = p.partition('=')
    c = json.load(open(path))
    c.pop('pairOf', None)
    builds[tag] = c
summary = {
    'label': label,
    'labelled': next(iter(builds.values()))['labelled'] if builds else 0,
    'verdict': verdict,
    'method': 'tools/libs/<lib>/build.sh <tag> <variant> <flags> (probe module, pinned emsdk 6.0.0, link -Oz -fexceptions), then '
              '(cd asmlift-wasm && pnpm exec tsx ../tools/libs/census_subset.ts) target/vota_web_wasm.wasm <probe.wasm> tools/libs/cls2.json '
              f'{label} --depth 0; discriminators: python3 -I tools/libs/discriminate.py; vtable classes: tools/libs/vtable_compare.ts',
    'builds': {t: {k: v for k, v in c.items() if k not in ('target', 'probe', 'label')} for t, c in builds.items()},
}
if notes is not None:
    summary['notes'] = notes
json.dump(summary, open(out, 'w'), indent=1)
print(f'{out}: {len(builds)} builds, ' + ', '.join(f"{t}={c['matched']}" for t, c in builds.items()))
