"""Seed of the owner classification: a target function whose constants point at a string literal of exactly one
library's sources belongs to that library. SQLite, OpenSSL and Boost.Regex literals are read from third_party/ (found
from this file's path). Everything else is read from the current directory, a scratch dir holding mem.bin
(tools/libs/memimg.py), funcs.json (tools/libs/funcs.py), src_rhv/ (RHVoice 1.14.0), src7z/ (7-Zip 21.02),
src_zlib/ (zlib 1.3.1) and emscripten/system/lib/ (copied out of the pinned image). The three source dirs are the
trees third_party/RHVoice, third_party/7zip and third_party/zlib hold: symlinks to them serve. Writes seed.json
there; tools/libs/classify.py extends it."""
import os,re,json,sys,collections
ROOT=os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
S='.'
TP=ROOT+'/third_party'
lit=re.compile(r'"((?:[^"\\\n]|\\.){4,})"')
def literals(roots,exts=('.c','.cpp','.h','.hpp','.cc','.inc','.inl','.cxx')):
    s=set()
    for root in roots:
        files=[root] if os.path.isfile(root) else [os.path.join(dp,f) for dp,dn,fn in os.walk(root) for f in fn]
        for p in files:
            if not p.endswith(exts): continue
            try: src=open(p,encoding='utf-8',errors='replace').read()
            except: continue
            for m in lit.finditer(src):
                v=m.group(1)
                if '\\' in v: continue
                s.add(v)
    return s
sets={
 'sqlite':literals([TP+'/sqlite']),
 'openssl':literals([TP+'/openssl/crypto',TP+'/openssl/providers',TP+'/openssl/include',TP+'/openssl/ssl/record']),
 'rhvoice':literals([S+'/src_rhv']),
 '7zip':literals([S+'/src7z']),
 'zlib':literals([S+'/src_zlib']),
 'libcxx':literals([S+'/emscripten/system/lib/libcxx',S+'/emscripten/system/lib/libcxxabi',S+'/emscripten/system/lib/libunwind']),
 'libc':literals([S+'/emscripten/system/lib/libc',S+'/emscripten/system/lib/compiler-rt',S+'/emscripten/system/lib/pthread'])|literals([S+'/emscripten/system/lib'],exts=('dlmalloc.c','emmalloc.c')),
 # Boost.Regex, header-only: its headers in the Boost release
 'boost':literals([TP+'/boost/boost/regex']+[TP+'/boost/boost/'+f for f in ('regex.hpp','regex.h','cregex.hpp','regex_fwd.hpp')]),
}
for k,v in sets.items(): print(k,len(v),file=sys.stderr)
mem=open(S+'/mem.bin','rb').read()
# string starts: every address a where mem[a-1]==0 and printable run
app_re=re.compile(r'/home/rubio|\b(vota|comum|api|ecourna|simulador)::|Modulo[A-Z]|N4vota|N5comum|N3api|N7ecourna|uenux|\bue[a-z]+\b|[çãõáéíóúâêôà]|Urna|urna|eleitor|Eleitor|candidat|Candidat|cargo|Cargo|secao|Secao|zona|Zona|votacao|Votacao|mesario|Mesario|RED\b|BU\b|\.jez|\.gif')
def classify(s):
    hits=[k for k,v in sets.items() if s in v]
    if app_re.search(s): hits.append('app')
    if len(hits)==1: return hits[0]
    if len(hits)>1:
        # prefer app paths; drop generic
        if 'app' in hits and '/home/rubio' in s: return 'app'
        return 'AMBIG:'+','.join(hits)
    return None
funcs=json.load(open(S+'/funcs.json'))
cache={}
def str_at(a):
    if a in cache: return cache[a]
    r=None
    if 1024<=a<len(mem) and (mem[a-1]==0) and mem[a]!=0:
        e=mem.find(b'\0',a)
        if 0<e-a<400:
            b=mem[a:e]
            if all(32<=c<127 or c in (9,10,13) or c>=128 for c in b):
                try: r=b.decode('utf-8')
                except: r=None
    cache[a]=r; return r
seed={}
detail={}
for f in funcs:
    tally=collections.Counter()
    for a in f['consts']:
        s=str_at(a)
        if s is None: continue
        c=classify(s)
        if c and not c.startswith('AMBIG'): tally[c]+=1
    if tally:
        best,n=tally.most_common(1)[0]
        seed[f['idx']]=best; detail[f['idx']]=dict(tally)
json.dump({'seed':seed,'detail':detail},open(S+'/seed.json','w'))
print('seeded',len(seed),collections.Counter(seed.values()),file=sys.stderr)
# propagate via callers: unclassified function whose callers are all one class
byidx={f['idx']:f for f in funcs}
callers=collections.defaultdict(set)
for f in funcs:
    for c in f['calls']: callers[c].add(f['idx'])
cls=dict(seed)
for it in range(30):
    changed=0
    for f in funcs:
        if f['idx'] in cls: continue
        cs={cls[c] for c in callers[f['idx']] if c in cls}
        unk=[c for c in callers[f['idx']] if c not in cls]
        if len(cs)==1 and not unk:
            cls[f['idx']]=cs.pop(); changed+=1
    # callee-only propagation: function calls only one class and nothing else
    for f in funcs:
        if f['idx'] in cls or not f['calls']: continue
        cs={cls[c] for c in f['calls'] if c in cls}
        unk=[c for c in f['calls'] if c not in cls and c>=136]
        if len(cs)==1 and not unk and len(f['calls'])>=2:
            cls[f['idx']]=cs.pop(); changed+=1
    if not changed: break
print('after propagation',len(cls),collections.Counter(cls.values()),file=sys.stderr)
tot=collections.Counter(); size=collections.Counter()
for f in funcs:
    c=cls.get(f['idx'],'unknown'); tot[c]+=1; size[c]+=f['end']-f['start']
print('class\tfuncs\tbytes')
for c,n in tot.most_common(): print(f'{c}\t{n}\t{size[c]}')
