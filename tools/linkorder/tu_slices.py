"""Slice the recovered order into translation-unit stretches.

Inside one use-count band the module order is the descending decimal string of the link ordinal, so within a digit
zone it is exact REVERSE link order: a translation unit's functions of that band form one run (its lower-zone
twins — ordinals with one digit less and the same leading digits — are sprinkled in at 1:10). Library objects are
anchored by the solver (ledger/ordinals.tsv); the unanchored functions between them are the app's TUs. Each
function is labelled with its probable source file: the std::source_location records it references
(ledger/source-locations.tsv), else the file of its vtable class (class -> file learned from the records, else the
directory convention <namespace dir>/<lowercase class>.cpp), else nothing (it joins its neighbours' run).
A run = maximal band stretch of one file label (unlabelled functions inherit; a one-function interruption whose
label returns right after is a lower-zone twin and is set aside into the twin sequence, which is sliced the same
way). Functions of other bands with a label join the TU of that label.
usage: python3 -I tools/linkorder/tu_slices.py ledger/ordinals.tsv ledger/func-attribution.tsv ledger/source-locations.tsv
                                      ledger/vtables.json ledger/tu-clusters.tsv docs/reference/tu-slices.md [blocks.tsv]
"""
import sys, re, json, collections, statistics
ordp, attrp, slp, vtp, outp, docp = sys.argv[1:7]
blocksp = sys.argv[7] if len(sys.argv) > 7 else None
APP = {'ecourna', 'vota', 'comum', 'api', 'simulador', 'vota-web', 'asn1-modules', 'ASN1'}

rows = []
for l in open(ordp):
    p = l.rstrip('\n').split('\t')
    if p[0].isdigit():
        rows.append(dict(idx=int(p[0]), uses=int(p[1]), lo=int(p[2]), hi=int(p[3]), pt=int(p[4]), block=p[5], kind=p[7], row=len(rows),
                         rlo=int(p[8]) if len(p) > 9 and p[8].isdigit() else None, rhi=int(p[9]) if len(p) > 9 and p[9].isdigit() else None))
byidx = {r['idx']: r for r in rows}
M = max(r['hi'] for r in rows) + 1
ord_of_rank = sorted(range(M), key=lambda x: str(x).encode(), reverse=True)
def zone_window(rs):
    """ordinal hull of the rows' rank windows restricted to the digit zone of the window middle (the hull over every
    zone is meaningless: a rank window in the '1' group spans 1xxxx, 1xxx, 1xx ...)"""
    if any(r['rlo'] is None for r in rs): return min(r['lo'] for r in rs), max(r['hi'] for r in rs), ''
    lo = min(r['rlo'] for r in rs); hi = max(r['rhi'] for r in rs)
    span = ord_of_rank[lo:hi + 1]
    d = collections.Counter(len(str(o)) for o in span).most_common(1)[0][0]
    ords = [o for o in span if len(str(o)) == d]
    return min(ords), max(ords), f'{d}-digit'
comp = {}; evid = {}; detail = {}
for l in open(attrp):
    p = l.rstrip('\n').split('\t')
    if p[0].isdigit(): comp[int(p[0])] = p[1]; evid[int(p[0])] = p[2]; detail[int(p[0])] = p[3] if len(p) > 3 else ''

# ---- labels ----------------------------------------------------------------------------------------------------
def short(file):
    file = re.sub(r'^/home/rubio/\.conan2/p/b/[^/]+/b/src/ecourna/', 'ecourna/', file)
    file = re.sub(r'^/home/rubio/\.conan2/p/b/[^/]+/p/include/ecourna/', 'ecourna(include)/', file)
    file = re.sub(r'^/home/rubio/tse/uenux2/', 'uenux2/', file)
    return file
slfile = collections.defaultdict(collections.Counter)           # func -> files (count of records)
cls2file = collections.defaultdict(collections.Counter); ns2dir = collections.defaultdict(collections.Counter)
for l in open(slp):
    p = l.rstrip('\n').split('\t')
    if not p[0].isdigit(): continue
    file, fn = p[1], p[4]
    for f in p[5].split(','):
        if f.strip().isdigit(): slfile[int(f)][file] += 1
    m = re.match(r'^(.*?)\(', fn); q = m.group(1) if m else fn
    toks = re.findall(r'[A-Za-z_][A-Za-z0-9_]*(?:<[^()]*?>)?::', q)
    qual = ''.join(toks)[:-2] if toks else ''
    if qual and not file.endswith(('.hpp', '.h')):
        cls2file[qual][file] += 1
        ns2dir['::'.join(qual.split('::')[:-1])][file.rsplit('/', 1)[0]] += 1
