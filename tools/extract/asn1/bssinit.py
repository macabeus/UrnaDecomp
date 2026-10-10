# Emulate the straight-line constant stores of the static initialiser (func 14478) to recover BSS records.
import re, struct, sys
import os
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
SP = ROOT + '/build/out/asn1-schema'
mem = bytearray(open(SP+'/mem.bin','rb').read())
FN = sys.argv[1] if len(sys.argv) > 1 else '14478'
lines = []
p = False
for line in open(ROOT + '/build/dump/vota.objdump.txt', encoding='utf-8', errors='replace'):
    if re.match(r'^[0-9a-f]+ func\[', line):
        if p: break
        p = ('func[%s]' % FN) in line
        continue
    if p and '|' in line:
        lines.append(line.split('|',1)[1].strip())
stack = []; locals_ = {}
writes = 0
def u32(a): return struct.unpack_from('<I', mem, a)[0]
for ins in lines:
    parts = ins.split()
    if not parts: continue
    op = parts[0]
    try:
        if op == 'i32.const': stack.append(int(parts[1]) & 0xffffffff)
        elif op == 'i64.const': stack.append(int(parts[1]) & 0xffffffffffffffff)
        elif op == 'local.tee': locals_[int(parts[1])] = stack[-1]
        elif op == 'local.set': locals_[int(parts[1])] = stack.pop()
        elif op == 'local.get': stack.append(locals_.get(int(parts[1]), None))
        elif op == 'i32.load' and len(parts) == 3:
            a = stack.pop(); off = int(parts[2])
            stack.append(u32(a + off) if a is not None and a + off + 4 <= len(mem) else None)
        elif op in ('i32.store', 'i64.store', 'i32.store8', 'i32.store16') and len(parts) == 3:
            v = stack.pop(); a = stack.pop(); off = int(parts[2])
            if a is None or v is None: continue
            a += off
            if a + 8 > len(mem): continue
            if op == 'i32.store': struct.pack_into('<I', mem, a, v & 0xffffffff)
            elif op == 'i64.store': struct.pack_into('<Q', mem, a, v)
            elif op == 'i32.store8': mem[a] = v & 0xff
            else: struct.pack_into('<H', mem, a, v & 0xffff)
            writes += 1
        elif op == 'i32.add':
            b = stack.pop(); a = stack.pop(); stack.append(None if a is None or b is None else (a + b) & 0xffffffff)
        elif op == 'drop': stack.pop()
        else:
            stack.clear()   # a call or anything else: forget
    except IndexError:
        stack.clear()
open(SP+'/mem-init.bin','wb').write(mem)
print(len(lines), 'instructions,', writes, 'constant stores applied')
