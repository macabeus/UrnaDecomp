# Full ASN.1 schema recovery of the generated Modulo* types from the target's data (+ replayed BSS initialisers).
import json, struct, sys, collections
import os
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
SP = ROOT + '/build/out/asn1-schema'
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from wasmmod import Module
U = ROOT
mem = open(SP + '/mem-init.bin', 'rb').read()
mod = Module(U + '/target/vota_web_wasm.wasm'); table = mod.table
def u32(a): return struct.unpack_from('<I', mem, a)[0]
def s32(a): return struct.unpack_from('<i', mem, a)[0]
def cstr(a):
    e = mem.index(b'\0', a); return mem[a:e].decode('latin-1')
DATA_END = 1811674; BSS_END = 1940000
classes = json.load(open(U + '/ledger/classes.json'))['classes']
vtables = json.load(open(U + '/tools/libs/vtables.json'))
vt_by_dem = {v['dem']: v['vtable'] for v in vtables}
consts = collections.defaultdict(set)
for line in open(SP + '/consts.tsv'):
    f, c = line.split('\t'); consts[int(f)].add(int(c))
funcs_ref = collections.defaultdict(set)
for f, cs in consts.items():
    for c in cs: funcs_ref[c].add(f)
seq_prev = json.load(open(SP + '/schema-seq.json'))['types']
name_of = {t['addr']: n for n, t in seq_prev.items()}
rtti = {c['name']: (c['bases'][0]['name'] if c.get('bases') else '') for c in classes if c['name'].startswith('Modulo')}

# create table-index -> base kind, by the tag/shape (checked above by hand against the vtables)
CREATE = {}
def kind_of(T):
    """classify the theInfo record at T"""
    w0 = u32(T); fidx = table.get(w0); tag = u32(T + 4)
    if T in name_of or fidx == 8962: return 'SEQUENCE'
    if fidx == 9010: return 'SEQUENCE_OF'
    if fidx == 8941: return 'ENUMERATED'
    if fidx == 8999: return 'CHOICE'
    if fidx == 2654: return 'INTEGER'
    if fidx == 8967: return 'OCTET_STRING'
    if fidx == 8950: return 'BOOLEAN'
    if fidx == 9003: return 'STRING'
    return 'UNKNOWN(fn%s,tag%s)' % (fidx, tag)

types = {}      # addr -> record
order = []
def add(T, via):
    if T in types:
        types[T]['via'].append(via); return
    k = kind_of(T)
    rec = {'addr': T, 'kind': k, 'via': [via], 'tag': u32(T + 4), 'parent': u32(T + 8), 'bss': T >= DATA_END}
    types[T] = rec; order.append(T)
    if k == 'SEQUENCE':
        w = [u32(T + 4*i) for i in range(13)]
        n = w[6] + w[7]
        fieldInfos = [u32(w[4] + 4*i) for i in range(n)] if w[4] else []
        ids = [s32(w[5] + 4*i) for i in range(n)] if w[5] else []
        if w[10] == 0: tags = None
        elif w[10] == 1149108: tags = 'default'
        else: tags = [u32(w[10] + 4*i) for i in range(n)]
        names = [cstr(u32(w[11] + 4*i)) for i in range(n)] if w[11] else []
        rec.update({'name': cstr(w[12]) if w[12] else None, 'extendable': w[3], 'numFields': w[6], 'knownExtensions': w[7],
                    'numOptional': w[8], 'nonOptionalExtensions': w[9], 'tags': tags,
                    'fields': [{'name': names[i], 'info': fieldInfos[i], 'optional_id': ids[i], 'tag': (tags[i] if isinstance(tags, list) else None)} for i in range(n)]})
        for i in range(n):
            if fieldInfos[i]: add(fieldInfos[i], (T, names[i]))
    elif k == 'CHOICE':
        w = [u32(T + 4*i) for i in range(9)]
        n = w[6]
        sel = [u32(w[4] + 4*i) for i in range(n)]
        tags = [u32(w[7] + 4*i) for i in range(n)] if w[7] else None
        names = [cstr(u32(w[8] + 4*i)) for i in range(n)] if w[8] else []
        rec.update({'extendable': w[3], 'numChoices': w[5], 'totalChoices': w[6], 'tags': tags,
                    'choices': [{'name': names[i], 'info': sel[i], 'tag': (tags[i] if tags else None)} for i in range(n)]})
        for i in range(n):
            if sel[i]: add(sel[i], (T, names[i]))
    elif k == 'ENUMERATED':
        w = [u32(T + 4*i) for i in range(6)]
        rec.update({'extendable': w[3], 'maxEnumValue': w[4], 'names': [cstr(u32(w[5] + 4*i)) for i in range(w[4] + 1)]})
    elif k == 'SEQUENCE_OF':
        w = [u32(T + 4*i) for i in range(8)]
        rec.update({'ctype': w[3], 'lower': s32(T + 16), 'upper': w[5], 'element': w[6], 'name': cstr(w[7]) if w[7] else None})
        add(w[6], (T, 'element'))
    elif k in ('INTEGER', 'OCTET_STRING'):
        w = [u32(T + 4*i) for i in range(6)]
        rec.update({'ctype': w[3], 'lower': s32(T + 16), 'upper': w[5]})
    elif k == 'STRING':
        w = [u32(T + 4*i) for i in range(11)]
        base = {27: 'GeneralString', 18: 'NumericString', 19: 'PrintableString', 26: 'VisibleString', 22: 'IA5String', 25: 'GraphicString', 30: 'BMPString'}.get(w[1] & 0x1fffffff)
        rec.update({'base': base, 'ctype': w[3], 'lower': s32(T + 16), 'upper': w[5], 'charset': w[6], 'charsetSize': w[7]})
    elif k == 'BOOLEAN':
        pass

