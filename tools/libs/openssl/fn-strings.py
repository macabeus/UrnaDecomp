"""For a probe build with a symbol map: which string literals each named function references (i32.const into the
data section), and in particular whether a function references its OWN name (OPENSSL_FUNC in ERR_raise) — the
evidence that lets the target's string table say whether that function is linked in the target.
usage: fn-strings.py <probe-map.wasm> <probe-map.objdump.txt> <probe-map.js.symbols> <out.tsv>
out.tsv: index \t name \t ownname(1/0) \t strings...
"""
import sys, re, struct
wasm, dump, syms, out = sys.argv[1:5]
data = open(wasm, 'rb').read()
def leb(b, i):
    r = 0; s = 0
    while True:
        c = b[i]; i += 1; r |= (c & 0x7f) << s; s += 7
        if not c & 0x80: return r, i
def sleb(b, i):
    r = 0; s = 0
    while True:
        c = b[i]; i += 1; r |= (c & 0x7f) << s; s += 7
        if not c & 0x80:
            if c & 0x40: r -= 1 << s
            return r, i
i = 8; segs = []
while i < len(data):
    sid = data[i]; i += 1; size, i = leb(data, i); end = i + size
    if sid == 11:
        n, j = leb(data, i)
        for _ in range(n):
            flags, j = leb(data, j)
            if flags == 0:
                assert data[j] == 0x41; off, j = sleb(data, j + 1); assert data[j] == 0x0b; j += 1
            elif flags == 1: off = None
            else: raise SystemExit('unsupported data segment flags')
            ln, j = leb(data, j); segs.append((off, data[j:j + ln])); j += ln
    i = end
mem = bytearray(max(o + len(b) for o, b in segs) + 1)
for o, b in segs: mem[o:o + len(b)] = b
def cstr(a):
    if a < 1024 or a >= len(mem): return None
    e = mem.find(b'\0', a)
    if e < 0 or e - a > 200 or e == a: return None
    s = mem[a:e]
    if any(c < 32 or c > 126 for c in s): return None
    return s.decode()
names = {}
for l in open(syms):
    l = l.strip()
    if ':' in l: k, v = l.split(':', 1); names[int(k)] = v
hdr = re.compile(r'^[0-9a-f]+ func\[(\d+)\]'); cur = None; refs = {}
for line in open(dump, errors='replace'):
    m = hdr.match(line)
    if m: cur = int(m.group(1)); refs[cur] = set(); continue
    if cur is None or '|' not in line: continue
    ins = line.split('|', 1)[1].strip()
    m = re.match(r'^i32\.const (-?\d+)$', ins)
    if m:
        s = cstr(int(m.group(1)))
        if s: refs[cur].add(s)
with open(out, 'w') as f:
    own = 0
    for k in sorted(refs):
        n = names.get(k, f'f{k}'); o = int(n in refs[k]); own += o
        f.write(f"{k}\t{n}\t{o}\t" + '\t'.join(sorted(refs[k])) + '\n')
print(f'{len(refs)} functions, {own} reference their own name')
