"""Normalize wasm-objdump -d output into per-function opcode sequences and hashes.
usage: wasmfn.py dump <objdump.txt> <out.jsonl> [<module.wasm>]  -> one line per function: {idx, n, hash, body}
       wasmfn.py cmp <probe.jsonl> <symbols> <target.jsonl> name...  -> for each named probe function, find target functions with same normalized body
Normalization: drop addresses/bytes; `call N` -> `call F`; `i32.const N` with N>=1024 or N<0 -> `i32.const A`; `br_table` kept; local indices kept;
`call_indirect T (type N)` -> `call_indirect T <signature>` when the module is given (type indices are per module: the target has 155
types, a probe 70, so the same source differs in every indirect call otherwise); without the module the index is kept and a warning printed.
"""
import sys, re, json, hashlib, subprocess
def type_signatures(wasm):
    """type index -> '(i32, i32) -> i32' from `wasm-objdump -x -j Type`"""
    out=subprocess.run(['wasm-objdump','-x','-j','Type',wasm],capture_output=True,text=True,check=True).stdout
    return {int(m.group(1)):m.group(2).strip() for m in re.finditer(r'type\[(\d+)\] (.*)$',out,re.M)}
def parse(path, types=None):
    if types is None: print('warning: no module given, call_indirect keeps the per-module TYPE INDEX (bodies with indirect calls will not compare across builds)', file=sys.stderr)
    fns={}; cur=None; body=[]
    hdr=re.compile(r'^[0-9a-f]+ func\[(\d+)\]')
    ci=re.compile(r'^call_indirect (\d+) \(type (\d+)\)$')
    for line in open(path, errors='replace'):
        m=hdr.match(line)
        if m:
            if cur is not None: fns[cur]=body
            cur=int(m.group(1)); body=[]; continue
        if cur is None or '|' not in line: continue
        ins=line.split('|',1)[1].strip()
        if not ins or ins.startswith('local['): 
            if ins.startswith('local['): body.append(re.sub(r'local\[\d+(\.\.\d+)?\]','local',ins))
            continue
        ins=re.sub(r'^call (\d+)(.*)$', r'call F', ins)
        mci=ci.match(ins)
        if mci and types is not None: ins=f'call_indirect {mci.group(1)} {types[int(mci.group(2))]}'
        m2=re.match(r'^i32\.const (\d+)$', ins)
        if m2:
            v=int(m2.group(1))
            if v>=1024: ins='i32.const A'
        m3=re.match(r'^((?:i32|i64|f32|f64)\.(?:load|store)[0-9a-z_]*) (\d+) (\d+)$', ins)
        if m3 and int(m3.group(3))>=1024: ins=f'{m3.group(1)} {m3.group(2)} A'
        body.append(ins)
    if cur is not None: fns[cur]=body
    return fns
def dump(src,out,wasm=None):
    fns=parse(src, None if wasm is None else type_signatures(wasm))
    with open(out,'w') as f:
        for i,b in sorted(fns.items()):
            s='\n'.join(b); h=hashlib.sha1(s.encode()).hexdigest()[:16]
            f.write(json.dumps({'idx':i,'n':len(b),'hash':h,'body':b})+'\n')
    print('functions',len(fns))
def load(p):
    return {j['idx']:j for j in (json.loads(l) for l in open(p))}
def cmp(probe,syms,target,names):
    P=load(probe); T=load(target)
    byhash={}
    for j in T.values(): byhash.setdefault(j['hash'],[]).append(j['idx'])
    name2idx={}
    for l in open(syms):
        l=l.strip()
        if ':' in l:
            i,n=l.split(':',1); name2idx[n]=int(i)
    if names==['--all']: names=[n for n in name2idx if name2idx[n] in P]
    for n in names:
        if n not in name2idx: print(f'{n}: not in probe symbols'); continue
        i=name2idx[n]
        if i not in P: print(f'{n}: idx {i} is an import'); continue
        j=P[i]; hits=byhash.get(j['hash'],[])
        if hits: print(f'{n}: probe func[{i}] n={j["n"]} EXACT target match: {["func[%d]"%h for h in hits]}')
        else:
            # nearest by length & common prefix
            best=None
            for t in T.values():
                if abs(t['n']-j['n'])>max(8,j['n']//5): continue
                k=0
                for a,b in zip(j['body'],t['body']):
                    if a!=b: break
                    k+=1
                sc=k/max(j['n'],t['n'])
                if best is None or sc>best[0]: best=(sc,t['idx'],t['n'],k)
            print(f'{n}: probe func[{i}] n={j["n"]} NO exact match; nearest target func[{best[1]}] n={best[2]} common-prefix {best[3]} ({best[0]:.2f})' if best else f'{n}: probe func[{i}] n={j["n"]} no candidate by length')
if __name__=='__main__':
    if sys.argv[1]=='dump': dump(sys.argv[2],sys.argv[3],sys.argv[4] if len(sys.argv)>4 else None)
    else: cmp(sys.argv[2],sys.argv[3],sys.argv[4],sys.argv[5:])
