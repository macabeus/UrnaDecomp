"""Reverse index from the objdump text: for every function, the data addresses it loads with
i32.const (or folded load/store offset) that land on a string, a vtable (vptr = vtable+8) or a typeinfo object; plus the direct call graph.
usage: python3 -I tools/extract/func_refs.py <objdump.txt> <strings.tsv> <vtables.json> <classes.json> <func-strings.tsv> <callgraph.tsv>
func-strings.tsv columns: func \t addr \t kind \t detail
  kind = str (addr is the start of a string), str+N (addr is N bytes into a string; detail is the WHOLE string), vptr (class), typeinfo (class)
callgraph.tsv columns: caller \t callee (direct `call` only; call_indirect is not resolved)"""
import sys, re, json, bisect
objdump, strings_tsv, vtables_json, classes_json, out_refs, out_calls = sys.argv[1:7]

starts = []; slen = {}; stext = {}
with open(strings_tsv, encoding='utf-8') as f:
    next(f)
    for line in f:
        a, n, enc, t = line.rstrip('\n').split('\t', 3)
        a = int(a); starts.append(a); slen[a] = int(n); stext[a] = t
starts.sort()
vptr = {v['vptr']: (v['class'], v['offset_to_top']) for v in json.load(open(vtables_json))}
tinfo = {c['typeinfo']: c['name'] for c in json.load(open(classes_json))['classes']}
lo = starts[0]; hi = max(a + slen[a] for a in starts) + 1

def lookup(addr):
    if addr in vptr:
        c, ott = vptr[addr]; return 'vptr', f'{c}\t{ott}'
    if addr in tinfo: return 'typeinfo', tinfo[addr]
    if addr in stext: return 'str', stext[addr]
    k = bisect.bisect_right(starts, addr) - 1
    if k >= 0:
        s = starts[k]
        if addr < s + slen[s]: return f'str+{addr - s}', stext[s]
    return None

func_re = re.compile(r'^[0-9a-f]+ func\[(\d+)\](?: <[^>]*>)?:')
const_re = re.compile(r'\|\s+i32\.const (\d+)\s*$')
# binaryen folds `i32.const ADDR; load/store` into `i32.const 0; load offset=ADDR`; wasm-objdump prints `i32.load 2 ADDR`
memarg_re = re.compile(r'\|\s+[if](?:32|64)\.(?:load|store)\S*\s+\d+\s+(\d+)\s*$')
call_re = re.compile(r'\|\s+call (\d+)(?: <[^>]*>)?\s*$')
cur = None; nfunc = 0; nref = 0; ncall = 0
with open(objdump, encoding='utf-8', errors='replace') as f, open(out_refs, 'w', encoding='utf-8') as o, open(out_calls, 'w') as oc:
    o.write('func\taddr\tkind\tdetail\n'); oc.write('caller\tcallee\n')
    seen = set()
    for line in f:
        m = func_re.match(line)
        if m:
            cur = int(m.group(1)); nfunc += 1; seen = set(); continue
        if cur is None: continue
        m = const_re.search(line) or memarg_re.search(line)
        if m:
            v = int(m.group(1))
            if lo <= v < hi and v not in seen:
                r = lookup(v)
                if r:
                    seen.add(v); o.write(f'{cur}\t{v}\t{r[0]}\t{r[1]}\n'); nref += 1
            continue
        m = call_re.search(line)
        if m:
            oc.write(f'{cur}\t{m.group(1)}\n'); ncall += 1
print('functions', nfunc, 'refs', nref, 'calls', ncall)
