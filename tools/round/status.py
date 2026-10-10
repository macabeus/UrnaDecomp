"""Write the progress numbers into README.md and docs/status.md from the committed scan.

    python3 -I -B tools/round/status.py           # rewrite the generated blocks (make -C src status)
    python3 -I -B tools/round/status.py --check   # exit 1 when a block or the scan disagrees with the claims (CI)

Inputs, all tracked: splat/matched.txt (the claims), splat/scan.tsv (the verdict of every target function, written by
`make -C src splat-scan` and `splat-claim`) and target/vota_web_wasm.wasm (for byte-identical twins). The scan's
`match` rows are exactly the claims when splat-claim wrote it, so its sums are the splat build's meter.

Each generated block sits between `<!-- status:NAME -->` and `<!-- /status:NAME -->`; text outside the markers is
hand-written and kept.
"""
import collections
import hashlib
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
CLAIMS = os.path.join(ROOT, 'splat', 'matched.txt')
SCAN = os.path.join(ROOT, 'splat', 'scan.tsv')
TARGET = os.path.join(ROOT, 'target', 'vota_web_wasm.wasm')
README = os.path.join(ROOT, 'README.md')
STATUS = os.path.join(ROOT, 'docs', 'status.md')

UENUX2 = [
    ('vota', 'uenux2 `src/app/vota`: the voting application'),
    ('comum', 'uenux2 `src/app/comum`: shared urna code (domain model, converters, reports)'),
    ('api', 'uenux2 `src/api`: GUI toolkit, io, ipc, util, logging, audio'),
    ('simulador', "the web simulator's `CWasm*` devices"),
    ('vota-web', 'the web engine (`CVotaWebEngine`, …)'),
]
TSE_OTHER = [
    ('ecourna', 'the ecourna library: security, SQLite wrapper, compression, `app/dados`'),
    ('asn1-modules', 'the generated `Modulo*` ASN.1 types'),
    ('ASN1', "TSE's fork of the III ASN.1 runtime"),
]
TSE = [c for c, _ in UENUX2 + TSE_OTHER]
NOT_ATTRIBUTED = ['unknown', 'app']
VERDICTS = [
    ('match', 'claimed'),
    ('differs', 'our link has a candidate body and it differs; `detail` names the first difference. '
                'These are the closest to a claim.'),
    ('unresolved', 'an operand of our body has no counterpart in the target: a callee the symbol file maps to no '
                   'target function (such as our roots stand-in or a `byn$mgfn-shared$` thunk), or a data name only '
                   'this function voted for (*self-voted*)'),
    ('no-candidate', 'our link has no body for it yet'),
]
TIERS = ['byte', 'fingerprint', 'addr', 'shape', 'none']


def n(x):
    return f'{x:,}'


def pct(a, b):
    return f'{100 * a / b:.2f}%' if b else '0.00%'


def read_claims():
    out = []
    with open(CLAIMS, encoding='utf-8') as f:
        for line in f:
            line = line.split('#', 1)[0].strip()
            if line:
                out.append(int(line.split()[0]))
    return out


def read_scan():
    with open(SCAN, encoding='utf-8') as f:
        header = f.readline().rstrip('\n').split('\t')
        rows = [dict(zip(header, line.rstrip('\n').split('\t'))) for line in f if line.strip()]
    for r in rows:
        r['index'] = int(r['index'])
        r['size'] = int(r['size'])
    return rows


def leb(b, i):
    v = s = 0
    while True:
        x = b[i]
        i += 1
        v |= (x & 0x7F) << s
        s += 7
        if x < 0x80:
            return v, i


