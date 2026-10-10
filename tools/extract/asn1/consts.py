import re, sys
import os
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
SP = ROOT + '/build/out/asn1-schema'
fre = re.compile(r'^[0-9a-f]+ func\[(\d+)\]')
cre = re.compile(r'\|\s+(i32|i64)\.const (-?\d+)\s*$')
out = open(SP + '/consts.tsv', 'w'); f = None; n = 0
LO, HI = 1024, 2100000
for line in open(ROOT + '/build/dump/vota.objdump.txt', encoding='utf-8', errors='replace'):
    m = fre.match(line)
    if m: f = m.group(1); continue
    m = cre.search(line)
    if not m or f is None: continue
    v = int(m.group(2))
    if m.group(1) == 'i32':
        if LO <= v < HI: out.write(f'{f}\t{v}\n'); n += 1
    else:
        v &= 0xffffffffffffffff
        for part in (v & 0xffffffff, v >> 32):
            if LO <= part < HI: out.write(f'{f}\t{part}\n'); n += 1
print(n, 'constants')