vt = collections.defaultdict(set)
for v in json.load(open(vtp)):
    for f in v['functions']: vt[f].add(v['class'])
def class_file(c):
    base = c.split('<')[0]
    if base in cls2file: return cls2file[base].most_common(1)[0][0], 'class-record'
    ns = '::'.join(base.split('::')[:-1]); name = base.split('::')[-1]
    if ns in ns2dir and re.match(r'^[A-Z][A-Za-z0-9_]*$', name):
        return ns2dir[ns].most_common(1)[0][0] + '/' + name.lower() + '.cpp', 'class-convention'
    return None, None
# a directory-convention guess whose basename is that of exactly one recorded .cpp is that file (the namespace's
# most common directory is a guess; the record is not)
rec_by_base = collections.defaultdict(set)
for c, files in cls2file.items():
    for f in files: rec_by_base[f.rsplit('/', 1)[-1]].add(f)
def canon(f, kind):
    if kind == 'class-convention' and len(rec_by_base.get(f.rsplit('/', 1)[-1], ())) == 1: return next(iter(rec_by_base[f.rsplit('/', 1)[-1]])), 'class-convention'
    return f, kind
label = {}; label_kind = {}
for r in rows:
    i = r['idx']
    cpp = [f for f in slfile.get(i, ()) if not f.endswith(('.hpp', '.h'))]
    if cpp:
        label[i] = max(cpp, key=lambda f: slfile[i][f]); label_kind[i] = 'source_location'; continue
    cands = collections.Counter()
    for c in vt.get(i, ()):
        f, k = class_file(c)
        if f: cands[(f, k)] += 1
    if cands:
        (f, k), _ = cands.most_common(1)[0]; f, k = canon(f, k); label[i] = f; label_kind[i] = k

# ---- band walk ---------------------------------------------------------------------------------------------------
bands = []; cur = []; last = None
for r in rows:
    if r['uses'] != last and cur: bands.append(cur); cur = []
    cur.append(r); last = r['uses']
if cur: bands.append(cur)

def slice_sequence(seq, zone_note):
    """seq: band rows (unanchored) in band order. Returns runs: list of dict(rows, file, labelled, kind)."""
    labs = [label.get(r['idx']) for r in seq]
    # lower-zone twins: a labelled one-function interruption whose neighbours agree on another file
    twin = [False] * len(seq)
    for i in range(1, len(seq) - 1):
        if labs[i] and labs[i - 1] and labs[i] != labs[i - 1]:
            j = i + 1
            while j < len(seq) and labs[j] is None: j += 1
            if j < len(seq) and labs[j] == labs[i - 1] and j - i <= 2: twin[i] = True
    main = [r for r, t in zip(seq, twin) if not t]; twins = [r for r, t in zip(seq, twin) if t]
    runs = []
    def build(rs, note):
        out = []; curf = None; cur = []; pending = []
        for r in rs:
            f = label.get(r['idx'])
            if f is None: pending.append(r); continue
            if f == curf: cur += pending; cur.append(r); pending = []
            else:
                if cur:
                    # unlabelled functions between two runs: split them in the middle, mark the boundary uncertain
                    k = len(pending) // 2; cur += pending[:k]; rest = pending[k:]
                    out.append(dict(rows=cur, file=curf, note=note, boundary=len(pending)))
                    pending = rest
                cur = pending + [r]; pending = []; curf = f
        if cur: cur += pending; out.append(dict(rows=cur, file=curf, note=note, boundary=0))
        elif pending: out.append(dict(rows=pending, file=None, note=note, boundary=0))
        return out
    runs += build(main, zone_note)
    if twins: runs += build(twins, zone_note + '/lower-zone twins')
    return runs

stretches = []
for bi, band in enumerate(bands):
    uses = band[0]['uses']
    # segments of unanchored rows between anchored ones (anchored = library objects)
    seg = []; left = None
    def flush(seg, left, right):
        if not seg: return
        note = f'uses={uses}'
        for run in slice_sequence(seg, note):
            run['band'] = uses; run['left'] = left; run['right'] = right; stretches.append(run)
    for r in band:
        if r['kind'] == 'anchored':
            flush(seg, left, r); seg = []; left = r
        else: seg.append(r)
    flush(seg, left, None)

# ---- merge: the same file in several bands = one TU; the uses==1 run is the TU's anchor stretch ------------------
# keep one stretch per (band run); the TU view groups them by file
def comp_of(rs):
    c = collections.Counter(comp.get(r['idx'], '-') for r in rs); return c
