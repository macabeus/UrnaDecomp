"""Census of one rebuilt probe module against the target, per target function.
usage: census.py <work> <name> <target.wasm> <plain.wasm> <named.symbols> <cls2.json> <out.json>
<work> holds the wasm-objdump -d dumps (target: build/dump/vota.objdump.txt; probe: <name>.plain.objdump.txt,
<name>.named.objdump.txt), the toolkit's `parse --functions` records and its depth-0 census JSON (census.sh writes them).

kind of a match, strongest first:
  byte        the raw body bytes are identical
  exact       rows identical after normalisation: `call N` -> `call F`; call_indirect type index -> its signature;
              i32.const / memarg offset in the static range [1024, __stack_pointer init) on both sides -> A
  table       exact except i32.const rows that are function-table slots on both sides whose functions match
              (recursively, 2 levels)
  fingerprint / shape   the toolkit's depth-0 census pairing (constants differ beyond the rules above)
  none
"""
import sys, json, collections, re, subprocess, hashlib
work, name, tw, pw, symf, clsf, outf = sys.argv[1:8]
HDR = re.compile(r'^[0-9a-f]+ func\[(\d+)\]')
MEM = re.compile(r'^((?:i32|i64|f32|f64)\.(?:load|store)[0-9a-z_]*) (\d+) (\d+)$')
CI = re.compile(r'^call_indirect (\d+) \(type (\d+)\)$')
CONST = re.compile(r'^i32\.const (\d+)$')

def types(wasm):
    out = subprocess.run(['wasm-objdump', '-x', '-j', 'Type', wasm], capture_output=True, text=True).stdout
    return {int(m.group(1)): m.group(2) for m in re.finditer(r'type\[(\d+)\] (.*)$', out, re.M)}
def elem(wasm):
    out = subprocess.run(['wasm-objdump', '-x', '-j', 'Elem', wasm], capture_output=True, text=True).stdout
    return {int(m.group(1)): int(m.group(2)) for m in re.finditer(r'elem\[(\d+)\] = ref\.func:(\d+)', out)}
def data_end(wasm):
    """end of static data + bss = the initial __stack_pointer (global 0): addresses of bss objects lie past the last segment"""
    out = subprocess.run(['wasm-objdump', '-x', '-j', 'Global', wasm], capture_output=True, text=True).stdout
    return int(re.search(r'global\[0\] i32 mutable=1 - init i32=(\d+)', out).group(1))

class Mod:
    def __init__(self, wasm, dump):
        self.types, self.elem, self.dend = types(wasm), elem(wasm), data_end(wasm)
        self.fns = {}          # idx -> rows (raw operands kept, call -> F, type -> signature)
        cur, body = None, []
        for line in open(dump, errors='replace'):
            m = HDR.match(line)
            if m:
                if cur is not None: self.fns[cur] = body
                cur, body = int(m.group(1)), []; continue
            if cur is None or '|' not in line: continue
            ins = line.split('|', 1)[1].strip()
            if not ins: continue
            if ins.startswith('local['):
                body.append(re.sub(r'local\[\d+(\.\.\d+)?\]', 'local', ins)); continue
            if ins.startswith('call '):
                ins = 'call F'
            else:
                m = CI.match(ins)
                if m: ins = f'call_indirect {m.group(1)} {self.types[int(m.group(2))]}'
            body.append(ins)
        if cur is not None: self.fns[cur] = body
        self.norm = {i: self.normalise(b) for i, b in self.fns.items()}
        self.hash = {i: hashlib.sha1('\n'.join(r).encode()).hexdigest()[:16] for i, r in self.norm.items()}
        self.skel = {i: hashlib.sha1('\n'.join(self.skeleton(r) for r in b).encode()).hexdigest()[:16] for i, b in self.fns.items()}
    def is_data(self, v): return 1024 <= v < self.dend
    def normalise(self, body):
        out = []
        for ins in body:
            m = CONST.match(ins)
            if m and self.is_data(int(m.group(1))): ins = 'i32.const A'
            else:
                m = MEM.match(ins)
                if m and self.is_data(int(m.group(3))): ins = f'{m.group(1)} {m.group(2)} A'
            out.append(ins)
        return out
    @staticmethod
    def skeleton(ins):
        m = CONST.match(ins)
        if m: return 'i32.const'
        m = MEM.match(ins)
        if m: return m.group(1)
        return ins

