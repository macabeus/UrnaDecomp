"""Root set from the probe's own structure + census evidence.
binaryen inlines-and-removes a function with ONE call site that is not exported/in the table, and drops one with none.
So for each probe function F (export-mode probe, everything alive):
  internal refs = call sites inside the probe (main excluded) + table slots
  - refs <= 1 and F paired in the census (any method)  -> the target has F as a function -> the app calls it -> ROOT
  - refs == 0, unpaired                                 -> unknown (dead in the target, or a mismatched body): kept EXPORTED,
                                                           conservative, revisited when more pairs exist
  - everything else: liveness/inlining is decided inside libcrypto, no root needed
usage: derive-roots.py <census.json> <probe-dir> <out-prefix> [--prev roots.txt ...] [--symbols libcrypto-symbols.txt]
  --symbols  only these names (libcrypto's defined functions) can be roots/exports; libc/libc++ are never roots
  --exclude  symbols proven absent from the target (negative-symbols.py): never roots/exports
  writes <out-prefix>.roots.txt and <out-prefix>.exports.txt (one symbol per line).
"""
import json, sys, re, collections
args = sys.argv[1:]
census, pdir, outp = args[0], args[1], args[2]
prev = []
if '--prev' in args:
    i = args.index('--prev') + 1
    while i < len(args) and not args[i].startswith('--'): prev.append(args[i]); i += 1
names = {}
for l in open(f'{pdir}/probe.names.tsv'):
    i, n, _ = l.rstrip('\n').split('\t'); names[int(i)] = n
refs = collections.Counter(); cur = None
hdr = re.compile(r'^[0-9a-f]+ func\[(\d+)\]')
main_idx = {i for i, n in names.items() if n == 'main'}
for line in open(f'{pdir}/probe.objdump.txt', errors='replace'):
    m = hdr.match(line)
    if m: cur = int(m.group(1)); continue
    if cur is None or cur in main_idx or '|' not in line: continue
    m = re.match(r'^call (\d+)', line.split('|', 1)[1].strip())
    if m: refs[int(m.group(1))] += 1
intable = set()
for line in open(f'{pdir}/probe.objdump-x.txt', errors='replace'):
    m = re.match(r'\s*- elem\[\d+\] = ref\.func:(\d+)', line) or re.match(r'\s*- elem\[\d+\] = func\[(\d+)\]', line)
    if m: intable.add(int(m.group(1)))
paired = {p['b'] for p in json.load(open(census))['pairs']}
exclude = set()
if '--exclude' in args: exclude = {l.strip() for l in open(args[args.index('--exclude') + 1]) if l.strip()}
symbols = None
if '--symbols' in args: symbols = {l.strip() for l in open(args[args.index('--symbols') + 1]) if l.strip()}
nonlib = re.compile(r'^(__|_emscripten|emscripten_|dlmalloc|dlfree|sbrk|std::|main$|_start|setThrew|_Z)')
roots = set(); exports = set()
for p in prev: roots |= {l.strip() for l in open(p) if l.strip()}
roots -= exclude
stat = collections.Counter()
for i, n in names.items():
    if n == '?' or '|' in n or nonlib.match(n): continue
    if symbols is not None and n not in symbols: continue
    if n in exclude: stat['excluded'] += 1; continue
    r = refs[i] + (1 if i in intable else 0)
    if r <= 1 and i in paired: roots.add(n); stat['root'] += 1
    elif r == 0: exports.add(n); stat['export-unknown'] += 1
    else: stat['internal'] += 1
exports -= roots
open(outp + '.roots.txt', 'w').write('\n'.join(sorted(roots)) + '\n')
open(outp + '.exports.txt', 'w').write('\n'.join(sorted(exports)) + '\n')
print(dict(stat), f'-> {len(roots)} roots, {len(exports)} extra exports')
