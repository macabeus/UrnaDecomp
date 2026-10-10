"""Attribute every defined function to a component.
usage: python3 -I tools/extract/attribute.py <func-strings.tsv> <vtables.json> <classes.json> <wasm> <func-attribution.tsv> <functions-by-file.tsv>
Evidence order (first hit wins): file = a __FILE__ string the function loads; vtable = the function sits in a class's
vtable; vptr = the function stores a class's vptr (ctor/dtor); pretty = a __PRETTY_FUNCTION__ string; str = another
string with a recognisable owner (sqlite/openssl/rhvoice/...). Unattributed functions are listed with component '-'."""
import sys, os, re, json, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wasmmod import Module
refs_tsv, vtables_json, classes_json, wasm, out_attr, out_byfile = sys.argv[1:7]

SEVENZIP = re.compile(r'^(N(Archive|Compress|Crypto|Windows|7z|Lzma)|I(Compress|Crypto|Sequential|InStream|OutStream|Archive|Stream|Progress|Unknown|Folder|Set|Get)|C(Filter|Limited|Local|Aligned|Tail|StdIn|StdOut|Out(File|Buffer|Handler)|In(File|Buffer)|Open|Extract|ArchiveOpen|Lzma|Bcj|My|Multi|Dec|Enc|Sequential|Byte|Copy|Coder|Mixer|Hash|Bufferer|Base|Buf|Record|Object|Prop|Un|Crc)|Z7_)')
def component_of_class(name):
    top = name.split('::')[0] if '::' in name else name
    if top == '(anonymous namespace)':
        return 'libc++abi' if '::itanium_demangle::' in name else 'vota-web'
    if top in ('vota', 'comum', 'api', 'ecourna', 'simulador', 'RHVoice', 'boost', 'ASN1'): return top
    if top.startswith('Modulo'): return 'asn1-modules'
    if top in ('std', '__cxxabiv1', '__gnu_cxx'): return 'libc++'
    if SEVENZIP.match(top) or re.match(r'^[CIN][A-Z]', top) or top in ('UString', 'AString'): return '7zip'
    if top in ('utf8',): return 'utf8cpp'
    if top in ('rapidxml',): return 'rapidxml'
    if top in ('directory_iterator', 'recursive_directory_iterator') or not re.match(r'^[A-Za-z_]\w*$', top) or top in ('int', 'char', 'unsigned', 'long', 'short', 'float', 'double', 'bool', 'void'): return 'libc++'
    return 'global:' + top

def component_of_file(path):
    if path.startswith('/home/rubio/tse/uenux2/src/app/vota'): return 'vota'
    if path.startswith('/home/rubio/tse/uenux2/src/app/comum'): return 'comum'
    if path.startswith('/home/rubio/tse/uenux2/src/api/'): return 'api'
    if '/ecourna/' in path: return 'ecourna'
    if path.startswith('/emsdk/'): return 'libc++abi'
    if path.startswith('/home/rubio/tse/uenux2/wasm/'): return 'vota-web'
    if path.startswith(('crypto/', 'providers/', 'ssl/', 'include/')): return 'openssl'
    return 'file:' + path

def component_of_string(s):
    if re.match(r'^(sqlite3?|SQLITE_|PRAGMA |SELECT |CREATE |INSERT |UPDATE |DELETE |ALTER |DROP |BEGIN|COMMIT|ROLLBACK|VACUUM|ATTACH|REINDEX|ANALYZE|EXPLAIN|unix|rtree|fts5|geopoly|json)', s) or 'sqlite' in s.lower(): return 'sqlite'
    if re.match(r'^(OPENSSL|OSSL_|EVP_|PKCS|RSA|DSA|DH|EC|X509|CRYPTO_|BN_|SSL)', s) or 'OpenSSL' in s: return 'openssl'
    if 'RHVoice' in s: return 'RHVoice'
    if re.match(r'^(7z|LZMA|BCJ|xz|Deflate)', s): return '7zip'
    return None

vt = json.load(open(vtables_json)); classes = json.load(open(classes_json))['classes']
func_class = collections.defaultdict(set)
for v in vt:
    for f in v['functions']: func_class[f].add(v['class'])

refs = collections.defaultdict(list)
with open(refs_tsv, encoding='utf-8') as f:
    next(f)
    for line in f:
        fn, addr, kind, detail = line.rstrip('\n').split('\t', 3)
        refs[int(fn)].append((kind, detail))