def table_equal(tm, tb, pm, pb, assume=None):
    """rows equal, or i32.const table slots on both sides whose functions are hash-equal / table_equal.
    Mutually recursive callbacks (a Walker storing &resolveExprStep and &resolveSelectStep) are handled
    co-inductively: a pair already under comparison is assumed equal."""
    if len(tb) != len(pb): return False
    assume = set() if assume is None else assume
    for a, b in zip(tb, pb):
        if a == b: continue
        ma, mb = CONST.match(a), CONST.match(b)
        if not (ma and mb): return False
        va, vb = int(ma.group(1)), int(mb.group(1))
        if tm.is_data(va) and pm.is_data(vb): continue
        fa, fb = tm.elem.get(va), pm.elem.get(vb)
        if fa is None or fb is None: return False
        if fa not in tm.fns or fb not in pm.fns:          # an import in the table
            if fa not in tm.fns and fb not in pm.fns: continue
            return False
        if tm.hash[fa] == pm.hash[fb] or (fa, fb) in assume: continue
        if len(assume) < 64 and table_equal(tm, tm.fns[fa], pm, pm.fns[fb], assume | {(fa, fb)}): continue
        return False
    return True

T = Mod(tw, 'build/dump/vota.objdump.txt')
P = Mod(pw, f'{work}/{name}.plain.objdump.txt')
N = Mod(f'build/out/sqlite/{name}/named/wasm/vota_web/vota_web_wasm.wasm', f'{work}/{name}.named.objdump.txt')
cls = {int(k): v for k, v in json.load(open(clsf)).items()}
sym = {}
for l in open(symf):
    l = l.strip()
    if ':' in l:
        i, n = l.split(':', 1); sym[int(i)] = n
byhash_named = collections.defaultdict(list)
for i, h in N.hash.items(): byhash_named[h].append(sym.get(i, '?'))
def pname(i): return '|'.join(sorted(set(byhash_named.get(P.hash[i], ['?']))))

def jl(p): return [json.loads(l) for l in open(p)]
def bodies(wasm, fjsonl):
    b = open(wasm, 'rb').read()
    return {j['index']: b[j['bodyOffset']:j['bodyOffset'] + j['size']] for j in jl(fjsonl)}
TB = bodies(tw, f'{work}/target.functions.jsonl'); PB = bodies(pw, f'{work}/{name}.plain.functions.jsonl')
pbyraw = collections.defaultdict(list)
for i, b in PB.items(): pbyraw[b].append(i)
pbyhash = collections.defaultdict(list)
for i, h in P.hash.items(): pbyhash[h].append(i)
pbyskel = collections.defaultdict(list)
for i, h in P.skel.items(): pbyskel[h].append(i)
c0 = json.load(open(f'{work}/{name}.census0.json'))
tk = {p['a']: p for p in c0['pairs']}

res = {}
for ti in sorted(T.fns):
    cands = pbyraw.get(TB[ti], [])
    if cands: res[ti] = {'cand': cands[0], 'kind': 'byte'}; continue
    cands = pbyhash.get(T.hash[ti], [])
    if cands: res[ti] = {'cand': cands[0], 'kind': 'exact'}; continue
    hit = None
    for pi in pbyskel.get(T.skel[ti], []):
        if table_equal(T, T.fns[ti], P, P.fns[pi]): hit = pi; break
    if hit is not None: res[ti] = {'cand': hit, 'kind': 'table'}; continue
    p = tk.get(ti)
    if p: res[ti] = {'cand': p['b'], 'kind': p['by']}; continue
    res[ti] = {'cand': None, 'kind': 'none'}
