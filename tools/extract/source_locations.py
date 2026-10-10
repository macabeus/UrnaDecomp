"""Find every std::source_location::__impl record {file, function, line, column} in the data image and the
functions whose code loads its address (the 4th argument of the ecourna exception constructors).
usage: python3 -I tools/extract/source_locations.py <objdump -d text> > ledger/source-locations.tsv"""
import os, sys, re, struct, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import wasmmod
root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
m = wasmmod.Module(root + '/target/vota_web_wasm.wasm')
img = bytearray(2_000_000)
for addr, b in m.segments: img[addr:addr + len(b)] = b
strings = {}
for line in open(root + '/ledger/strings.tsv', encoding='utf-8'):
    p = line.rstrip('\n').split('\t')
    if p[0] == 'addr': continue
    strings[int(p[0])] = p[3]
srcfiles = {a for a, t in strings.items() if t.startswith('/home/rubio') or t.startswith('/emsdk')}
uses = collections.defaultdict(set)
cur = None
fre = re.compile(r'^[0-9a-f]+ func\[(\d+)\]')
cre = re.compile(r'\|\s+i32\.const (-?\d+)$')
for line in open(sys.argv[1]):
    mm = fre.match(line)
    if mm: cur = int(mm.group(1)); continue
    mm = cre.search(line)
    if mm and cur is not None: uses[int(mm.group(1))].add(cur)
print('loc_addr\tfile\tline\tcol\tfunction\tfuncs')
for off in range(1024, len(img) - 16, 4):
    w0 = struct.unpack_from('<I', img, off)[0]
    if w0 in srcfiles:
        w1, w2, w3 = struct.unpack_from('<III', img, off + 4)
        if w1 in strings and 0 < w2 < 100000 and w3 < 2000:
            print(f"{off}\t{strings[w0]}\t{w2}\t{w3}\t{strings[w1]}\t{','.join(str(x) for x in sorted(uses.get(off, ())))}")
