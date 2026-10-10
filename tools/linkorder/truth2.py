"""Skeleton ground truth for the frozen skeleton: final index -> post-Oz ordinal -> wasm-ld ordinal -> name -> object/pos.
What truth.py + segment.py compute on build/out/linkorder, computed on build/out/LINKORDER2; the archive members'
function lists come from build/out/linkorder/arfn/."""
import sys, os, glob, collections, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wasmcode import Module, uses
ROOT=os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
L=f'{ROOT}/build/out/LINKORDER2'
OLD=f'{ROOT}/build/out/linkorder'
final=Module(f'{L}/frozen/plain.wasm'); named=Module(f'{L}/step10-named-g.wasm')
assert final.n_imported_funcs==named.n_imported_funcs and len(final.func_types)==len(named.func_types)
fb=final.bytes; nb=named.bytes
same=sum(1 for (f,s,e),(g,s2,e2) in zip(final.code,named.code) if fb[s:e]==nb[s2:e2])
print('final vs metadce-replay: identical bodies',same,'of',len(final.code))
nm=named.names(); ord05={f:int(nm[f]) for f,_,_ in named.code}
sym={}
for l in open(f'{L}/step05n.symbols'):
    i,n=l.rstrip('\n').split(':',1); sym[int(i)]=n
n05imp=sum(1 for n in sym.values() if n.startswith('fimport$'))
ldname={i-n05imp:n for i,n in sym.items() if not n.startswith('fimport$')}
base=Module(f'{L}/tmp/emscripten_temp/emcc-01-base.wasm'); nimp=base.n_imported_funcs
basenames={}
for l in open(f'{L}/base-names.tsv'):
    i,n=l.rstrip('\n').split('\t',1)
    if int(i)>=nimp: basenames[int(i)-nimp]=n
N=len(basenames); print('wasm-ld defined functions',N,'imports',nimp)
dm={}
for l in open(f'{L}/demangle-map.tsv'):
    a,b=l.rstrip('\n').split('\t',1); dm[a]=b
objs=[]
for k,o in enumerate(open(f'{L}/link-objects.txt').read().split()):
    fns=[dm.get(l.rstrip('\n').split('\t',1)[1], l.rstrip('\n').split('\t',1)[1]) for l in open(f'{L}/objfn/{k+1:04d}.tsv') if '\t' in l]
    objs.append((o,fns,k))
for d in sorted(glob.glob(f'{OLD}/arfn/lib*/')):
    lib=os.path.basename(d.rstrip('/'))
    for t in sorted(glob.glob(d+'*.tsv')):
        fns=[dm.get(l.rstrip('\n').split('\t',1)[1], l.rstrip('\n').split('\t',1)[1]) for l in open(t) if '\t' in l]
        if fns: objs.append((lib+':'+os.path.basename(t)[:-4],fns,10**6))
print('objects',len(objs))
byname=collections.defaultdict(list)
for oi,(lab,fns,_) in enumerate(objs):
    for r,f in enumerate(fns): byname[f].append((oi,r))
def score(oi,start_r,j,look=25):
    fns=objs[oi][1]; r=start_r; s=0
    for jj in range(j,min(N,j+look)):
        f=basenames[jj]
        try: r2=fns.index(f,r)
        except ValueError: break
        s+=1; r=r2+1
    return s
owner=[None]*N; posn=[None]*N; used=set(); cur=None; curr=0; j=0
while j<N:
    f=basenames[j]
    if cur is not None:
        fns=objs[cur][1]
        try:
            r2=fns.index(f,curr); owner[j]=cur; posn[j]=r2; curr=r2+1; j+=1; continue
        except ValueError: pass
    cands=[(oi,r) for oi,r in byname.get(f,[]) if oi not in used]
    if not cands: owner[j]=None; j+=1; cur=None; continue
    best=max(cands,key=lambda c:(score(c[0],c[1],j),-objs[c[0]][2]))
    cur,r=best; used.add(cur); owner[j]=cur; posn[j]=r; curr=r+1; j+=1
run_rows=[]; i=0
while i<N:
    o=owner[i]; k=i
    while k+1<N and owner[k+1]==o: k+=1
    run_rows.append((i,k,k-i+1,objs[o][0] if o is not None else '?')); i=k+1
with open(f'{L}/skeleton-objects.tsv','w') as f:
    f.write('first_ld_ordinal\tlast\tcount\tobject\n')
    for r in run_rows: f.write('\t'.join(map(str,r))+'\n')
labels=[r[3] for r in run_rows]; multi=collections.Counter(labels)
print('runs',len(run_rows),'unowned ordinals',sum(1 for o in owner if o is None),'objects used',len(used),'of',len(objs))
print('objects in >1 run:',[(l,c) for l,c in multi.items() if c>1 and l!='?'][:20])
expl=[(r[0],objs[[o for o,(lab,_,_) in enumerate(objs) if lab==r[3]][0]][2],r[3]) for r in run_rows if r[3]!='?' and ':' not in r[3]]
print('explicit objects: runs',len(expl),'link-rank inversions',sum(1 for a,b in zip(expl,expl[1:]) if b[1]<a[1]))
u=uses(final)
with open(f'{L}/skeleton-truth.tsv','w') as f:
    f.write('final_index\tuses\tord05\tld_ordinal\tname\tobject\tpos_in_object\n')
    for fi,_,_ in final.code:
        o5=ord05[fi]; ldn=ldname[o5]
        if ldn.isdigit():
            k=int(ldn); o=owner[k]
            f.write(f'{fi}\t{u[fi]}\t{o5}\t{k}\t{basenames[k]}\t{objs[o][0] if o is not None else "?"}\t{posn[k] if o is not None else ""}\n')
        else: f.write(f'{fi}\t{u[fi]}\t{o5}\t\t{ldn}\t<binaryen>\t\n')
seq=[(fi,u[fi],str(ord05[fi])) for fi,_,_ in final.code]
pred=sorted(seq,key=lambda t:(t[1],t[2].encode()),reverse=True)
print('LAW CHECK: out of place when sorting by (uses desc, str(ord05) desc):',sum(1 for a,b in zip(seq,pred) if a[0]!=b[0]))
print('ctors: final index',seq[-1][0],'ord05',seq[-1][2])
allo=set(ord05.values()); print('ord05 range',min(allo),max(allo),'count',len(allo),'holes:',sorted(set(range(max(allo)+1))-allo))
print('binaryen (mgfn) functions',sum(1 for fi,_,_ in final.code if not ldname[ord05[fi]].isdigit()), 'their ord05 min', min(ord05[fi] for fi,_,_ in final.code if not ldname[ord05[fi]].isdigit()))
json.dump({'owner':[objs[o][0] if o is not None else None for o in owner],'pos':posn},open(f'{L}/skeleton-owner.json','w'))
