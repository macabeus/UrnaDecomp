# The funcref table and every vtable of the target: writes table.json and vtables.json in the current (scratch) dir,
# which holds mem.bin (tools/libs/memimg.py).
import os,struct,json,re,collections,sys,subprocess
ROOT=os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
W=ROOT+'/target/vota_web_wasm.wasm'
data=open(W,'rb').read(); mem=open('mem.bin','rb').read()
def leb(p):
    r=0;s=0
    while True:
        b=data[p];p+=1;r|=(b&0x7f)<<s;s+=7
        if not b&0x80:return r,p
pos=8; table=None
while pos<len(data):
    sid=data[pos];pos+=1;size,pos=leb(pos);end=pos+size
    if sid==9:
        n,p=leb(pos); assert n==1
        flags,p=leb(p); assert flags==0, flags
        assert data[p]==0x41; p+=1; off,p=leb(p); assert data[p]==0x0b; p+=1
        cnt,p=leb(p); table=[None]*(off+cnt)
        for i in range(cnt):
            fi,p=leb(p); table[off+i]=fi
        print('elem offset',off,'count',cnt,file=sys.stderr)
    pos=end
json.dump(table,open('table.json','w'))
# index of u32 values
idx=collections.defaultdict(list)
for a in range(0,len(mem)-3,4):
    v=struct.unpack_from('<I',mem,a)[0]
    if 1024<=v<len(mem): idx[v].append(a)
# RTTI names: find string starts matching mangled pattern
names=[]
for m in re.finditer(rb'\0((?:N[A-Za-z0-9_]+E|St[0-9]+[A-Za-z_][A-Za-z0-9_]*|[0-9]+[A-Za-z_][A-Za-z0-9_]*))\0',mem):
    names.append((m.start()+1,m.group(1).decode()))
print('rtti names',len(names),file=sys.stderr)
dem={}
raw=[n for a,n in names]
out=subprocess.run(['c++filt'],input='\n'.join('_Z'+n for n in raw),capture_output=True,text=True).stdout.split('\n')
for n,d in zip(raw,out): dem[n]=d
res=[]
for a,n in names:
    for T in idx.get(a,[]):
        T-=4  # typeinfo object starts 4 bytes before the name pointer
        if T<0: continue
        for X in idx.get(T,[]):
            if X<4 or struct.unpack_from('<I',mem,X-4)[0]!=0: continue  # offset-to-top must be 0
            slots=[]
            q=X+4
            while q+4<=len(mem):
                v=struct.unpack_from('<I',mem,q)[0]
                if 0<v<len(table) and table[v] is not None: slots.append(table[v]); q+=4
                else: break
            if slots: res.append({'name':n,'dem':dem[n],'vtable':X,'funcs':slots})
json.dump(res,open('vtables.json','w'))
print('vtables found',len(res),'funcs',len({f for r in res for f in r['funcs']}),file=sys.stderr)
