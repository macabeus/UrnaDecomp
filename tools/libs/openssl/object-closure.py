"""wasm-ld archive semantics on libcrypto's members: an undefined symbol pulls the member that defines it.  Computes the
closure of (the 346 members the target names + the members defining the roots/exports) and reports every "negative"
member in it (one whose ERR __FILE__ string the target lacks, or that defines a phantom), with the seed that pulled it.
usage: object-closure.py <members.tsv> <members-undef.tsv> <negative.txt> <roots.txt> <exports.txt> [--prune out-prefix]
  --prune  writes roots/exports minus every seed whose pull chain reaches a negative member
"""
import sys, collections
members, undef_f, neg_f, roots_f, exports_f = sys.argv[1:6]
urna = __file__.rsplit('/tools/libs/', 1)[0]
linked = {l.strip() for l in open(f'{urna}/tools/libs/ossl_files.txt') if l.strip()}
obj_src = {}; obj_ev = {}; defs = {}; obj_syms = {}
for l in open(members):
    src, obj, ev, syms = (l.rstrip('\n').split('\t') + [''])[:4]
    obj_src[obj] = src; obj_ev[obj] = ev; obj_syms[obj] = set(syms.split())
    for s in syms.split(): defs.setdefault(s, obj)
undef = {}
for l in open(undef_f):
    obj, u = (l.rstrip('\n').split('\t') + [''])[:2]; undef[obj] = set(u.split())
neg_syms = {l.strip() for l in open(neg_f) if l.strip()}
negobj = {o for o in obj_src if (obj_ev[o] == 'file' and obj_src[o] not in linked)}
# a member is also negative when it defines a phantom but is NOT one of the 346 (a linked member may legitimately hold dropped functions)
negobj |= {o for o in obj_src if obj_src[o] not in linked and obj_syms[o] & neg_syms}
roots = {l.strip() for l in open(roots_f) if l.strip()}; exports = {l.strip() for l in open(exports_f) if l.strip()}
member_objs = {o for o in obj_src if obj_src[o] in linked}
def closure(seeds):
    seen = set(); parent = {}; stack = list(seeds)
    for s in seeds: parent[s] = None
    while stack:
        o = stack.pop()
        if o in seen: continue
        seen.add(o)
        for u in undef.get(o, ()):
            d = defs.get(u)
            if d and d not in seen and d not in parent: parent[d] = o; stack.append(d)
    return seen, parent
base, _ = closure(member_objs)
print(f'closure of the {len(member_objs)} named members: {len(base)} objects; negative among them: {sorted(obj_src[o] for o in base & negobj)}')
seed_of = {}
for sym in roots | exports:
    o = defs.get(sym)
    if o and o not in base: seed_of.setdefault(o, set()).add(sym)
full, parent = closure(member_objs | set(seed_of))
bad = full & negobj
print(f'with {len(seed_of)} root/export objects: {len(full)} objects; negative: {len(bad)}')
def chain(o):
    c = []
    while o is not None and o not in member_objs: c.append(obj_src[o]); o = parent.get(o)
    return c
culprits = collections.Counter(); bad_syms = set()
for o in sorted(bad, key=lambda o: obj_src[o]):
    ch = chain(o); root_obj = None
    x = o
    while parent.get(x) is not None: x = parent[x]
    if x in seed_of:
        culprits[x] += 1; bad_syms |= seed_of[x]
    print(f'  {obj_src[o]}  <- ' + ' <- '.join(ch[1:4]) + (f'  [seed {obj_src[x]}: {sorted(seed_of[x])[:3]}]' if x in seed_of else '  [reached from a NAMED member]'))
print(f'{len(culprits)} seed objects pull negatives; {len(bad_syms)} root/export symbols to drop')
if '--prune' in sys.argv:
    outp = sys.argv[sys.argv.index('--prune') + 1]
    open(outp + '.roots.txt', 'w').write('\n'.join(sorted(roots - bad_syms)) + '\n')
    open(outp + '.exports.txt', 'w').write('\n'.join(sorted(exports - bad_syms)) + '\n')
    print(f'roots {len(roots)}->{len(roots - bad_syms)}, exports {len(exports)}->{len(exports - bad_syms)}')
