# parse wasm-objdump -d dump: per function -> {idx, start, size, consts:set, calls:set}; writes funcs.json in the
# current (scratch) dir. Reads build/dump/vota.objdump.txt = wasm-objdump -d target/vota_web_wasm.wasm.
import os, re, json, sys
ROOT=os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
fn=None; funcs=[]
hdr=re.compile(r'^([0-9a-f]+) func\[(\d+)\]')
cst=re.compile(r'\| +i32\.const (\d+)$')
cal=re.compile(r'\| +call (\d+)$')
cali=re.compile(r'\| +call_indirect')
lastaddr=0
with open(ROOT+'/build/dump/vota.objdump.txt','r',errors='replace') as f:
    for line in f:
        m=hdr.match(line)
        if m:
            if fn: fn['end']=lastaddr; funcs.append(fn)
            fn={'idx':int(m.group(2)),'start':int(m.group(1),16),'consts':set(),'calls':set(),'ncall_indirect':0,'ninstr':0}
            continue
        if fn is None: continue
        if len(line)>8 and line[7]==':':
            try: lastaddr=int(line[1:7],16)
            except: pass
            fn['ninstr']+=1
        m=cst.search(line)
        if m: fn['consts'].add(int(m.group(1))); continue
        m=cal.search(line)
        if m: fn['calls'].add(int(m.group(1))); continue
        if cali.search(line): fn['ncall_indirect']+=1
if fn: fn['end']=lastaddr; funcs.append(fn)
for f in funcs: f['consts']=sorted(f['consts']); f['calls']=sorted(f['calls'])
json.dump(funcs,open('funcs.json','w'))
print(len(funcs),'functions; instr total',sum(f['ninstr'] for f in funcs))
