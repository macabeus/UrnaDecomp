"""Prune roots/exports with the target's string table: a probe function that references its own name (OPENSSL_FUNC)
while the target lacks that string is NOT in the target ("phantom"); neither is any root/export from which a phantom is
reachable by calls (a static call chain in the target would have linked it).  Appends them to the negative list.
usage: prune-by-phantoms.py <probe-dir> <roots.txt> <exports.txt> <negative.txt> <out-prefix>
   (probe-dir holds probe-map.strings.tsv from fn-strings.py and probe-map.objdump.txt)
"""
import sys, re, collections
pdir, roots_f, exports_f, neg_f, outp = sys.argv[1:6]
urna = __file__.rsplit('/tools/libs/', 1)[0]
tstr = set()
for l in open(f'{urna}/ledger/strings.tsv'):
    p = l.rstrip('\n').split('\t', 3)
    if len(p) == 4 and p[0].isdigit(): tstr.add(p[3])
# wasm-ld tail-merges string literals: a name that is the suffix of a longer target string may be present as that
# suffix (its pointer lands inside the longer string), so only a name that is no suffix of anything is absent for sure
tails = {s[i:] for s in tstr for i in range(len(s))}
tstr = tails
names = {}; phantom = set()
for l in open(f'{pdir}/probe-map.strings.tsv'):
    p = l.rstrip('\n').split('\t'); names[int(p[0])] = p[1]
    if p[2] == '1' and p[1] not in tstr: phantom.add(int(p[0]))
callers = collections.defaultdict(set); cur = None
hdr = re.compile(r'^[0-9a-f]+ func\[(\d+)\]')
for line in open(f'{pdir}/probe-map.objdump.txt', errors='replace'):
    m = hdr.match(line)
    if m: cur = int(m.group(1)); continue
    if cur is None or '|' not in line: continue
    m = re.match(r'^call (\d+)', line.split('|', 1)[1].strip())
    if m: callers[int(m.group(1))].add(cur)
anc = set(); stack = list(phantom)
while stack:
    x = stack.pop()
    for c in callers.get(x, ()):
        if c not in anc: anc.add(c); stack.append(c)
bad = {names[i] for i in phantom | anc} - {'main'}
roots = {l.strip() for l in open(roots_f) if l.strip()}
exports = {l.strip() for l in open(exports_f) if l.strip()}
neg = {l.strip() for l in open(neg_f) if l.strip()}
r2 = roots - bad; e2 = exports - bad
open(outp + '.roots.txt', 'w').write('\n'.join(sorted(r2)) + '\n')
open(outp + '.exports.txt', 'w').write('\n'.join(sorted(e2)) + '\n')
open(neg_f, 'w').write('\n'.join(sorted(neg | bad)) + '\n')
print(f'{len(phantom)} phantoms, {len(anc)} ancestors; roots {len(roots)}->{len(r2)}, exports {len(exports)}->{len(e2)}; negative list {len(neg)}->{len(neg | bad)}')