def target_bodies():
    """function index → body bytes (after the size LEB), from the import and code sections."""
    b = open(TARGET, 'rb').read()
    i, imported, bodies = 8, 0, {}
    while i < len(b):
        sid = b[i]
        size, i = leb(b, i + 1)
        end = i + size
        if sid == 2:
            count, j = leb(b, i)
            for _ in range(count):
                for _ in range(2):
                    ln, j = leb(b, j)
                    j += ln
                kind = b[j]
                j += 1
                if kind == 0:
                    imported += 1
                    _, j = leb(b, j)
                elif kind == 1:
                    j += 1
                    flag = b[j]
                    _, j = leb(b, j + 1)
                    if flag & 1:
                        _, j = leb(b, j)
                elif kind == 2:
                    flag = b[j]
                    _, j = leb(b, j + 1)
                    if flag & 1:
                        _, j = leb(b, j)
                elif kind == 3:
                    j += 2
                elif kind == 4:
                    _, j = leb(b, j + 1)
                else:
                    raise ValueError(f'import kind {kind}')
        elif sid == 10:
            count, j = leb(b, i)
            for k in range(count):
                ln, j = leb(b, j)
                bodies[imported + k] = b[j:j + ln]
                j += ln
        i = end
    return bodies


def object_origin(obj):
    """Where our candidate's code comes from, read from the object the link map names."""
    if obj == '-':
        return 'no candidate in our link'
    m = re.match(r'sysroot:([^(]+)', obj)
    if m:
        lib = m.group(1)
        return {
            'libc.a': 'emscripten sysroot: libc (musl)',
            'libc_optz.a': 'emscripten sysroot: libc (musl)',
            'libdlmalloc.a': 'emscripten sysroot: dlmalloc',
            'libcompiler_rt.a': 'emscripten sysroot: compiler-rt',
            'libc++.a': 'emscripten sysroot: libc++',
            'libc++-noexcept.a': 'emscripten sysroot: libc++',
            'libc++abi.a': 'emscripten sysroot: libc++abi',
            'libc++abi-noexcept.a': 'emscripten sysroot: libc++abi',
        }.get(lib, f'emscripten sysroot: {lib}')
    if obj == '<binaryen>':
        return 'binaryen (functions the optimizer creates)'
    for prefix, origin in [('openssl/', 'OpenSSL'), ('sqlite/', 'SQLite'), ('7zip/', '7-Zip'), ('rhvoice/', 'RHVoice'),
                           ('zlib/', 'zlib'), ('app/ecourna/', 'our ecourna units'), ('app/uenux2/', 'our uenux2 units'),
                           ('app/asn1/', 'our ASN.1 runtime and modules'), ('app/', 'our stand-ins (src/app, src/web)')]:
        if obj.startswith(prefix):
            return origin
    return obj


def tally(rows):
    t = collections.defaultdict(lambda: [0, 0, 0, 0])
    for r in rows:
        c = t[r['group']]
        c[1] += 1
        c[3] += r['size']
        if r['verdict'] == 'match':
            c[0] += 1
            c[2] += r['size']
    return t


def total(t, groups):
    s = [0, 0, 0, 0]
    for g in groups:
        for k in range(4):
            s[k] += t[g][k]
    return s


