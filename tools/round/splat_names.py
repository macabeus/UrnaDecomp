"""Names for every function of the plain link, from a scratch relink's kept intermediates (tools/round/link-map.sh).

  splat_names.py replay-script <out> <tmp>          print the docker replay script (binaryen stages 2-4 again)
  splat_names.py compose <out> <tmp> <plain.wasm>   write <out>/names.tsv: final index -> wasm-ld name, object

The plain pipeline loses every name: emcc strips the name section before `wasm-opt -Oz` (emcc-02-strip), and binaryen
names an unnamed function by its defined index
(emcc-02 is saved before that strip and still has names). So the chain is rebuilt stage by stage:
  wasm-ld defined index -> name, object      emcc-02's name section; the CODE section of wasm-ld's --Map
  post-Oz defined index -> wasm-ld index     `wasm-opt -Oz` replayed on emcc-02 stripped of its names, with
                                             --symbolmap (output == emcc-05)
  final defined index   -> post-Oz index     metadce and the final wasm-opt replayed with -g on emcc-08 carrying
                                             "k" names (code == the plain module's)
"""
import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'extract'))
from wasmmod import Module, read_uleb  # noqa: E402

BIN = '/emsdk/upstream/bin'


def uleb(v):
    out = bytearray()
    while True:
        c = v & 0x7f
        v >>= 7
        if v:
            out.append(c | 0x80)
        else:
            out.append(c)
            return bytes(out)


def strip_custom(m, drop):
    """The module without the custom sections whose name `drop` accepts (what emcc's llvm-objcopy step removes)."""
    b = m.bytes
    out = bytearray(b[:8])
    for sid, a, e in m.sections:
        if sid == 0:
            n, i = read_uleb(b, a)
            if drop(b[i:i + n].decode()):
                continue
        out += bytes([sid]) + uleb(e - a) + b[a:e]
    return bytes(out)


def objcopy_dropped(name):
    return name in ('name', 'producers') or name.startswith('.debug') or name.startswith('llvm.func_attr.annotate.')


def stage_commands(log_path):
    """The three binaryen invocations emcc ran, as argv lists (in order: -Oz, metadce, final)."""
    found = []
    for line in open(log_path, encoding='utf-8', errors='replace'):
        m = re.match(r'utils:DEBUG: successfully executed (/emsdk/upstream/bin/(wasm-opt|wasm-metadce) .*)$', line.rstrip('\n'))
        if m and '--version' not in m.group(1):
            found.append(m.group(1).split(' '))
    if len(found) != 3 or '-Oz' not in found[0] or 'wasm-metadce' not in found[1][0] or '--minify-imports-and-exports-and-modules' not in found[2]:
        raise SystemExit(f'{log_path}: expected wasm-opt -Oz, wasm-metadce, wasm-opt --minify…; found {[c[:2] for c in found]}')
    return found


def retarget(argv, src, dst):
    """argv with its `<in> -o <out>` pair replaced (emcc runs every stage in place on the output module)."""
    k = argv.index('-o')
    return argv[:k - 1] + [src, '-o', dst] + argv[k + 2:]


def name_section(names):
    """A `name` custom section naming function index i by names[i]."""
    sub = bytearray(uleb(len(names)))
    for i, n in sorted(names.items()):
        s = n.encode()
        sub += uleb(i) + uleb(len(s)) + s
    payload = uleb(4) + b'name' + bytes([1]) + uleb(len(sub)) + sub
    return bytes([0]) + uleb(len(payload)) + payload


def read_function_names(m):
    """Function index -> name from a module's name section (subsection 1)."""
    b = m.bytes
    for sid, a, e in m.sections:
        if sid != 0:
            continue
        n, i = read_uleb(b, a)
        if b[i:i + n] != b'name':
            continue
        i += n
        while i < e:
            kind = b[i]
            i += 1
            size, i = read_uleb(b, i)
            end = i + size
            if kind == 1:
                out = {}
                cnt, i = read_uleb(b, i)
                for _ in range(cnt):
                    idx, i = read_uleb(b, i)
                    ln, i = read_uleb(b, i)
                    out[idx] = b[i:i + ln].decode()
                    i += ln
                return out
            i = end
    raise SystemExit('no function names in the replayed module')


def read_symbolmap(path):
    out = {}
    for line in open(path):
        i, n = line.rstrip('\n').split(':', 1)
        out[int(i)] = n
    return out


def map_code_entries(map_path):
    """wasm-ld --Map CODE section: (name, object) per defined function, in code order."""
    rx = re.compile(r'^\s*(\S+)\s+([0-9a-f]+)\s+([0-9a-f]+)( +)(.*)$')
    sec = None
    out = []
    for line in open(map_path, encoding='utf-8'):
        m = rx.match(line.rstrip('\n'))
        if not m:
            continue
        depth = len(m.group(4))
        if depth == 1:
            sec = m.group(5).split()[0]
        elif depth == 9 and sec == 'CODE':
            entry = m.group(5)
            mm = re.match(r'^(.*):\((.*)\)$', entry)
            if not mm:
                raise SystemExit(f'{map_path}: unreadable CODE entry {entry!r}')
            out.append((mm.group(2), mm.group(1)))
    return out


