"""Write tools/libs/openssl/census.json: the libcrypto block's match counts against the target from a probe dir
(census.json = depth 2, census-d0.json = depth 0, probe.names.tsv), per class and per tier, plus the commands.
usage: write-census-json.py <probe-dir> <variant> <roots.txt> <out.json>
"""
import json, sys, collections, subprocess
pdir, variant, roots_f, out = sys.argv[1:5]
urna = __file__.rsplit('/tools/libs/', 1)[0]
cls = {int(k): v for k, v in json.load(open(f'{urna}/tools/libs/cls2.json')).items()}
attr = {}
for l in open(f'{urna}/ledger/func-attribution.tsv'):
    p = (l.rstrip('\n').split('\t') + ['', ''])[:4]
    if p[0].isdigit(): attr[int(p[0])] = p[1:]
fj = {d['idx']: d for d in json.load(open(f'{urna}/tools/libs/funcs.json'))}
names = {}
for l in open(f'{pdir}/probe.names.tsv'):
    i, n, s = l.rstrip('\n').split('\t'); names[int(i)] = (n, int(s))
syms = {l.strip() for l in open(f'{urna}/build/out/openssl/{variant}/libcrypto-symbols.txt')}
res = {'variant': variant, 'probe': pdir, 'roots': roots_f, 'roots_count': sum(1 for l in open(roots_f) if l.strip()),
       'target_functions': 14343, 'target_openssl_class': sum(1 for v in cls.values() if v == 'openssl'),
       'target_openssl_file_evidence': sum(1 for f, a in attr.items() if cls.get(f) == 'openssl' and a[1] == 'file'),
       'probe_functions': len(names), 'probe_libcrypto_functions': sum(1 for n, s in names.values() if n.split('|')[0] in syms)}
for depth, f in (('depth2', 'census.json'), ('depth0', 'census-d0.json')):
    cen = json.load(open(f'{pdir}/{f}'))
    paired_a = {p['a']: p['by'] for p in cen['pairs']}; paired_b = {p['b'] for p in cen['pairs']}
    tiers = collections.Counter(paired_a[f] for f in paired_a if cls.get(f) == 'openssl')
    fe = [f for f, a in attr.items() if cls.get(f) == 'openssl' and a[1] == 'file']
    ossl = [f for f, c in cls.items() if c == 'openssl']
    res[depth] = {
        'all_pairs': len(cen['pairs']), 'tiers_all': dict(collections.Counter(p['by'] for p in cen['pairs'])),
        'target_openssl_paired': sum(tiers.values()), 'target_openssl_tiers': dict(tiers),
        'target_openssl_paired_address_blind': tiers['fingerprint'] + tiers['addr'],
        'target_openssl_file_evidence_paired': sum(1 for f in fe if f in paired_a),
        'target_openssl_instrs_paired': sum(fj[f]['ninstr'] for f in ossl if f in paired_a),
        'target_openssl_instrs_total': sum(fj[f]['ninstr'] for f in ossl),
        'probe_libcrypto_paired': sum(1 for i, (n, s) in names.items() if i in paired_b and n.split('|')[0] in syms),
        'probe_libcrypto_unpaired': sorted(n for i, (n, s) in names.items() if i not in paired_b and n.split('|')[0] in syms),
    }
res['byte_exact'] = 'not applicable: a separately linked probe never reproduces the target addresses; fingerprint = address-blind exact body + callees (depth 2) / body alone (depth 0); addr = address-blind exact body; shape = same opcode stream, a constant or callee differs'
res['commands'] = [
    'tools/libs/openssl/configure.sh O3   (URNA_OSSL_CFLAGS=-O3)', 'tools/libs/openssl/build.sh O3',
    f'ROOTS={roots_f} EXTRA_EXPORTS=tools/libs/openssl/roots/empty.txt tools/libs/openssl/census.sh O3 <tag>',
    '(cd asmlift-wasm && pnpm exec tsx src/cli.ts census target/vota_web_wasm.wasm <probe.wasm> [--depth 0] --json)']
json.dump(res, open(out, 'w'), indent=1)
print(json.dumps({k: v for k, v in res.items() if k not in ('commands', 'byte_exact')}, default=lambda o: '…')[:1500])