def blocks(rows, claims):
    t = tally(rows)
    libs = sorted((g for g in t if g not in TSE and g not in NOT_ATTRIBUTED), key=lambda g: (-t[g][0], g))
    every = total(t, list(t))
    tse, lib, na, ue = total(t, TSE), total(t, libs), total(t, NOT_ATTRIBUTED), total(t, [c for c, _ in UENUX2])
    code = lambda xs: ', '.join(f'`{x}`' for x in xs)
    out = {}

    out['readme'] = '\n'.join([
        f'**{n(every[0])} of {n(every[1])} target functions claimed ({pct(every[0], every[1])}), '
        f'{pct(every[2], every[3])} of the code bytes.** Most of these claims are upstream libraries rebuilt from '
        f"their own sources. TSE's own code, which this project exists to recover, is {n(tse[0])} of {n(tse[1])} "
        f'functions claimed: {pct(tse[2], tse[3])} of its code bytes.',
        '',
        '| origin | components | functions claimed | code bytes claimed |',
        '| --- | --- | ---: | ---: |',
        f'| TSE code: uenux2 | {code(c for c, _ in UENUX2)} | {n(ue[0])} / {n(ue[1])} | {pct(ue[2], ue[3])} |',
        f'| TSE code: ecourna | `ecourna` | {n(t["ecourna"][0])} / {n(t["ecourna"][1])} | '
        f'{pct(t["ecourna"][2], t["ecourna"][3])} |',
        f"| TSE code: ASN.1 types | `asn1-modules` (generated from TSE's schemas) | {n(t['asn1-modules'][0])} / "
        f"{n(t['asn1-modules'][1])} | {pct(t['asn1-modules'][2], t['asn1-modules'][3])} |",
        f"| TSE code: ASN.1 runtime | `ASN1` (TSE's fork of the III ASN.1 runtime) | {n(t['ASN1'][0])} / "
        f"{n(t['ASN1'][1])} | {pct(t['ASN1'][2], t['ASN1'][3])} |",
        f'| **TSE code** | | **{n(tse[0])} / {n(tse[1])}** | **{pct(tse[2], tse[3])}** |',
        f'| upstream libraries | {code(libs)} | {n(lib[0])} / {n(lib[1])} | {pct(lib[2], lib[3])} |',
        f'| not attributed | `app`, `unknown` | {n(na[0])} / {n(na[1])} | {pct(na[2], na[3])} |',
    ])

    out['headline'] = (f'**{n(every[0])} of {n(every[1])} functions claimed ({pct(every[0], every[1])}), '
                       f'{n(every[2])} of {n(every[3])} code bytes ({pct(every[2], every[3])}).**')

    def cells(c, bold=False):
        b = '**' if bold else ''
        return ' | '.join(f'{b}{x}{b}' for x in (f'{n(c[0])} / {n(c[1])}', pct(c[0], c[1]), f'{n(c[2])} / {n(c[3])}',
                                                   pct(c[2], c[3])))

    out['origin'] = '\n'.join([
        '| origin | components | functions | | code bytes | |',
        '| --- | --- | ---: | ---: | ---: | ---: |',
        f'| TSE code | {code(TSE)} | {cells(tse)} |',
        f'| upstream libraries | {code(libs)} | {cells(lib)} |',
        f'| not attributed | {code(NOT_ATTRIBUTED)} | {cells(na)} |',
        f'| **total** | | {cells(every, bold=True)} |',
    ])
    out['tse'] = '\n'.join(
        ['| component | what it is | functions | | code bytes | |', '| --- | --- | ---: | ---: | ---: | ---: |']
        + [f'| `{c}` | {w} | {cells(t[c])} |' for c, w in UENUX2]
        + [f'| **uenux2** | the five rows above | {cells(ue, bold=True)} |']
        + [f'| `{c}` | {w} | {cells(t[c])} |' for c, w in TSE_OTHER])
    head = ['| component | functions | | code bytes | |', '| --- | ---: | ---: | ---: | ---: |']
    out['libraries'] = '\n'.join(head + [f'| `{g}` | {cells(t[g])} |' for g in libs])
    out['unattributed'] = '\n'.join(head + [f'| `{g}` | {cells(t[g])} |' for g in NOT_ATTRIBUTED])

    by = collections.defaultdict(lambda: {g: [0, 0] for g in NOT_ATTRIBUTED})
    for r in rows:
        if r['group'] in NOT_ATTRIBUTED:
            c = by[object_origin(r.get('object', '-'))][r['group']]
            c[1] += 1
            c[0] += r['verdict'] == 'match'
    order = sorted(by, key=lambda o: (o == 'no candidate in our link', -sum(v[1] for v in by[o].values()), o))
    out['unattributed-origin'] = '\n'.join(
        ['| our candidate is compiled from | `unknown` claimed / total | `app` claimed / total |', '| --- | ---: | ---: |']
        + [f'| {o} | {n(by[o]["unknown"][0])} / {n(by[o]["unknown"][1])} | {n(by[o]["app"][0])} / {n(by[o]["app"][1])} |'
           for o in order])

    v = collections.Counter(r['verdict'] for r in rows)
    vb = collections.Counter()
    for r in rows:
        vb[r['verdict']] += r['size']
    out['verdicts'] = '\n'.join(['| verdict | functions | code bytes | what it means |', '| --- | ---: | ---: | --- |']
                                + [f'| `{k}` | {n(v[k])} | {n(vb[k])} | {w} |' for k, w in VERDICTS])

    tse_v = collections.Counter(r['verdict'] for r in rows if r['group'] in TSE)
    out['tse-verdicts'] = (f'For TSE code alone: {n(tse_v["match"])} `match`, {n(tse_v["differs"])} `differs`, '
                           f'{n(tse_v["unresolved"])} `unresolved` and {n(tse_v["no-candidate"])} `no-candidate`.')

    bodies = target_bodies()
    seen = collections.Counter(hashlib.sha256(b).digest() for b in bodies.values())
    claimed = set(claims)
    small = sum(1 for r in rows if r['index'] in claimed and r['size'] <= 8)
    twins = sum(1 for i in claimed if seen[hashlib.sha256(bodies[i]).digest()] > 1)
    out['twins'] = (f'{n(small)} claims are bodies of 8 B or less, and {n(twins)} claims have a byte-identical twin in '
                    'the target.')

    tiers = collections.defaultdict(collections.Counter)
    for r in rows:
        tiers[r['ledger_kind']][r['verdict']] += 1
    out['tiers'] = '\n'.join(
        ['| tier | functions | match | differs | unresolved | no-candidate |', '| --- | ---: | ---: | ---: | ---: | ---: |']
        + [f'| `{k}` | {n(sum(tiers[k].values()))} | ' + ' | '.join(n(tiers[k][x]) for x, _ in VERDICTS) + ' |'
           for k in TIERS])
    return out


