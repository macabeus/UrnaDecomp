#!/bin/bash
# publish.sh <ordinals.tsv> <blocks.tsv>: ledger/ordinals.tsv, ledger/tu-clusters.tsv, docs/reference/tu-slices.md,
# build/out/LINKORDER2/linkline.md, and the window statistics of docs/reference/link-order.md §7.4
set -e
cd "$(dirname "$0")/../.."
ORD=$1; BLK=$2; L=build/out/LINKORDER2
cp "$ORD" ledger/ordinals.tsv
python3 -I tools/linkorder/tu_slices.py ledger/ordinals.tsv ledger/func-attribution.tsv ledger/source-locations.tsv ledger/vtables.json ledger/tu-clusters.tsv docs/reference/tu-slices.md
python3 -I tools/linkorder/linkline.py "$BLK" ledger/ordinals.tsv > $L/linkline.md
python3 - "$ORD" <<'PY'
import sys, collections, statistics
comp={}
for l in open('ledger/func-attribution.tsv'):
    p=l.rstrip('\n').split('\t')
    if p[0].isdigit(): comp[int(p[0])]=p[1]
APP={'ecourna','vota','comum','api','simulador','vota-web','asn1-modules','ASN1'}
rows=[]
for l in open(sys.argv[1]):
    p=l.rstrip('\n').split('\t')
    if p[0].isdigit(): rows.append((int(p[0]),int(p[8])-0 if p[8].isdigit() else None,int(p[9]) if p[9].isdigit() else None,p[7]))
def stats(rs):
    w=[hi-lo+1 for i,lo,hi,k in rs]
    return f'n={len(rs)} <=10: {sum(1 for x in w if x<=10)} <=50: {sum(1 for x in w if x<=50)} <=100: {sum(1 for x in w if x<=100)} <=500: {sum(1 for x in w if x<=500)} median {statistics.median(w) if w else 0:.0f}'
print('ALL (rank windows)      ', stats(rows))
print('anchored                ', stats([r for r in rows if r[3]=='anchored']))
print('unanchored              ', stats([r for r in rows if r[3]!='anchored']))
print('app components          ', stats([r for r in rows if comp.get(r[0]) in APP]))
for c in ['openssl','sqlite','7zip','RHVoice','libc++','boost']:
    print(f'component {c:14s}', stats([r for r in rows if comp.get(r[0])==c]))
PY
echo "published from $ORD"
