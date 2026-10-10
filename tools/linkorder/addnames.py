import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'extract'))
from wasmmod import Module, read_uleb
def uleb(v):
    out=bytearray()
    while True:
        c=v&0x7f; v>>=7
        if v: out.append(c|0x80)
        else: out.append(c); return bytes(out)
src,dst=sys.argv[1:3]
m=Module(src); b=m.bytes
names=bytearray()
nd=len(m.func_types)
sub=bytearray(uleb(nd))
for k in range(nd):
    s=str(k).encode(); sub+=uleb(m.n_imported_funcs+k)+uleb(len(s))+s
payload=uleb(4)+b'name'+bytes([1])+uleb(len(sub))+sub
sec=bytes([0])+uleb(len(payload))+payload
# insert right after the code/data sections: append at end (custom sections may appear anywhere after data? must precede nothing). Append at end.
open(dst,'wb').write(b+sec); print('wrote',dst,'named',nd)
