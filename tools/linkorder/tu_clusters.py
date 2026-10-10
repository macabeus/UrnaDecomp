"""Group the recovered ordinals (ledger/ordinals.tsv) into contiguous runs: library objects (anchored blocks)
and the unanchored stretches between them (the app's translation units), labelled from the ledgers.
usage: python3 -I tools/linkorder/tu_clusters.py ledger/ordinals.tsv ledger/func-attribution.tsv ledger/vtables.json ledger/func-strings.tsv ledger/tu-clusters.tsv [max_window]
Columns: cluster  kind(object|app)  ord_lo  ord_hi  n  median_window  object|labels  sample"""
import sys, json, collections, re, statistics
ordp, attrp, vtp, fsp, outp = sys.argv[1:6]
maxw = int(sys.argv[6]) if len(sys.argv) > 6 else 10**9       # only functions whose ordinal window is at most this wide are placed
rows = []
for l in open(ordp):
    p = l.rstrip('\n').split('\t')
    if not p[0].isdigit(): continue
    rows.append(dict(idx=int(p[0]), uses=int(p[1]), lo=int(p[2]), hi=int(p[3]), point=int(p[4]) if p[4] else None, block=p[5], kind=p[7]))
comp = {}
for l in open(attrp):
    p = l.rstrip('\n').split('\t')
    if p[0].isdigit() and p[1] != '-': comp[int(p[0])] = p[1]
cls = collections.defaultdict(set)
for v in json.load(open(vtp)):
    ns = v['class'].split('<')[0]
    ns = '::'.join(ns.split('::')[:2]) if '::' in ns else ns
    for f in v['functions']: cls[f].add(ns)
fdir = collections.defaultdict(set)
for l in open(fsp):
    p = l.rstrip('\n').split('\t')
    if len(p) < 4 or not p[0].isdigit() or p[2] != 'str': continue
    m = re.search(r'/src/(.+)/[^/]+\.(?:cpp|h|hpp|c)$', p[3]) or re.search(r'^((?:crypto|providers|ssl)/[^/]+)/', p[3])
    if m: fdir[int(p[0])].add(m.group(1))
def labels(idxs):
    c = collections.Counter()
    for i in idxs:
        if i in comp: c['comp:' + comp[i]] += 1
        for ns in cls.get(i, ()): c['class:' + ns] += 1
        for d in fdir.get(i, ()): c['file:' + d] += 1
    return ', '.join(f'{k} {n}' for k, n in c.most_common(6))
unplaced = [r for r in rows if r['hi'] - r['lo'] + 1 > maxw]
rows = [r for r in rows if r['hi'] - r['lo'] + 1 <= maxw]
rows.sort(key=lambda r: (r['point'] if r['point'] is not None else (r['lo'] + r['hi']) // 2, r['idx']))
clusters = []; cur = None
for r in rows:
    key = ('object', r['block']) if r['kind'] == 'anchored' else ('app', '')
    pt = r['point'] if r['point'] is not None else (r['lo'] + r['hi']) // 2
    if cur and cur['key'] == key and (key[0] == 'app' or True): cur['rows'].append(r)
    else:
        cur = dict(key=key, rows=[r]); clusters.append(cur)
with open(outp, 'w') as f:
    f.write('cluster\tkind\tord_lo\tord_hi\tn\tmedian_window\tobject_or_labels\tsample_indices\n')
    for k, c in enumerate(clusters):
        rs = c['rows']; pts = [r['point'] if r['point'] is not None else (r['lo'] + r['hi']) // 2 for r in rs]
        w = statistics.median(r['hi'] - r['lo'] + 1 for r in rs)
        lab = c['key'][1] if c['key'][0] == 'object' else labels(r['idx'] for r in rs)
        f.write(f"{k}\t{c['key'][0]}\t{min(pts)}\t{max(pts)}\t{len(rs)}\t{w:.0f}\t{lab}\t{','.join(str(r['idx']) for r in rs[:8])}\n")
print('placed', len(rows), 'unplaced (window >', maxw, ')', len(unplaced)); print('clusters', len(clusters), 'objects', sum(1 for c in clusters if c['key'][0] == 'object'), 'app stretches', sum(1 for c in clusters if c['key'][0] == 'app'),
      'app functions', sum(len(c['rows']) for c in clusters if c['key'][0] == 'app'))
big = sorted((c for c in clusters if c['key'][0] == 'app'), key=lambda c: -len(c['rows']))[:12]
for c in big:
    pts = [r['point'] if r['point'] is not None else (r['lo'] + r['hi']) // 2 for r in c['rows']]
    print(f"  app stretch ~{min(pts)}..{max(pts)} n={len(c['rows'])}: {labels(r['idx'] for r in c['rows'])[:160]}")
