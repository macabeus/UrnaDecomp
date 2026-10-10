"""Identify target functions with skeleton functions (frozen skeleton + its ground truth) and emit anchors:
index<TAB>block(object)<TAB>pos(position in the object's FULL function list)<TAB>source<TAB>size.
Sources: ledger pairs (byte/fingerprint/addr, depth 2), census depth-0 pairs (fp/addr), wasmfn EXACT by name,
sqlite attribution names, OPENSSL_FUNC own-name strings, then callee-structure propagation to a fixpoint.
Gates: the skeleton body must be unique in the skeleton and the target body unique in the target (norm_body),
or the evidence is a unique name; a skeleton function claimed by two targets (or vice versa) is dropped."""
import os, sys, json, collections, re
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wasmcode import Module, norm_body
ROOT=os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
U=ROOT+'/'; L=U+'build/out/LINKORDER2/'
W=Module(U+'target/vota_web_wasm.wasm'); S=Module(L+'frozen/plain.wasm')
truth={}
for l in open(L+'skeleton-truth.tsv'):
    p=l.rstrip('\n').split('\t')
    if p[0].isdigit(): truth[int(p[0])]=dict(uses=int(p[1]),ord05=int(p[2]),name=p[4],obj=p[5],pos=(int(p[6]) if p[6] else None))
name2s=collections.defaultdict(list)
for s,t in truth.items(): name2s[t['name']].append(s)
def bodies(m):
    nb={}; mult=collections.Counter()
    for f,_,_ in m.code:
        b=norm_body(m,f); nb[f]=b; mult[b]+=1
    return nb,mult
nbW,multW=bodies(W); nbS,multS=bodies(S)
print('target fns',len(nbW),'unique bodies',sum(1 for f,b in nbW.items() if multW[b]==1),'| skeleton fns',len(nbS),'unique',sum(1 for f,b in nbS.items() if multS[b]==1))
nins=lambda b: len(b)
cand=collections.defaultdict(dict)   # T -> {S: (tier, source)}
TIER={'byte':0,'name':1,'addr':2,'fingerprint':3,'exact':1,'prop':4,'shape':5}
def add(t,s,src,tier):
    if t not in nbW or s not in nbS: return
    if s not in cand[t] or TIER[tier]<TIER[cand[t][s][0]]: cand[t][s]=(tier,src)
# P1 ledger (frozen)
for l in open(L+'frozen/functions.tsv'):
    p=l.rstrip('\n').split('\t')
    if not p[0].isdigit() or p[5]!='skeleton' or p[7] not in ('byte','fingerprint','addr'): continue
    add(int(p[0]),int(p[6]),'ledger',p[7])
# P2 census depth 0
for pr in json.load(open(U+'build/out/census/census-d0.json'))['pairs']:
    if pr['by'] in ('fingerprint','addr'): add(pr['a'],pr['b'],'d0',pr['by'])
# P3 wasmfn EXACT by name
for l in open(U+'build/out/census/wasmfn-cmp.txt'):
    m=re.match(r'^(.*): probe func\[(\d+)\] n=(\d+) EXACT target match: \[(.*)\]$',l.rstrip('\n'))
    if not m: continue
    nm=m.group(1); ts=[int(x) for x in re.findall(r"func\[(\d+)\]",m.group(4))]
    ss=name2s.get(nm,[])
    if len(ss)==1 and len(ts)==1: add(ts[0],ss[0],'wasmfn','exact')
# P4 sqlite attribution
for l in open(U+'tools/libs/sqlite/attribution.tsv'):
    p=l.rstrip('\n').split('\t')
    if not p[0].isdigit(): continue
    ss=[s for s in name2s.get(p[2],[]) if truth[s]['obj'].startswith('sqlite/')]
    if len(ss)==1: add(int(p[0]),ss[0],'sqlite-name','name')
# P5 OPENSSL_FUNC own-name strings: a target function referencing a string equal to a libcrypto function name
ossl_names={n for n,ss in name2s.items() if len(ss)==1 and ('libcrypto' in truth[ss[0]]['obj'] or truth[ss[0]]['obj'].startswith('openssl/'))}
own=collections.defaultdict(set)
for l in open(U+'ledger/func-strings.tsv'):
    p=l.rstrip('\n').split('\t')
    if len(p)<4 or not p[0].isdigit() or p[2]!='str': continue
    if p[3] in ossl_names: own[int(p[0])].add(p[3])