FILE_RE = re.compile(r'^(/|[a-z]+/)[A-Za-z0-9_./+-]*\.(c|cpp|h|hpp|cc|cxx|inl)$')
PRETTY_RE = re.compile(r'^[^%]*::[^%]*\(')
mod = Module(wasm)
rows = []; byfile = collections.defaultdict(set); census = collections.Counter(); evidence_count = collections.Counter()
for fi in range(mod.n_imported_funcs, mod.n_imported_funcs + len(mod.func_types)):
    r = refs.get(fi, [])
    # exact-start refs only: an i32.const that lands INSIDE a string is almost always a plain integer constant
    files = [d for k, d in r if k == 'str' and FILE_RE.match(d)]
    pretties = [d for k, d in r if k == 'str' and PRETTY_RE.match(d) and '::' in d]
    vptrs = [d.split('\t')[0] for k, d in r if k == 'vptr']
    tis = [d for k, d in r if k == 'typeinfo']
    others = [component_of_string(d) for k, d in r if k == 'str']
    others = [o for o in others if o]
    comp = ev = det = None
    if files:
        comps = collections.Counter(component_of_file(p) for p in files)
        comp, ev, det = comps.most_common(1)[0][0], 'file', ';'.join(sorted(set(files)))
        for p in files:
            for pf in pretties: byfile[p].add(pf)
    elif fi in func_class:
        cls = sorted(func_class[fi]); comp, ev, det = component_of_class(cls[0]), 'vtable', ';'.join(cls)
    elif vptrs:
        comp, ev, det = component_of_class(vptrs[0]), 'vptr', ';'.join(sorted(set(vptrs)))
    elif pretties:
        comp, ev, det = component_of_class(pretties[0].split('(')[0].split()[-1]), 'pretty', pretties[0]
    elif tis:
        comp, ev, det = component_of_class(tis[0]), 'typeinfo', tis[0]
    elif others:
        comp, ev, det = collections.Counter(others).most_common(1)[0][0], 'str', ''
    else:
        comp, ev, det = '-', '-', ''
    rows.append((fi, comp, ev, det)); census[comp] += 1; evidence_count[ev] += 1
# propagate along the call graph: an unattributed function whose attributed direct callers all agree inherits
calls = collections.defaultdict(set)
cg = os.path.join(os.path.dirname(refs_tsv), 'callgraph.tsv')
with open(cg) as f:
    next(f)
    for line in f:
        a, b = line.split(); calls[int(b)].add(int(a))
attr = {fi: comp for fi, comp, ev, det in rows}
changed = True; rounds = 0
while changed and rounds < 10:
    changed = False; rounds += 1
    for i, (fi, comp, ev, det) in enumerate(rows):
        if comp != '-': continue
        cs = {attr[c] for c in calls.get(fi, ()) if attr.get(c, '-') != '-'}
        if len(cs) == 1:
            comp = cs.pop(); rows[i] = (fi, comp, 'callers', ''); attr[fi] = comp; changed = True
census2 = collections.Counter(c for _, c, _, _ in rows)
print('after caller propagation: unattributed', census2['-'])
for c, n in census2.most_common(): print(f'{n:6d}  {c}')
# runs of the dominant component along the function index axis (link order)
print('index-axis runs (bucket=200):')
B = 200
for k in range(0, len(rows), B):
    cnt = collections.Counter(c for _, c, _, _ in rows[k:k+B] if c != '-')
    top = cnt.most_common(3)
    print(f'  {rows[k][0]:6d}-{rows[min(k+B, len(rows))-1][0]:6d}  ' + ', '.join(f'{c}:{n}' for c, n in top))
with open(out_attr, 'w', encoding='utf-8') as o:
    o.write('func\tcomponent\tevidence\tdetail\n')
    for fi, comp, ev, det in rows: o.write(f'{fi}\t{comp}\t{ev}\t{det}\n')
with open(out_byfile, 'w', encoding='utf-8') as o:
    o.write('file\tpretty_function\n')
    for p in sorted(byfile):
        for pf in sorted(byfile[p]): o.write(f'{p}\t{pf}\n')
print('functions', len(rows))
print('evidence', dict(evidence_count))
for c, n in census.most_common(): print(f'{n:6d}  {c}')