def substitute(path, generated):
    text = open(path, encoding='utf-8').read()
    names = re.findall(r'<!-- status:([a-z-]+) -->', text)
    if not names:
        raise SystemExit(f'{path}: no <!-- status:NAME --> block')
    new = text
    for name in names:
        if name not in generated:
            raise SystemExit(f'{path}: unknown block status:{name}')
        new = re.sub(rf'(<!-- status:{name} -->\n).*?(<!-- /status:{name} -->)',
                     lambda m: f'{m.group(1)}{generated[name]}\n{m.group(2)}', new, flags=re.S)
    return text, new


def main(argv):
    check = argv[1:] == ['--check']
    if argv[1:] not in ([], ['--check']):
        raise SystemExit(__doc__)
    claims = read_claims()
    rows = read_scan()
    problems = []
    matches = sorted(r['index'] for r in rows if r['verdict'] == 'match')
    if matches != sorted(claims):
        extra = sorted(set(matches) - set(claims))[:5]
        missing = sorted(set(claims) - set(matches))[:5]
        problems.append(f'splat/scan.tsv has {len(matches)} match rows but splat/matched.txt has {len(claims)} claims '
                        f'(scan only: {extra}, claims only: {missing}); run TMPDIR=/tmp make -C src splat-claim')
    generated = blocks(rows, claims)
    for path, (old, new) in [(p, substitute(p, generated)) for p in (README, STATUS)]:
        if old != new:
            if check:
                problems.append(f'{os.path.relpath(path, ROOT)} is not what the scan gives; run make -C src status')
            else:
                with open(path, 'w', encoding='utf-8') as f:
                    f.write(new)
                print(f'wrote {os.path.relpath(path, ROOT)}')
    for p in problems:
        print(p, file=sys.stderr)
    if not problems:
        print(f'{len(claims)} claims; README.md and docs/status.md agree with splat/scan.tsv')
    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
