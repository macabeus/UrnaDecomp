"""Skeleton ground truth: final function index -> binaryen ordinal (post-Oz defined index) -> wasm-ld ordinal -> name -> object."""
import sys, os, glob, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wasmcode import Module, uses
ROOT=os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
L=f'{ROOT}/build/out/linkorder'
final=Module(f'{ROOT}/build/out/wasm/vota_web/vota_web_wasm.wasm')
named=Module(f'{L}/step10-named-g.wasm')            # metadce replay with index names
assert final.n_imported_funcs==named.n_imported_funcs and len(final.func_types)==len(named.func_types)
# code bytes identical?
fb=final.bytes; nb=named.bytes
same=sum(1 for (f,s,e),(g,s2,e2) in zip(final.code,named.code) if fb[s:e]==nb[s2:e2])
print('final vs metadce-replay: identical bodies',same,'of',len(final.code))
nm=named.names()
ord05={f:int(nm[f]) for f,_,_ in named.code}       # final index -> post-Oz defined index
# post-Oz defined index -> wasm-ld name ("k" or byn$...)
sym={}
for l in open(f'{L}/step05n.symbols'):
    i,n=l.rstrip('\n').split(':',1); sym[int(i)]=n
n05imp=sum(1 for n in sym.values() if n.startswith('fimport$'))
ldname={i-n05imp:n for i,n in sym.items() if not n.startswith('fimport$')}
basenames={}
for l in open(f'{L}/base-names.tsv'):
    i,n=l.rstrip('\n').split('\t',1); basenames[int(i)-142]=n
dups=collections.Counter(basenames.values()); print('wasm-ld names: duplicates',sum(1 for n,c in dups.items() if c>1),'e.g.',[n for n,c in dups.items() if c>1][:5])
# objects: link objects in order, then archive members (order unknown a priori)
objs=[]
for k,o in enumerate(open(f'{L}/link-objects.txt').read().split()):
    fns=[l.rstrip('\n').split('\t',1)[1] for l in open(f'{L}/objfn/{k+1:04d}.tsv')]
    objs.append((o,fns))
arobjs=[]
for d in sorted(glob.glob(f'{L}/arfn/lib*/')):
    lib=os.path.basename(d.rstrip('/'))
    for t in sorted(glob.glob(d+'*.tsv')):
        fns=[l.rstrip('\n').split('\t',1)[1] for l in open(t)]
        if fns: arobjs.append((lib+':'+os.path.basename(t)[:-4],fns))
print('link objects',len(objs),'archive members with code',len(arobjs))
# assign wasm-ld ordinals to objects: greedy walk in link order, then archive members by first-occurrence
N=len(basenames); owner={}; pos={}
cur=0
def take(label,fns,start):
    """match fns in order against basenames from start; return (matched ordinals, last+1)"""
    got=[]; i=start
    for r,f in enumerate(fns):
        # next occurrence of f at or after i
        j=i
        while j<N and basenames[j]!=f: j+=1
        if j<N and j-i<=0+len(fns):  # allow skipping GC'd functions (small gap)
            got.append((j,r)); i=j+1
    return got,i
for label,fns in objs:
    got,cur2=take(label,fns,cur)
    for j,r in got: owner[j]=label; pos[j]=r
    if got: cur=got[-1][0]+1
print('assigned to explicit link objects',len(owner),'first unassigned ordinal',min(j for j in range(N) if j not in owner))
# archive members: find for each member the first ordinal of its first function after cur; order members by that
rest=[j for j in range(N) if j not in owner]
name2ords=collections.defaultdict(list)
for j in rest: name2ords[basenames[j]].append(j)
placed=[]
for label,fns in arobjs:
    cands=[name2ords[f][0] for f in fns if name2ords.get(f)]
    if cands: placed.append((min(cands),label,fns))
placed.sort()
cur=min(rest)
for start,label,fns in placed:
    got,_=take(label,fns,cur)
    for j,r in got:
        if j in owner: continue
        owner[j]=label; pos[j]=r
    if got: cur=max(cur,got[0][0])
un=[j for j in range(N) if j not in owner]
print('unassigned wasm-ld ordinals',len(un),[ (j,basenames[j]) for j in un[:12]])
# contiguity: for each object, the ordinals it owns must be one contiguous run with pos increasing
runs=collections.defaultdict(list)
for j in sorted(owner): runs[owner[j]].append((j,pos[j]))
bad=0
for o,lst in runs.items():
    js=[j for j,_ in lst]; ps=[p for _,p in lst]
    if js!=list(range(js[0],js[-1]+1)) or ps!=sorted(ps): bad+=1
print('objects with a non-contiguous or disordered run:',bad,'of',len(runs))
order=sorted(runs,key=lambda o:runs[o][0][0])
with open(f'{L}/skeleton-objects.tsv','w') as f:
    f.write('first_ld_ordinal\tlast\tcount\tobject\n')
    for o in order: f.write(f'{runs[o][0][0]}\t{runs[o][-1][0]}\t{len(runs[o])}\t{o}\n')
# truth table
u=uses(final)
with open(f'{L}/skeleton-truth.tsv','w') as f:
    f.write('final_index\tuses\tord05\tld_ordinal\tname\tobject\tpos_in_object\n')
    for fi,_,_ in final.code:
        o5=ord05[fi]; ldn=ldname[o5]
        if ldn.isdigit():
            k=int(ldn); f.write(f'{fi}\t{u[fi]}\t{o5}\t{k}\t{basenames[k]}\t{owner.get(k,"?")}\t{pos.get(k,"")}\n')
        else:
            f.write(f'{fi}\t{u[fi]}\t{o5}\t\t{ldn}\t<binaryen>\t\n')
# the law: final order == sort by (uses desc, str(ord05) desc)
seq=[(fi,u[fi],str(ord05[fi])) for fi,_,_ in final.code]
pred=sorted(seq,key=lambda t:(-t[1],t[2].encode()),reverse=False)
# descending strcmp: sort by uses desc then string desc
pred=sorted(seq,key=lambda t:(t[1],t[2].encode()),reverse=True)
viol=sum(1 for a,b in zip(seq,pred) if a[0]!=b[0])
print('law check: functions out of place when sorting by (uses desc, str(ord05) desc):',viol)
print('ctors: final index',seq[-1][0],'ord05',seq[-1][2])
print('ord05 range',min(ord05.values()),max(ord05.values()),'count',len(ord05),'missing (metadce-removed) ord05:',sorted(set(range(max(ord05.values())+1))-set(ord05.values())))