for ti, r in res.items():
    r['class'] = cls.get(ti, '?'); r['n'] = len(T.fns[ti])
    r['name'] = pname(r['cand']) if r['cand'] is not None else None
KINDS = ('byte', 'exact', 'table', 'fingerprint', 'shape', 'none')
GOOD = ('byte', 'exact', 'table')
def summary(idxs):
    c = collections.Counter(res[i]['kind'] for i in idxs)
    return {'fns': len(idxs), **{k: c.get(k, 0) for k in KINDS}, 'matched': sum(c.get(k, 0) for k in GOOD),
            'instr': sum(len(T.fns[i]) for i in idxs), 'instr_matched': sum(len(T.fns[i]) for i in idxs if res[i]['kind'] in GOOD)}
sq = [i for i in T.fns if cls.get(i) == 'sqlite']
hit = {r['cand'] for r in res.values() if r['kind'] in GOOD}
# coverage from the probe's side: how many of its functions have a match in the target, pairing hash groups by count
tcount = collections.Counter(T.hash.values())
pgroups = collections.defaultdict(list)
for i in P.fns: pgroups[P.hash[i]].append(i)
covered = set()
for h, ps in pgroups.items():
    for i in ps[:tcount.get(h, 0)]: covered.add(i)
for i in hit:
    if i not in covered and i is not None: covered.add(i)      # byte/table matches outside the hash grouping
SQLITE_SYMS = set(open('tools/libs/sqlite/symbols.txt').read().split())   # llvm-nm of sqlite3.o (defined functions)
def is_sqlite(n): return any(x in SQLITE_SYMS for x in n.split('|'))
probe_sqlite = [i for i in P.fns if is_sqlite(pname(i))]
coverage = {'probe_fns': len(P.fns), 'probe_sqlite_fns': len(probe_sqlite),
            'probe_sqlite_found_in_target': sum(1 for i in probe_sqlite if i in covered),
            'probe_sqlite_instr': sum(len(P.fns[i]) for i in probe_sqlite),
            'probe_sqlite_instr_found': sum(len(P.fns[i]) for i in probe_sqlite if i in covered)}
unhit = sorted(((len(P.fns[i]), pname(i)) for i in P.fns if i not in hit), reverse=True)
out = {'variant': name, 'cflags': open(f'build/out/sqlite/{name}/cflags.txt').read().split(),
       'summary': {'sqlite_class': summary(sq), 'all_target': summary(list(T.fns)), 'coverage': coverage},
       'plain_fns': len(P.fns), 'plain_unhit': [f'{n} {s}' for s, n in unhit],
       'sqlite_attribution': {str(i): r['name'] for i, r in res.items() if r['kind'] in GOOD and r['name'] and is_sqlite(r['name'])
                              and all(x in SQLITE_SYMS for x in r['name'].split('|')) and (tcount[T.hash[i]] == 1 or len(T.fns[i]) >= 10)},
       'functions': {str(i): res[i] for i in sorted(T.fns) if res[i]['kind'] != 'none' or cls.get(i) == 'sqlite'}}
json.dump(out, open(outf, 'w'), indent=0)
print(name, json.dumps(out['summary']))
print('plain fns', len(P.fns), 'unhit', len(unhit), 'target fns attributed to sqlite', len(out['sqlite_attribution']))
with open(outf.replace('census.json', 'attribution.tsv'), 'w') as f:
    f.write('target_fn\tkind\tsqlite_function\tinstr\tcls2_class\n')
    for i in sorted(int(k) for k in out['sqlite_attribution']):
        f.write(f"{i}\t{res[i]['kind']}\t{res[i]['name']}\t{len(T.fns[i])}\t{cls.get(i)}\n")
