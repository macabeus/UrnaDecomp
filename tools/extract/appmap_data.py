"""Summaries for docs/reference/appmap.md: source tree, subsystems (class hierarchies), per-file function counts.
usage: python3 -I tools/extract/appmap_data.py <ledger dir> > <out.md>"""
import sys, json, re, collections, os
L = sys.argv[1]
files = [l.rstrip('\n') for l in open(f'{L}/source-files.txt') if l.strip()]
classes = json.load(open(f'{L}/classes.json'))['classes']
vt = json.load(open(f'{L}/vtables.json'))
attr = [l.rstrip('\n').split('\t') for l in open(f'{L}/func-attribution.tsv', encoding='utf-8')][1:]

# --- source tree ----------------------------------------------------------------
print('## Source tree (from __FILE__ strings; count = functions whose code loads that path)')
fcount = collections.Counter()
for fi, comp, ev, det in attr:
    if ev == 'file':
        for p in det.split(';'): fcount[p] += 1
tree = {}
for p in files:
    node = tree
    for part in p.strip('/').split('/'): node = node.setdefault(part, {})
def walk(node, prefix='', depth=0, path=''):
    for k in sorted(node):
        full = path + '/' + k
        kids = node[k]
        # collapse single-child chains
        chain = [k]
        while len(kids) == 1 and not re.search(r'\.(c|cpp|h|hpp|cc|cxx|inl)$', next(iter(kids))):
            k2 = next(iter(kids)); chain.append(k2); kids = kids[k2]; full += '/' + k2
        label = '/'.join(chain)
        if kids:
            n = sum(1 for f in files if f.startswith(full.lstrip('/') if not full.startswith('/home') and not full.startswith('/emsdk') else full))
            print(f'{"  " * depth}- {label}/  ({n} files)')
            if depth < 6: walk(kids, prefix, depth + 1, full)
        else:
            key = full if full.startswith(('/home', '/emsdk')) else full.lstrip('/')
            c = fcount.get(key, 0)
            print(f'{"  " * depth}- {label}' + (f'  [{c} fn]' if c else ''))
# print uenux2 + ecourna fully, third-party collapsed to directories
def sub(root):
    node = tree
    for part in root.strip('/').split('/'): node = node[part]
    return node
for root in ['/home/rubio/tse/uenux2/src', '/home/rubio/tse/uenux2/wasm', '/home/rubio/.conan2/p/b/libecea1da310e5107/b/src/ecourna']:
    print(f'\n### {root}'); walk(sub(root), path=root)
print('\n### OpenSSL (relative paths, ERR file strings) — files per directory')
od = collections.Counter('/'.join(f.split('/')[:2]) for f in files if f.startswith(('crypto/', 'providers/', 'ssl/')))
print(', '.join(f'{d} ({n})' for d, n in sorted(od.items())))
print('\n### emscripten system libs'); print('\n'.join(f'- {f}' for f in files if f.startswith('/emsdk')))

# --- subsystems -------------------------------------------------------------------
byname = {c['name']: c for c in classes}
children = collections.defaultdict(list)
for c in classes:
    for b in c.get('bases', []):
        if b.get('name'): children[b['name']].append(c['name'])
def subtree(root, depth=0, seen=None, maxd=4):
    seen = seen or set()
    out = []
    for ch in sorted(children.get(root, [])):
        if ch in seen: continue
        seen.add(ch); out.append('  ' * depth + '- ' + ch)
        if depth < maxd: out += subtree(ch, depth + 1, seen, maxd)
    return out
vslots = collections.defaultdict(int)
for v in vt: vslots[v['class']] = max(vslots[v['class']], len(v['functions']))
print('\n## Subsystems (RTTI inheritance; number = virtual slots in the primary vtable)')
for root, title in [('comum::CAppState', 'Voting-flow state machine: states derived from comum::CAppState'),
                    ('api::IScreen', 'Screen abstraction api::IScreen'),
                    ('api::CFormBuilder', 'GUI form builders api::CFormBuilder'),
                    ('api::IFormBuilder', 'GUI form builders api::IFormBuilder'),
                    ('api::ITextToSpeech', 'Text to speech'),
                    ('ecourna::api::security::ISymmetricCipher', 'Security: symmetric ciphers'),
                    ('ecourna::api::sql::sqlite::CSqlConnection', 'SQLite DAO'),
                    ('ASN1::AbstractData', 'ASN.1 runtime data model (asnparser)'),
                    ('api::IInput', 'Input devices'), ('api::ISound', 'Sound'), ('api::ITimer', 'Timers'),
                    ('comum::IDao', 'DAO interfaces'), ('comum::CDaoBase', 'DAO base')]:
    if root in byname or children.get(root):
        print(f'\n### {title}  (`{root}`, {vslots.get(root, 0)} slots, {len(children.get(root, []))} direct subclasses)')
        print('\n'.join(subtree(root)[:120]))
# generic: top-level roots with many descendants in app namespaces
def ndesc(r, seen=None):
    seen = seen or set(); n = 0
    for ch in children.get(r, []):
        if ch not in seen: seen.add(ch); n += 1 + ndesc(ch, seen)
    return n
roots = [c['name'] for c in classes if c['kind'].endswith('class') and not c.get('bases') and c['name'].split('::')[0] in ('vota', 'comum', 'api', 'ecourna', 'simulador')]
big = sorted(((ndesc(r), r) for r in roots), reverse=True)[:40]
print('\n### Largest app class hierarchies (root → descendants)'); print('\n'.join(f'- {r}: {n}' for n, r in big if n))
# app classes by namespace (second level)
print('\n### App classes per namespace (RTTI count)')
ns = collections.Counter('::'.join(c['name'].split('::')[:2]) if c['name'].count('::') >= 2 else c['name'].split('::')[0] for c in classes if c['kind'].endswith('class') and c['name'].split('::')[0] in ('vota', 'comum', 'api', 'ecourna', 'simulador'))
print(', '.join(f'{k} ({v})' for k, v in sorted(ns.items())))
print('\n### vota:: classes'); print(', '.join(sorted(c['name'][6:] for c in classes if c['name'].startswith('vota::') and c['kind'].endswith('class'))))
print('\n### comum:: classes'); print(', '.join(sorted(c['name'][7:] for c in classes if c['name'].startswith('comum::') and c['kind'].endswith('class'))))
print('\n### api:: classes'); print(', '.join(sorted(c['name'][5:] for c in classes if c['name'].startswith('api::') and c['kind'].endswith('class'))))
print('\n### ecourna:: classes'); print(', '.join(sorted(c['name'][9:] for c in classes if c['name'].startswith('ecourna::') and c['kind'].endswith('class'))))
print('\n### simulador:: classes'); print(', '.join(sorted(c['name'][11:] for c in classes if c['name'].startswith('simulador::') and c['kind'].endswith('class'))))
