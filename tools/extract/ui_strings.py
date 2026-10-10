"""List the user-facing (pt-BR) strings: every non-ASCII Latin-1/UTF-8 string plus every ASCII string with a space
that a function attributed to the TSE app (vota/comum/api/simulador/ecourna/vota-web) loads, minus paths, format
skeletons and pretty-function names.
usage: python3 -I tools/extract/ui_strings.py <strings.tsv> <func-strings.tsv> <func-attribution.tsv> <ui-strings.tsv>"""
import sys, re, collections
strings_tsv, refs_tsv, attr_tsv, out = sys.argv[1:5]
attr = {}
for l in open(attr_tsv, encoding='utf-8'):
    p = l.rstrip('\n').split('\t')
    if p[0] != 'func': attr[int(p[0])] = p[1]
APP = {'vota', 'comum', 'api', 'simulador', 'ecourna', 'vota-web'}
users = collections.defaultdict(set)
for l in open(refs_tsv, encoding='utf-8'):
    p = l.rstrip('\n').split('\t', 3)
    if p[0] != 'func' and p[2] == 'str': users[int(p[1])].add(int(p[0]))
SKIP = re.compile(r'^(/|[a-z]+/.*\.(c|cpp|h))|::.*\(|^[{}:<>0-9.,|\-\s]*$|^%|^[A-Za-z_][A-Za-z0-9_]*$')
rows = []
for l in open(strings_tsv, encoding='utf-8'):
    a, n, enc, t = l.rstrip('\n').split('\t', 3)
    if a == 'addr': continue
    a = int(a)
    comps = collections.Counter(attr.get(f, '-') for f in users.get(a, ()))
    app_user = any(c in APP for c in comps)
    if SKIP.match(t): continue
    if enc in ('latin1', 'utf8') or (app_user and ' ' in t):
        rows.append((a, enc, '|'.join(f'{c}:{k}' for c, k in comps.most_common(3)) or '-', t))
with open(out, 'w', encoding='utf-8') as o:
    o.write('addr\tenc\tusers\ttext\n')
    for r in rows: o.write('\t'.join(map(str, r)) + '\n')
print('ui strings', len(rows), 'latin1', sum(1 for r in rows if r[1] == 'latin1'), 'with an app user', sum(1 for r in rows if r[2] != '-'))
