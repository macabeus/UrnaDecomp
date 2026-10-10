"""Assign every wasm-ld ordinal of the skeleton to its object (explicit link objects + archive members) by walking
the ordinal sequence and segmenting it into runs, each run following one object's function order."""
import sys, os, glob, collections, json
ROOT=os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
L=f'{ROOT}/build/out/linkorder'
dm={}
for l in open(f'{L}/demangle-map.tsv'):
    a,b=l.rstrip('\n').split('\t',1); dm[a]=b
basenames=[l.rstrip('\n').split('\t',1)[1] for l in open(f'{L}/base-names.tsv')]
N=len(basenames)
objs=[]   # (label, [demangled names in object order], link_rank)
for k,o in enumerate(open(f'{L}/link-objects.txt').read().split()):
    fns=[dm[l.rstrip('\n').split('\t',1)[1]] for l in open(f'{L}/objfn/{k+1:04d}.tsv')]
    objs.append((o,fns,k))
for d in sorted(glob.glob(f'{L}/arfn/lib*/')):
    lib=os.path.basename(d.rstrip('/'))
    for t in sorted(glob.glob(d+'*.tsv')):
        fns=[dm[l.rstrip('\n').split('\t',1)[1]] for l in open(t)]
        if fns: objs.append((lib+':'+os.path.basename(t)[:-4],fns,10**6))
byname=collections.defaultdict(list)
for oi,(lab,fns,_) in enumerate(objs):
    for r,f in enumerate(fns): byname[f].append((oi,r))
def score(oi,start_r,j,look=25):
    """how many of basenames[j:j+look] appear in objs[oi] in order from start_r"""
    fns=objs[oi][1]; r=start_r; s=0
    for jj in range(j,min(N,j+look)):
        f=basenames[jj]
        try: r2=fns.index(f,r)
        except ValueError: break
        s+=1; r=r2+1
    return s
owner=[None]*N; posn=[None]*N
used=set(); cur=None; curr=0; runs=[]
j=0
while j<N:
    f=basenames[j]
    if cur is not None:
        fns=objs[cur][1]
        try:
            r2=fns.index(f,curr)
            # accept if nothing else matches much better? keep simple: continue the run
            owner[j]=cur; posn[j]=r2; curr=r2+1; j+=1; continue
        except ValueError: pass
    cands=[(oi,r) for oi,r in byname.get(f,[]) if oi not in used]
    if not cands:
        owner[j]=None; j+=1; cur=None; continue
    best=max(cands,key=lambda c:(score(c[0],c[1],j),-objs[c[0]][2]))
    cur,r=best; used.add(cur); owner[j]=cur; posn[j]=r; curr=r+1; runs.append((j,cur)); j+=1
# runs summary
run_rows=[]; i=0
while i<N:
    o=owner[i]; k=i
    while k+1<N and owner[k+1]==o: k+=1
    run_rows.append((i,k,k-i+1,objs[o][0] if o is not None else '?'))
    i=k+1
with open(f'{L}/skeleton-objects.tsv','w') as f:
    f.write('first_ld_ordinal\tlast\tcount\tobject\n')
    for r in run_rows: f.write('\t'.join(map(str,r))+'\n')
labels=[r[3] for r in run_rows]
print('runs',len(run_rows),'unowned ordinals',sum(1 for o in owner if o is None),'objects used',len(used),'of',len(objs))
multi=collections.Counter(labels); print('objects appearing in >1 run:',[(l,c) for l,c in multi.items() if c>1 and l!='?'][:20])
# link order check for explicit objects: first ordinal must increase with link rank
expl=[(r[0],objs[[o for o,(lab,_,_) in enumerate(objs) if lab==r[3]][0]][2],r[3]) for r in run_rows if r[3]!='?' and ':' not in r[3]]
inv=sum(1 for a,b in zip(expl,expl[1:]) if b[1]<a[1])
print('explicit objects: runs',len(expl),'link-rank inversions',inv)
print('first 20 runs:'); [print(r) for r in run_rows[:20]]
print('last 12 runs:'); [print(r) for r in run_rows[-12:]]
json.dump({'owner':[objs[o][0] if o is not None else None for o in owner],'pos':posn},open(f'{L}/skeleton-owner.json','w'))
