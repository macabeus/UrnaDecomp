"""Recover RTTI (typeinfo) objects and vtables from the wasm data segments.
usage: python3 -I tools/extract/rtti.py <wasm> <classes.json> <vtables.json>

Itanium ABI layout (wasm32, 4-byte pointers):
  typeinfo object : [vptr -> (vtable of its __*_type_info class)+8][name ptr -> mangled string]...
  __si_class_type_info : + [base typeinfo ptr]
  __vmi_class_type_info: + [u32 flags][u32 base_count] then base_count x [base typeinfo ptr][i32 offset_flags]
  vtable          : [i32 offset_to_top][typeinfo ptr][funcref table index]... (several sub-vtables per class group)
Function pointers are indices into the funcref table (table[0]); the elem segment maps them to function indices.
Demangling: `c++filt` on `_ZTS<name>` ("typeinfo name for X") → X."""
import sys, os, json, re, struct, subprocess, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wasmmod import Module

mod = Module(sys.argv[1])
lo, img = mod.memory_image()
hi = lo + len(img)
covered = bytearray(len(img))
for a, d in mod.segments: covered[a-lo:a-lo+len(d)] = b'\1' * len(d)

def u32(addr): return struct.unpack_from('<I', img, addr - lo)[0]
def i32(addr): return struct.unpack_from('<i', img, addr - lo)[0]
def in_data(addr): return lo <= addr < hi and covered[addr - lo]

# 1. candidate mangled type names: a NUL-terminated string that c++filt accepts as _ZTS<name>
MANGLED = re.compile(rb'^(N|St|P|K|V|r|S|F|A|M|Dp|Dv|Z|[a-z]$|[0-9])')
names = {}  # addr -> raw
i = 0; n = len(img)
while i < n:
    if covered[i] and 0x21 <= img[i] < 0x7f and (i == 0 or not covered[i-1] or img[i-1] == 0):
        j = i
        while j < n and covered[j] and 0x21 <= img[j] < 0x7f: j += 1
        if j < n and img[j] == 0 and j - i >= 1 and MANGLED.match(img[i:j]):
            names[lo + i] = bytes(img[i:j])
        i = j + 1
    else: i += 1

# 2. typeinfo objects: an aligned word equal to a name address, preceded by a plausible vptr into data
ti = {}  # typeinfo addr -> name addr
name_set = set(names)
for a in range(lo + 4, hi - 3, 4):
    if not covered[a - lo]: continue
    v = u32(a)
    if v in name_set and in_data(u32(a - 4)):
        ti[a - 4] = v
# group by vptr
by_vptr = collections.defaultdict(list)
for t, nm in ti.items(): by_vptr[u32(t)].append(t)
# identify the vptr of each __cxxabiv1 kind: the kind's own typeinfo object T_k; its vtable is [0][T_k]...; vptr = vtable+8
kinds = {}
for t, nm in ti.items():
    s = names[nm]
    m = re.match(rb'^N10__cxxabiv1(\d+)(__\w+_type_info)E$', s)
    if m:
        for a in range(lo + 4, hi - 3, 4):
            if covered[a - lo] and u32(a) == t and i32(a - 4) == 0:
                kinds[a + 4] = m.group(2).decode()
print('typeinfo-kind vptrs:', kinds, file=sys.stderr)

def demangle_all(raws):
    inp = '\n'.join('_ZTS' + r.decode() for r in raws) + '\n'
    out = subprocess.run(['c++filt'], input=inp, capture_output=True, text=True).stdout.split('\n')
    res = {}
    for r, d in zip(raws, out):
        if d.startswith('typeinfo name for '): res[r] = d[len('typeinfo name for '):]
        else: res[r] = None  # not demanglable
    return res
dem = demangle_all(sorted({names[nm] for nm in ti.values()}))

classes = {}
unknown_vptr = collections.Counter()
for t, nm in sorted(ti.items()):
    vptr = u32(t); kind = kinds.get(vptr)
    raw = names[nm]; d = dem.get(raw)
    if kind is None:
        unknown_vptr[vptr] += 1; continue
    if d is None: continue
    ent = {'typeinfo': t, 'mangled': raw.decode(), 'name': d, 'kind': kind[2:].replace('_type_info', '')}
    bases = []
    if kind == '__si_class_type_info':
        b = u32(t + 8); bases.append({'typeinfo': b, 'offset': 0, 'virtual': False, 'public': True})
    elif kind == '__vmi_class_type_info':
        flags = u32(t + 8); cnt = u32(t + 12)
        ent['flags'] = flags
        for k in range(cnt):
            b = u32(t + 16 + 8*k); of = i32(t + 20 + 8*k)
            bases.append({'typeinfo': b, 'offset': of >> 8, 'virtual': bool(of & 1), 'public': bool(of & 2)})
    elif kind == '__pointer_type_info':
        ent['pointee'] = u32(t + 12); ent['flags'] = u32(t + 8)
    ent['bases'] = bases
    classes[t] = ent
# resolve base names
for ent in classes.values():
    for b in ent['bases']:
        b['name'] = classes.get(b['typeinfo'], {}).get('name')
    if 'pointee' in ent: ent['pointee_name'] = classes.get(ent['pointee'], {}).get('name')
print('typeinfo objects', len(ti), 'classified', len(classes), 'unknown vptrs', dict(unknown_vptr), file=sys.stderr)

# 3. vtables: [offset_to_top <= 0][typeinfo ptr in classes] then table indices
ti_set = set(classes)
table = mod.table
def is_entry(v):
    return v in table  # a funcref table index (0 is the null entry; table starts at 1)
vtables = []
a = lo
while a + 8 <= hi:
    if covered[a-lo] and (a & 3) == 0 and u32(a + 4) in ti_set and i32(a) <= 0:
        ott = i32(a); t = u32(a + 4); e = a + 8; entries = []
        while e + 4 <= hi and covered[e-lo]:
            v = u32(e)
            # stop at a new vtable header: [<=0][typeinfo]
            if u32(e + 4) in ti_set and i32(e) <= 0 and e + 4 < hi: break
            if not is_entry(v): break
            entries.append(v); e += 4
        vtables.append({'address': a, 'vptr': a + 8, 'offset_to_top': ott, 'class': classes[t]['name'],
                        'typeinfo': t, 'table_indices': entries, 'functions': [table[v] for v in entries]})
        a = e if entries else a + 4
    else: a += 4
# wasm vtable entries: emscripten puts the class group in one object; mark sub-vtables of a group
print('vtables', len(vtables), 'with entries', sum(1 for v in vtables if v['functions']),
      'classes with a vtable', len({v['typeinfo'] for v in vtables}), file=sys.stderr)

json.dump({'classes': sorted(classes.values(), key=lambda c: c['typeinfo'])}, open(sys.argv[2], 'w'), indent=1, ensure_ascii=False)
json.dump(vtables, open(sys.argv[3], 'w'), indent=1, ensure_ascii=False)
kind_count = collections.Counter(c['kind'] for c in classes.values())
print('kinds', dict(kind_count), file=sys.stderr)
