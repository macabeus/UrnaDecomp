import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'extract'))
from wasmmod import Module, read_uleb
src,dst=sys.argv[1:3]
m=Module(src); b=m.bytes
out=bytearray(b[:8])
for sid,a,e in m.sections:
    if sid==0:
        n,i=read_uleb(b,a)
        if b[i:i+n]==b'name': continue
    # re-emit section header: find header start = a - len(uleb(e-a)) - 1
    ln=e-a; hdr=bytearray([sid]); v=ln
    while True:
        c=v&0x7f; v>>=7
        if v: hdr.append(c|0x80)
        else: hdr.append(c); break
    out+=hdr+b[a:e]
open(dst,'wb').write(out); print('wrote',dst,len(out))
