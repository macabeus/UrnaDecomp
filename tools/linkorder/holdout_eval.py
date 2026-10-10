"""Hold-out consistency: for each held-out anchor (t, object, pos), solved WITHOUT it, test whether t's rank window
contains an ordinal strictly between the ordinal windows of its object's nearest held-in members below and above pos.

    python3 -I tools/linkorder/holdout_eval.py [<dir>]   # <dir> holds the three inputs below; default build/out/LINKORDER2

Inputs: ordinals-target-holdout.tsv (the solve of the held-in anchors), anchors-target-heldout.tsv and
anchors-target-holdin.tsv (a random 10 % split of the anchors; docs/reference/link-order.md §6)."""
import os, sys, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from linkorder import string_rank
ROOT=os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
L=os.path.join(sys.argv[1] if len(sys.argv) > 1 else ROOT+'/build/out/LINKORDER2', '')
ords={}
for l in open(L+'ordinals-target-holdout.tsv'):
    p=l.rstrip('\n').split('\t')
    if p[0].isdigit(): ords[int(p[0])]=(int(p[8]),int(p[9]),p[7])
M=14345; rank_of, ord_of_rank = string_rank(M)
def ordset(t):
    lo,hi,_=ords[t]; return set(int(x) for x in ord_of_rank[lo:hi+1])
held=[l.rstrip('\n').split('\t') for l in open(L+'anchors-target-heldout.tsv') if l.split('\t')[0].isdigit()]
kept=collections.defaultdict(list)
for l in open(L+'anchors-target-holdin.tsv'):
    p=l.rstrip('\n').split('\t')
    if p[0].isdigit(): kept[p[1]].append((int(p[2]),int(p[0])))
for o in kept: kept[o].sort()
ok=bad=skip=0; tight_ok=tight_bad=0; widths=[]
for t,obj,pos in held:
    t=int(t); pos=int(pos); mem=kept.get(obj,[])
    below=[m for m in mem if m[0]<pos]; above=[m for m in mem if m[0]>pos]
    if not below and not above: skip+=1; continue
    S=ordset(t); w=len(S); widths.append(w)
    lo=-1; hi=M
    if below:
        pb,tb=below[-1]; lo=min(ordset(tb))        # t must be above the lowest possible ordinal of the member below
    if above:
        pa,ta=above[0]; hi=max(ordset(ta))         # and below the highest possible ordinal of the member above
    good=any(lo<x<hi for x in S)
    if good: ok+=1
    else: bad+=1
    if w<=50:
        if good: tight_ok+=1
        else: tight_bad+=1
import statistics
print(f'held-out {len(held)}: consistent {ok}, inconsistent {bad}, no neighbour {skip}; median window {statistics.median(widths):.0f}; windows <=50: {tight_ok+tight_bad} of which inconsistent {tight_bad}')