for t,ns in own.items():
    if len(ns)==1: add(t,name2s[next(iter(ns))][0],'ossl-func','name')
print('candidate pairs',sum(len(v) for v in cand.values()),'targets',len(cand))
# gates + conflicts
def accept_round(cand):
    acc={}
    for t,d in cand.items():
        best=min(d.items(), key=lambda kv: TIER[kv[1][0]])
        s,(tier,src)=best
        bt,bs=nbW[t],nbS[s]
        if tier in ('name','exact'): acc[t]=(s,tier,src); continue
        if multW[bt]!=1 or multS[bs]!=1: continue              # non-unique bodies: not a safe anchor
        if nins(bt)<6: continue
        acc[t]=(s,tier,src)
    bys=collections.defaultdict(list)
    for t,(s,tier,src) in acc.items(): bys[s].append(t)
    for s,ts in bys.items():
        if len(ts)>1:
            ts.sort(key=lambda t: TIER[acc[t][1]])
            if TIER[acc[ts[0]][1]]<TIER[acc[ts[1]][1]]: ts=ts[1:]
            for t in ts: acc.pop(t,None)
    return acc
acc=accept_round(cand)
print('accepted after gates',len(acc), collections.Counter(v[1] for v in acc.values()))
# callee propagation
nimpW=W.n_imported_funcs; nimpS=S.n_imported_funcs
for it in range(8):
    votes=collections.defaultdict(collections.Counter)
    for t,(s,tier,src) in acc.items():
        ct,cs=W.calls[t],S.calls[s]
        if len(ct)!=len(cs): continue
        for a,b in zip(ct,cs):
            if a<nimpW or b<nimpS or a in acc: continue
            votes[a][b]+=1
    new=0
    for a,c in votes.items():
        if len(c)!=1: continue
        b=next(iter(c))
        if nins(nbW[a])<4 and c[b]<2: continue
        cand[a][b]=('prop','callee') if b not in cand[a] else cand[a][b]
    acc2=accept_round(cand); new=len(acc2)-len(acc); acc=acc2
    print(f'  propagation iter {it}: accepted {len(acc)} (+{new})')
    if new<=0: break
# anchors
out=[]; mg=[]; byobj=collections.Counter(); unk=0
for t,(s,tier,src) in sorted(acc.items()):
    tr=truth[s]
    if tr['obj']=='<binaryen>': mg.append((t,s)); continue
    if tr['obj']=='?' or tr['pos'] is None: unk+=1; continue
    out.append((t,tr['obj'],tr['pos'],f'{src}:{tier}',W.body[t][1]-W.body[t][0])); byobj[tr['obj']]+=1
with open(L+'anchors-target.tsv','w') as f:
    f.write('index\tblock\tpos\tsource\tsize\n')
    for r in out: f.write('\t'.join(map(str,r))+'\n')
with open(L+'mgfn-target.tsv','w') as f:
    for t,s in mg: f.write(f'{t}\t{s}\t{truth[s]["name"]}\n')
with open(L+'pairs-target-skeleton.tsv','w') as f:
    f.write('target\tskeleton\ttier\tsource\tname\tobject\tpos\n')
    for t,(s,tier,src) in sorted(acc.items()): f.write(f'{t}\t{s}\t{tier}\t{src}\t{truth[s]["name"]}\t{truth[s]["obj"]}\t{truth[s]["pos"]}\n')
print('anchors',len(out),'objects',len(byobj),'mgfn pairs',len(mg),'unowned skeleton fns',unk)
# per library coverage: skeleton live per object vs identified
live=collections.Counter(t['obj'] for t in truth.values())
def lib(o): return o.split('/')[0].split(':')[0]
cov=collections.defaultdict(lambda:[0,0,0])
for o,n in live.items(): cov[lib(o)][0]+=n; cov[lib(o)][1]+=byobj.get(o,0); cov[lib(o)][2]+=1
for k,(n,i,no) in sorted(cov.items(), key=lambda kv:-kv[1][0]): print(f'  {k:12s} skeleton-live {n:5d} identified {i:5d} ({100*i/max(n,1):.0f}%) objects {no}')