def ord_window(rs):
    lo, hi, _ = zone_window(rs); return lo, hi
stretches = [s for s in stretches if s['rows']]
# library filler (unidentified library functions between two anchors) is not a TU stretch
stretches = [s for s in stretches if s['file'] or any(comp.get(r['idx']) in APP for r in s['rows'])]
# order stretches: by band (uses==1 first = the richest), then band position (reverse link order within a zone)
stretches.sort(key=lambda s: (s['band'] != 1, s['band'], s['rows'][0]['row']))
with open(outp, 'w') as f:
    f.write('stretch\tband_uses\tband_pos_lo\tband_pos_hi\tord_lo\tord_hi\tn\tn_labelled\tcomponent\tprobable_file\tlabel_kinds\tnote\tboundary_unlabelled\tleft_anchor\tright_anchor\tsample_indices\n')
    for k, s in enumerate(stretches):
        rs = s['rows']; lo, hi = ord_window(rs); c = comp_of(rs)
        kinds = collections.Counter(label_kind[r['idx']] for r in rs if r['idx'] in label and label[r['idx']] == s['file'])
        la = s['left']['block'] if s['left'] else ''; ra = s['right']['block'] if s['right'] else ''
        f.write(f"{k}\t{s['band']}\t{rs[0]['row']}\t{rs[-1]['row']}\t{lo}\t{hi}\t{len(rs)}\t{sum(kinds.values())}\t{','.join(f'{a}:{b}' for a, b in c.most_common(3))}\t{short(s['file']) if s['file'] else ''}\t{','.join(f'{a}:{b}' for a, b in kinds.items())}\t{s['note']}\t{s['boundary']}\t{la}\t{ra}\t{','.join(str(r['idx']) for r in rs[:10])}\n")
app_rows = [r for r in rows if comp.get(r['idx']) in APP]
placed = sum(len(s['rows']) for s in stretches)
print('stretches', len(stretches), 'functions in stretches', placed, 'of', len(rows), '| app functions', len(app_rows), 'labelled', sum(1 for r in app_rows if r['idx'] in label))
big = [s for s in stretches if s['file'] and len(s['rows']) >= 3]
print('stretches with a file and >= 3 functions:', len(big), 'functions', sum(len(s['rows']) for s in big), 'distinct files', len({s['file'] for s in big}))

# ---- doc -------------------------------------------------------------------------------------------------------
tu = collections.defaultdict(list)
for s in stretches:
    if s['file']: tu[s['file']].append(s)
with open(docp, 'w') as f:
    f.write('# TU slices of the target (band walk over the recovered order)\n\n')
    f.write('Generated by `tools/linkorder/tu_slices.py` from `ledger/ordinals.tsv`; one row per band stretch (a translation unit shows one '
            'stretch per use-count band it has functions in, the `uses=1` stretch being the richest). Band positions are exact '
            'reverse link order inside a digit zone; ordinal windows come from the solver (library anchors bound them). '
            '`probable file` = the std::source_location records the functions reference (kind `source_location`), else the file of '
            'their vtable class (`class-record` = learned from a record of that class; `class-convention` = <namespace dir>/<lowercase class>.cpp, a guess). '
            'Unlabelled functions between two runs are split in the middle (`boundary` = how many).\n\n')
    f.write(f'Stretches: {len(stretches)}; functions placed in a stretch: {placed} of {len(rows)}; app functions: {len(app_rows)}, labelled {sum(1 for r in app_rows if r["idx"] in label)}; '
            f'files with a stretch of >= 3 functions: {len({s["file"] for s in big})}.\n\n')
    f.write('| stretch | band | band pos | ordinal window | #fns | component | probable file(s) | evidence |\n|---|---|---|---|---|---|---|---|\n')
    for k, s in enumerate(stretches):
        rs = s['rows']; lo, hi = ord_window(rs); c = comp_of(rs)
        kinds = collections.Counter(label_kind[r['idx']] for r in rs if r['idx'] in label and label[r['idx']] == s['file'])
        ev = ', '.join(f'{a} {b}' for a, b in kinds.items()) or 'none (inherits neighbours)'
        if s['boundary']: ev += f'; boundary ±{s["boundary"] // 2}'
        if 'twins' in s['note']: ev += '; lower-zone twin sequence'
        f.write(f"| {k} | uses={s['band']} | {rs[0]['row']}–{rs[-1]['row']} | {lo}–{hi} | {len(rs)} | {', '.join(f'{a} {b}' for a, b in c.most_common(2))} | {short(s['file']) if s['file'] else '?'} | {ev} |\n")
print('wrote', outp, docp)
