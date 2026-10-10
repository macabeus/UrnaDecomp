# Recover the ASN.1 schema of the generated Modulo* types from the target's DATA: every generated class has a
# static theInfo record (asnparser output); SEQUENCE records end with the type name (TSE's 13th word).
import json, struct, sys, csv, collections
import os
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
SP = ROOT + '/build/out/asn1-schema'
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from wasmmod import Module
U = ROOT
mem = open(SP + '/mem.bin', 'rb').read()
mod = Module(U + '/target/vota_web_wasm.wasm')
table = mod.table
def u32(a): return struct.unpack_from('<I', mem, a)[0]
def s32(a): return struct.unpack_from('<i', mem, a)[0]
def cstr(a):
    e = mem.index(b'\0', a); return mem[a:e].decode('latin-1')
DATA_END = 1811674
def isptr(v): return 1024 <= v < DATA_END
classes = json.load(open(U + '/ledger/classes.json'))['classes']
vtables = json.load(open(U + '/tools/libs/vtables.json'))
vt_by_addr = {v['vtable']: v['dem'] for v in vtables}   # 'vtable' = address stored in the vptr
# consts per function
consts = collections.defaultdict(set)
for line in open(SP + '/consts.tsv'):
    f, c = line.split('\t'); consts[int(f)].add(int(c))
funcs_ref = collections.defaultdict(set)
for f, cs in consts.items():
    for c in cs: funcs_ref[c].add(f)
# strings: address of every string text
strs = {}
for r in csv.DictReader(open(U + '/ledger/strings.tsv'), delimiter='\t'):
    strs.setdefault(r['text'], []).append(int(r['addr']))

def create_class(fidx):
    """the class a create thunk/body instantiates: the vtable constant in it"""
    for c in consts.get(fidx, ()):
        if c in vt_by_addr: return vt_by_addr[c]
    return None

modulo = [c for c in classes if c['name'].startswith('Modulo')]
short = {}
for c in modulo:
    base = c['bases'][0]['name'] if c.get('bases') else ''
    short[c['name']] = base
seq_names = [n for n, b in short.items() if b == 'ASN1::SEQUENCE']

# 1. SEQUENCE theInfo records via the typeName word
records = {}   # addr -> dict
name_of = {}   # theInfo addr -> class name
for full in seq_names:
    sn = full.split('::')[-1]
    cands = []
    needle = sn.encode('latin-1') + b'\0'
    starts = []
    i = mem.find(needle, 1024)
    while i != -1 and i < DATA_END:
        starts.append(i); i = mem.find(needle, i + 1)
    for sa in starts:
        p = struct.pack('<I', sa)
        for h in range(1024, DATA_END - 3, 4):
            if mem[h:h+4] == p:
                base = h - 48
                if base < 1024: continue
                w = [u32(base + 4*k) for k in range(13)]
                if w[0] < len(table) and w[2] == 0 and w[3] in (0, 1) and w[6] <= 64 and (w[4] == 0 or isptr(w[4])) and (w[11] == 0 or isptr(w[11])):
                    cands.append(base)
    if len(cands) != 1:
        print('AMBIGUOUS/MISSING', full, cands, file=sys.stderr)
    for base in cands[:1]:
        name_of[base] = full

def parse_record(base, kind_hint=None):
    w0 = u32(base); fidx = table[w0] if w0 < len(table) else None
    cls = create_class(fidx) if fidx is not None else None
    tag = u32(base + 4)
    rec = {'addr': base, 'create_fn': fidx, 'create_class': cls, 'tag': tag, 'parent': u32(base + 8)}
    return rec

SEQ_INFO_WORDS = 13
out_types = {}
queue = list(name_of.items())
seen = set()
def classify(T):
    """kind of the theInfo record at T"""
    if T in name_of: return 'SEQUENCE', name_of[T]
    if T == 0: return 'REMOVED', None
    w0 = u32(T)
    if w0 not in table: return 'BAD', hex(w0)
    fidx = table[w0]
    cls = create_class(fidx)
    return 'OTHER', cls

while queue:
    base, full = queue.pop()
    if base in seen: continue
    seen.add(base)
    w = [u32(base + 4*k) for k in range(13)]
    n = w[6] + w[7]
    fieldInfos = [u32(w[4] + 4*i) for i in range(n)] if w[4] else []
    ids = [s32(w[5] + 4*i) for i in range(n)] if w[5] else []
    tags = None
    if w[10] == 0: tags = 'automatic'
    elif w[10] == 1149108: tags = 'default'
    else: tags = [u32(w[10] + 4*i) for i in range(n)]
    names = [cstr(u32(w[11] + 4*i)) for i in range(n)] if w[11] else []
    fields = []
    for i in range(n):
        T = fieldInfos[i]
        k, cls = classify(T)
        fields.append({'name': names[i] if i < len(names) else None, 'info': T, 'kind': k, 'class': cls, 'optional_id': ids[i] if i < len(ids) else None, 'tag': tags[i] if isinstance(tags, list) else None})
    out_types[full] = {'kind': 'SEQUENCE', 'addr': base, 'tag': w[1], 'extendable': w[3], 'numFields': w[6], 'knownExtensions': w[7], 'numOptional': w[8], 'nonOptionalExtensions': w[9], 'tags': tags, 'typeName': cstr(w[12]), 'fields': fields}

json.dump({'types': out_types}, open(SP + '/schema-seq.json', 'w'), indent=1, ensure_ascii=False)
print(len(name_of), 'SEQUENCE theInfo records found of', len(seq_names))
# field type census
cnt = collections.Counter()
for t in out_types.values():
    for f in t['fields']:
        cnt[(f['kind'], f['class'])] += 1
for k, v in cnt.most_common(): print(v, k)