def replay_script(out, tmp):
    oz, metadce, final = stage_commands(f'{out}/link.log')
    e = f'{tmp}/emscripten_temp'
    pre = Module(f'{e}/emcc-08-preclean.wasm')
    if pre.bytes != open(f'{e}/emcc-05-wasm-opt.wasm', 'rb').read():
        raise SystemExit('emcc-08-preclean differs from emcc-05-wasm-opt: the replay chain assumes no change between them')
    # emcc saves emcc-02 BEFORE its llvm-objcopy strips the name section; the -Oz input is the stripped module
    with open(f'{out}/r02-stripped.wasm', 'wb') as f:
        f.write(strip_custom(Module(f'{e}/emcc-02-strip.wasm'), objcopy_dropped))
    imported = pre.n_imported_funcs
    names = {imported + k: str(k) for k in range(len(pre.func_types))}
    with open(f'{out}/r08-named.wasm', 'wb') as f:
        f.write(pre.bytes + name_section(names))
    rel = lambda p: os.path.relpath(p, 'build/out')  # noqa: E731 - docker cwd is /work/build/out
    lines = [
        'set -euo pipefail',
        'cd build/out',
        ' '.join(retarget(oz, rel(f'{out}/r02-stripped.wasm'), rel(f'{out}/r05.wasm')) + [f'--symbolmap={rel(out)}/r05.symbols']),
        f'cmp {rel(out)}/r05.wasm {rel(e)}/emcc-05-wasm-opt.wasm',
        'echo "replayed -Oz == emcc-05"',
        ' '.join(retarget(metadce, rel(f'{out}/r08-named.wasm'), rel(f'{out}/r10-named.wasm')) + ['-g']),
        ' '.join(retarget(final, rel(f'{out}/r10-named.wasm'), rel(f'{out}/r13-named.wasm')) + ['-g']),
    ]
    print('\n'.join(lines))


def compose(out, tmp, plain_path):
    e = f'{tmp}/emscripten_temp'
    plain = Module(plain_path)
    r13 = Module(f'{out}/r13-named.wasm')
    if len(plain.code) != len(r13.code) or any(
        plain.bytes[s:t] != r13.bytes[s2:t2] for (_, s, t), (_, s2, t2) in zip(plain.code, r13.code)
    ):
        raise SystemExit('the -g replay of metadce + final wasm-opt does not reproduce the plain code section')
    final_to_oz = {i: int(n) for i, n in read_function_names(r13).items() if i >= r13.n_imported_funcs}
    r05 = Module(f'{out}/r05.wasm')
    sym05 = read_symbolmap(f'{out}/r05.symbols')
    oz_to_ld = {i - r05.n_imported_funcs: n for i, n in sym05.items() if i >= r05.n_imported_funcs}
    base = Module(f'{e}/emcc-02-strip.wasm')
    ld_names = read_function_names(base)
    objects = map_code_entries(f'{out}/vota_web_wasm.map')
    if len(objects) != len(base.func_types):
        raise SystemExit(f'--Map lists {len(objects)} functions, wasm-ld wrote {len(base.func_types)}')
    # the name section's spelling (the map calls a signature_mismatch:f stub plain f), the map's object
    entries = [(ld_names[base.n_imported_funcs + k], obj) for k, (_, obj) in enumerate(objects)]
    rows = []
    for fi, _, _ in plain.code:
        oz = final_to_oz[fi]
        ld = oz_to_ld[oz]
        if ld.isdigit():
            name, obj = entries[int(ld)]
            rows.append((fi, name, obj, ld, oz))
        else:
            # merge-similar-functions names its shared body after the first merged function's (default) name
            m = re.match(r'^(byn\$mgfn-shared\$)(\d+)$', ld)
            rows.append((fi, m.group(1) + entries[int(m.group(2))][0] if m else ld, '<binaryen>', '-', oz))
    with open(f'{out}/names.tsv', 'w') as f:
        f.write('index\tname\tobject\tld_index\tpost_oz_index\n')
        for r in rows:
            f.write('\t'.join(map(str, r)) + '\n')
    binaryen = sum(1 for r in rows if r[2] == '<binaryen>')
    print(f'{out}/names.tsv: {len(rows)} functions named ({binaryen} binaryen thunks), {len(entries)} wasm-ld functions')


if __name__ == '__main__':
    cmd = sys.argv[1] if len(sys.argv) > 1 else ''
    if cmd == 'replay-script' and len(sys.argv) == 4:
        replay_script(sys.argv[2], sys.argv[3])
    elif cmd == 'compose' and len(sys.argv) == 5:
        compose(sys.argv[2], sys.argv[3], sys.argv[4])
    else:
        raise SystemExit(__doc__)
