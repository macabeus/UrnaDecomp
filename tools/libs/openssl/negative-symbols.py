"""Functions that are provably NOT in the target, to be excluded from roots/exports:
 1. every function of a libcrypto member that carries its own __FILE__ string (it has an ERR_raise) but whose path is
    absent from tools/libs/ossl_files.txt — a linked member would have left that string in the target's data;
 2. every function that references its own name (OPENSSL_FUNC) in the probe while the target has no such string.
usage: negative-symbols.py <members.tsv> <probe-map.strings.tsv> <out.txt>
"""
import sys
members, fnstrings, out = sys.argv[1:4]
urna = __file__.rsplit('/tools/libs/', 1)[0]
linked = {l.strip() for l in open(f'{urna}/tools/libs/ossl_files.txt') if l.strip()}
tstr = set()
for l in open(f'{urna}/ledger/strings.tsv'):
    p = l.rstrip('\n').split('\t', 3)
    if len(p) == 4 and p[0].isdigit(): tstr.add(p[3])
# wasm-ld tail-merges string literals: a name that is the suffix of a longer target string may be present as that
# suffix (its pointer lands inside the longer string), so only a name that is no suffix of anything is absent for sure
tails = {s[i:] for s in tstr for i in range(len(s))}
tstr = tails
neg = set(); negmembers = 0
for l in open(members):
    src, obj, ev, syms = (l.rstrip('\n').split('\t') + [''])[:4]
    if ev == 'file' and src not in linked:
        negmembers += 1; neg |= set(syms.split())
own = 0
for l in open(fnstrings):
    p = l.rstrip('\n').split('\t')
    if p[2] == '1' and p[1] not in tstr: neg.add(p[1]); own += 1
open(out, 'w').write('\n'.join(sorted(neg)) + '\n')
print(f'{negmembers} members with an ERR file string the target lacks, {own} own-name phantoms -> {len(neg)} excluded symbols')
