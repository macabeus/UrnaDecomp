"""The recovered link line: every anchored object with its ordinal interval (from the solver's chunk bases), in base
order, labelled with its component; app stretches (ledger/tu-clusters.tsv) are interleaved by their ordinal window.
usage: python3 -I tools/linkorder/linkline.py blocks.tsv ordinals.tsv [tu-clusters.tsv] > linkline.md"""
import sys, collections, re
blocksp, ordp = sys.argv[1:3]; tup = sys.argv[3] if len(sys.argv) > 3 else None
def component(obj):
    if obj.startswith('sqlite/'): return 'sqlite'
    if obj.startswith('7zip/'): return '7zip'
    if obj.startswith('rhvoice/'): return 'rhvoice'
    if obj.startswith('openssl/') or obj.startswith('libcrypto:'): return 'libcrypto'
    if obj.startswith('libc++abi:'): return 'libc++abi'
    if obj.startswith('libc++:'): return 'libc++'
    if obj.startswith('libc:') or obj.startswith('libdlmalloc:') or obj.startswith('libcompiler_rt:'): return 'libc/rt'
    if obj.startswith('app/ecourna/'): return 'ecourna (landed unit)'
    if obj == '<binaryen>': return 'merge-similar-functions'
    return 'app'
objs = collections.defaultdict(lambda: dict(first=None, last=None, members=0, chunks=0, feas=0))
for l in open(blocksp):
    p = l.rstrip('\n').split('\t')
    if not p[0].isdigit(): continue
    o = objs[p[2]]; span = int(p[6]); blo, bhi, feas = int(p[7]), int(p[8]), int(p[9])
    if blo < 0: continue
    k = int(p[1].split('#')[-1]) if '#' in p[1] else 0
    if o['first'] is None or k < o['first'][0]: o['first'] = (k, blo, bhi, feas)
    if o['last'] is None or k > o['last'][0]: o['last'] = (k, blo + span - 1, bhi + span - 1, feas)
    o['members'] += int(p[5]); o['chunks'] += 1; o['feas'] = max(o['feas'], feas)
rows = []
for name, o in objs.items():
    if o['first'] is None: continue
    mid = (o['first'][1] + o['last'][2]) // 2; width = o['last'][2] - o['first'][1] + 1
    rows.append((mid, width, name, o))
rows.sort()
print('Objects by the midpoint of their ordinal interval (first chunk base window .. last chunk end window, as HULLS: a '
      'small feasible set spread over two digit zones, e.g. {826, 9949}, shows as 826–9949); `tight` = the first and the last '
      'chunk each have at most 20 feasible bases.')
print()
print('| midpoint | first chunk base (feasible) | last chunk end (feasible) | object | component | anchored members | chunks | tight |')
print('|---|---|---|---|---|---|---|---|')
for mid, width, name, o in rows:
    f = o['first']; la = o['last']; tight = f[3] <= 20 and la[3] <= 20
    print(f'| {mid} | {f[1]}–{f[2]} ({f[3]}) | {la[1]}–{la[2]} ({la[3]}) | `{name}` | {component(name)} | {o["members"]} | {o["chunks"]} | {"yes" if tight else ""} |')
print()
tight_rows = [(mid, width, name, o) for mid, width, name, o in rows if o['first'][3] <= 20 and o['last'][3] <= 20]
runs = []
for mid, width, name, o in tight_rows:
    c = component(name); lo = o['first'][1]; hi = o['last'][2]
    if runs and runs[-1][0] == c: runs[-1][2] = max(runs[-1][2], hi); runs[-1][3] += 1; runs[-1][4] += o['members']
    else: runs.append([c, lo, hi, 1, o['members']])
print(f'Component runs over the {len(tight_rows)} tightly placed objects (consecutive objects of one component):')
print()
print('| from | to | component | objects | anchored members |'); print('|---|---|---|---|---|')
for c, lo, hi, n, m in runs: print(f'| {lo} | {hi} | {c} | {n} | {m} |')
