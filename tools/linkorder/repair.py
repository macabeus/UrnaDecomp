"""Drop the anchors of the smaller chunk of every inconsistent chain link (a wrong identification), writing a repaired anchor file.
usage: repair.py anchors.tsv badlinks.tsv out.tsv"""
import sys
anc, bad, out = sys.argv[1:4]
drop = set()
for l in open(bad):
    p = l.rstrip('\n').split('\t')
    if p[0] == 'object': continue
    a = p[2].split(','); b = p[4].split(',')
    victim = a if len(a) < len(b) else (b if len(b) < len(a) else a + b)
    drop |= {int(x) for x in victim if x}
n = 0
with open(out, 'w') as f:
    for l in open(anc):
        p = l.split('\t')
        if p[0].isdigit() and int(p[0]) in drop: continue
        f.write(l); n += 1
print('dropped', len(drop), 'anchors; kept', n - 1)