for T in sorted(name_of): add(T, ('root', name_of[T]))

# names: RTTI SEQUENCE names first
for T, n in name_of.items(): types[T]['cxx'] = n
# runtime statics (one record, many users, parent == 0, no constraint) -> ASN1 base type
RUNTIME = {}
for T, r in types.items():
    if 'cxx' in r: continue
    if r['kind'] == 'INTEGER' and r['ctype'] == 0: r['cxx'] = 'ASN1::INTEGER'
    elif r['kind'] == 'INTEGER': r['cxx'] = 'ASN1::Constrained_INTEGER<ASN1::FixedConstraint, %d, %d>' % (r['lower'], r['upper'])
    elif r['kind'] == 'OCTET_STRING' and r['ctype'] == 0: r['cxx'] = 'ASN1::OCTET_STRING'
    elif r['kind'] == 'OCTET_STRING': r['cxx'] = 'ASN1::Constrained_OCTET_STRING<ASN1::SizeConstraint<ASN1::FixedConstraint, %d, %d> >' % (r['lower'], r['upper'])
    elif r['kind'] == 'BOOLEAN': r['cxx'] = 'ASN1::BOOLEAN'
    elif r['kind'] == 'STRING' and r['parent'] == 0 and r['ctype'] == 0: r['cxx'] = 'ASN1::' + str(r['base'])
# vtable co-reference for the RTTI ENUMERATED/CHOICE/STRING/nested classes
cand_classes = {n: b for n, b in rtti.items() if b != 'ASN1::SEQUENCE'}
assoc = {}
for n, b in cand_classes.items():
    V = vt_by_dem.get(n)
    V = V + 4 if V is not None else None
    if V is None: continue
    fs = funcs_ref.get(V, set())
    best = []
    for T, r in types.items():
        if 'cxx' in r: continue
        want = {'ASN1::ENUMERATED': 'ENUMERATED', 'ASN1::CHOICE': 'CHOICE', 'ASN1::GeneralString': 'STRING', 'ASN1::NumericString': 'STRING'}.get(b)
        if want is None and b.startswith('ASN1::SEQUENCE_OF<'): want = 'SEQUENCE_OF'
        if r['kind'] != want: continue
        score = len(fs & funcs_ref.get(T, set()))
        if score: best.append((score, T))
    best.sort(reverse=True)
    assoc[n] = best[:3]
for n, best in assoc.items():
    if best and (len(best) == 1 or best[0][0] > best[1][0]):
        T = best[0][1]
        if 'cxx' not in types[T]: types[T]['cxx'] = n
        else: types[T].setdefault('alias', []).append(n)
# nested value_type names for the rest: unique owner field
for T, r in types.items():
    if 'cxx' in r: continue
    owners = [(o, f) for (o, f) in r['via'] if o != 'root' and f != 'element']
    if owners:
        o, f = owners[0]
        r['cxx'] = types[o].get('cxx', '?') + '::' + f + '::value_type'
        if len(set(owners)) > 1: r['shared_by'] = [(types[o].get('cxx'), f) for o, f in owners]
    elif r['kind'] == 'SEQUENCE' and r.get('name'):
        r['cxx'] = '?::' + r['name']
json.dump({'types': {str(k): v for k, v in types.items()}, 'order': order, 'assoc': {k: v for k, v in assoc.items()}}, open(SP + '/schema.json', 'w'), indent=1, ensure_ascii=False)
print(len(types), 'records;', collections.Counter(r['kind'] for r in types.values()))
unn = [r for r in types.values() if 'cxx' not in r]
print('unnamed:', len(unn))
for r in unn[:20]: print('  ', r['addr'], r['kind'], r.get('name'), r['via'][:2])
amb = {n: b for n, b in assoc.items() if not b or (len(b) > 1 and b[0][0] == b[1][0])}
print('ambiguous RTTI associations:', len(amb))
for n, b in amb.items(): print('  ', n, b)
missing = [n for n in cand_classes if n not in assoc or not assoc[n]]
print('RTTI classes without a record:', missing)
