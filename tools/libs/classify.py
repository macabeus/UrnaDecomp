import json,collections,re,sys
funcs=json.load(open('funcs.json')); byidx={f['idx']:f for f in funcs}
seed={int(k):v for k,v in json.load(open('seed.json'))['seed'].items()}
vt=json.load(open('vtables.json')); table=json.load(open('table.json'))
def ns2cls(d):
    if re.match(r'^(std::__2|__cxxabiv1|\(anonymous namespace\)::itanium_demangle|std::)',d): return 'libcxx'
    if re.match(r'^(RHVoice|utf8::|rapidxml)',d): return 'rhvoice'
    if re.match(r'^(vota|comum|api|ecourna|simulador|Modulo[A-Z])',d): return 'app'
    if d.startswith('ASN1::'): return 'asn1'
    if d.startswith('boost'): return 'boost'
    if re.match(r'^(NArchive|NCompress|NWindows|NCoderMixer2|C[A-Z][A-Za-z0-9_]*$|I[A-Z][A-Za-z0-9_]*$)',d): return '7zip'
    return None
vseed={}
for r in vt:
    c=ns2cls(r['dem'])
    if not c: continue
    for f in r['funcs']: vseed.setdefault(f,collections.Counter())[c]+=1
conf=0
for f,cnt in vseed.items():
    c=cnt.most_common(1)[0][0]
    if f in seed and seed[f]!=c: conf+=1
    seed.setdefault(f,c)   # string evidence wins on conflict
print('vtable seeds',len(vseed),'conflicts with string seeds',conf,'total seeds',len(seed),collections.Counter(seed.values()),file=sys.stderr)
uses=collections.Counter(); intable=set(t for t in table if t is not None)
for f in funcs:
    for c in f['calls']: uses[c]+=1
for t in intable: uses[t]+=1
# monotonicity test: how often does uses increase from one idx to the next?
seq=[uses[f['idx']] for f in funcs]
inc=sum(1 for a,b in zip(seq,seq[1:]) if b>a)
print('use-count increases along idx:',inc,'of',len(seq)-1,file=sys.stderr)
callers=collections.defaultdict(set)
for f in funcs:
    for c in f['calls']: callers[c].add(f['idx'])
cls=dict(seed); how={k:'seed' for k in cls}
def propagate():
    ch=0
    for f in funcs:
        i=f['idx']
        if i in cls: continue
        cs={cls[c] for c in callers[i] if c in cls}; unk=[c for c in callers[i] if c not in cls]
        if len(cs)==1 and not unk: cls[i]=cs.pop(); how[i]='callers'; ch+=1; continue
        if f['calls']:
            cs={cls[c] for c in f['calls'] if c in cls}; unk=[c for c in f['calls'] if c not in cls and c>=136]
            if len(cs)==1 and not unk and len(f['calls'])>=2: cls[i]=cs.pop(); how[i]='callees'; ch+=1
    return ch
def smooth(maxgap):
    ch=0
    strata=collections.defaultdict(list)
    for f in funcs: strata[uses[f['idx']]].append(f['idx'])
    for u,ids in strata.items():
        n=len(ids)
        for k,i in enumerate(ids):
            if i in cls: continue
            p=k-1
            while p>=0 and ids[p] not in cls: p-=1
            q=k+1
            while q<n and ids[q] not in cls: q+=1
            if p>=0 and q<n and cls[ids[p]]==cls[ids[q]] and (q-p)<=maxgap:
                cls[i]=cls[ids[p]]; how[i]='smooth'; ch+=1
    return ch
for rnd in range(12):
    a=propagate(); b=smooth(60)
    if not a and not b: break
print('classified',len(cls),collections.Counter(how.values()),file=sys.stderr)
json.dump(cls,open('cls2.json','w'))
tot=collections.Counter(); size=collections.Counter(); seeds=collections.Counter(seed.values())
print('class\tfuncs\tseeded\tbytes')
for c,n in tot.most_common() if False else sorted(collections.Counter(cls.get(f['idx'],'unknown') for f in funcs).items(),key=lambda x:-x[1]):
    print(f"{c}\t{n}\t{seeds.get(c,0)}\t{sum(f['end']-f['start'] for f in funcs if cls.get(f['idx'],'unknown')==c)}")
