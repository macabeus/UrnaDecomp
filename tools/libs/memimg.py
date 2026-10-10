# build a memory image from the wasm data segments; print segment table
import sys, struct
data = open(sys.argv[1],'rb').read()
pos = 8
def leb(p):
    r=0; s=0
    while True:
        b=data[p]; p+=1; r|=(b&0x7f)<<s; s+=7
        if not b&0x80: return r,p
def sleb(p):
    r=0; s=0
    while True:
        b=data[p]; p+=1; r|=(b&0x7f)<<s; s+=7
        if not b&0x80:
            if b&0x40: r-=1<<s
            return r,p
segs=[]
while pos < len(data):
    sid=data[pos]; pos+=1
    size,pos=leb(pos)
    end=pos+size
    if sid==11:
        n,p=leb(pos)
        for i in range(n):
            flags,p=leb(p)
            off=None
            if flags==0 or flags==2:
                if flags==2: mi,p=leb(p)
                assert data[p]==0x41; p+=1
                off,p=sleb(p); assert data[p]==0x0b; p+=1
            ln,p=leb(p)
            segs.append((off,p,ln))
            p+=ln
    pos=end
mem=bytearray(0x2000000)
for off,fo,ln in segs:
    if off is not None: mem[off:off+ln]=data[fo:fo+ln]
open(sys.argv[2],'wb').write(mem)
print(len(segs),'segments; first',segs[0][0],'last end',max(o+l for o,f,l in segs if o is not None))
import collections
sizes=sorted(((l,o) for o,f,l in segs if o is not None), reverse=True)[:10]
print('largest',sizes)
