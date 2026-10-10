"""Structural comparison of the skeleton link against the target, as Markdown on stdout.
usage: python3 -I tools/round/census_report.py <target.wasm> <skeleton.wasm>   (the glue x.js is read beside each module)
Sections: module anatomy (sections, memory, globals, table, data), import set diff (real names from the glues'
wasmImports tables, with wasm types), export order (letter -> real name), the EM_JS text of the glue, the glue byte diff
(common prefix/suffix, first differing offset) and the same-named top-level glue definitions that differ.
"""
import json, re, subprocess, sys

tw, sw = sys.argv[1], sys.argv[2]
tj, sj = tw[:-5] + '.js', sw[:-5] + '.js'

def objdump(path, section):
    return subprocess.run(['wasm-objdump', '-x', '-j', section, path], capture_output=True, text=True).stdout

def headers(path):
    out = subprocess.run(['wasm-objdump', '-h', path], capture_output=True, text=True).stdout
    h = {}
    for l in out.splitlines():
        m = re.match(r'\s*(\w+) start=0x([0-9a-f]+) end=0x([0-9a-f]+) \(size=0x([0-9a-f]+)\) count: (\d+)', l)
        if m:
            h[m.group(1)] = (int(m.group(4), 16), int(m.group(5)))
    return h

def types(path):
    t = {}
    for l in objdump(path, 'Type').splitlines():
        m = re.match(r'\s*- type\[(\d+)\] \((.*?)\) -> (.*)$', l)
        if m:
            t[int(m.group(1))] = f"({m.group(2)})->{m.group(3).strip()}"
    return t

def imports(path, glue):
    names = {}
    m = re.search(r'var wasmImports=\{([^}]*)\}', open(glue, encoding='utf-8').read())
    for ent in m.group(1).split(','):
        k, v = ent.split(':', 1)
        v = v.strip()
        names[k.strip()] = v[1:] if v.startswith('_') else v
    t = types(path)
    out = {}
    for l in objdump(path, 'Import').splitlines():
        m = re.match(r'\s*- func\[(\d+)\] sig=(\d+) <a\.(\S+)>', l)
        if m:
            out[names[m.group(3)]] = (int(m.group(1)), t[int(m.group(2))])
    return out

def exports(path, glue):
    g = open(glue, encoding='utf-8').read()
    names = {}
    for m in re.finditer(r'Module\["(_\w+)"\]=wasmExports\["(\w+)"\]', g):
        names[m.group(2)] = m.group(1)[1:]
    for m in re.finditer(r'var (_\w+)=wasmExports\["(\w+)"\]', g):
        names.setdefault(m.group(2), m.group(1)[1:])
    for m in re.finditer(r'(_\w+)=wasmExports\["(\w+)"\]', g):
        names.setdefault(m.group(2), m.group(1)[1:])
    for m in re.finditer(r'wasmExports\["(\w+)"\]\(\)', g):
        names.setdefault(m.group(1), '__wasm_call_ctors')
    for m in re.finditer(r'wasmMemory=wasmExports\["(\w+)"\]', g):
        names[m.group(1)] = 'memory'
    for m in re.finditer(r'wasmTable=wasmExports\["(\w+)"\]', g):
        names[m.group(1)] = '__indirect_function_table'
    rows = []
    for l in objdump(path, 'Export').splitlines():
        m = re.match(r'\s*- (func|memory|table)\[(\d+)\].*-> "(\w+)"', l)
        if m:
            rows.append((m.group(3), m.group(1), int(m.group(2)), names.get(m.group(3), '?')))
    return rows

def mem(path):
    r = {}
    for l in objdump(path, 'Memory').splitlines():
        m = re.search(r'initial=(\d+) max=(\d+)', l)
        if m: r['pages'] = (int(m.group(1)), int(m.group(2)))
    g = []
    for l in objdump(path, 'Global').splitlines():
        m = re.search(r'init i32=(\d+)', l)
        if m: g.append(int(m.group(1)))
    r['globals'] = g
    for l in objdump(path, 'Elem').splitlines():
        m = re.search(r'count=(\d+) - init i32=(\d+)', l)
        if m: r['elem'] = (int(m.group(1)), int(m.group(2)))
    last = 0
    for l in objdump(path, 'Data').splitlines():
        m = re.search(r'segment\[\d+\] .*memory=0 size=(\d+) - init i32=(\d+)', l)
        if m: last = max(last, int(m.group(2)) + int(m.group(1)))
    r['data_end'] = last
    return r

print('## Module anatomy\n')
ht, hs = headers(tw), headers(sw)
mt, ms = mem(tw), mem(sw)
print('| | target | skeleton |\n|---|---|---|')
import os
print(f"| file size (B) | {os.path.getsize(tw):,} | {os.path.getsize(sw):,} |")
for sec in ['Type', 'Import', 'Function', 'Export', 'Elem', 'DataCount', 'Code', 'Data']:
    a, b = ht.get(sec, (0, 0)), hs.get(sec, (0, 0))
    print(f"| {sec}: count / section bytes | {a[1]:,} / {a[0]:,} | {b[1]:,} / {b[0]:,} |")
print(f"| memory pages initial / max | {mt['pages'][0]} / {mt['pages'][1]} | {ms['pages'][0]} / {ms['pages'][1]} |")
print(f"| __stack_pointer init (global 0) | {mt['globals'][0]:,} | {ms['globals'][0]:,} |")
print(f"| static end = sp − 8 MB stack | {mt['globals'][0]-8388608:,} | {ms['globals'][0]-8388608:,} |")
print(f"| last data byte | {mt['data_end']:,} | {ms['data_end']:,} |")
print(f"| table (elem) entries / base | {mt['elem'][0]:,} / {mt['elem'][1]} | {ms['elem'][0]:,} / {ms['elem'][1]} |")
print(f"| globals | {len(mt['globals'])} | {len(ms['globals'])} |")

print('\n## Imports\n')
it, is_ = imports(tw, tj), imports(sw, sj)
common = sorted(set(it) & set(is_))
only_t = sorted(set(it) - set(is_)); only_s = sorted(set(is_) - set(it))
type_diff = [n for n in common if it[n][1] != is_[n][1]]
print(f"target {len(it)} imports, skeleton {len(is_)}; common {len(common)} ({len(type_diff)} with a different type); "
      f"only in target {len(only_t)}; only in skeleton {len(only_s)}.\n")
if only_t: print('- only in target: ' + ', '.join(f"`{n}` {it[n][1]}" for n in only_t))
if only_s: print('- only in skeleton: ' + ', '.join(f"`{n}` {is_[n][1]}" for n in only_s))
if type_diff: print('- type differs: ' + ', '.join(f"`{n}` {it[n][1]} vs {is_[n][1]}" for n in type_diff))
same_order = [n for n in common if it[n][0] == is_[n][0]]
print(f"- import ORDER (wasm-ld symbol-table order): {len(same_order)} of {len(common)} common imports sit at the same index; "
      f"target order starts {', '.join(sorted(it, key=lambda n: it[n][0])[:8])}; skeleton starts {', '.join(sorted(is_, key=lambda n: is_[n][0])[:8])}")

print('\n## Exports (order = wasm-ld symbol-table order)\n')
et, es = exports(tw, tj), exports(sw, sj)
print('| # | target | skeleton |\n|---|---|---|')
for i in range(max(len(et), len(es))):
    a = et[i] if i < len(et) else ('', '', '', '')
    b = es[i] if i < len(es) else ('', '', '', '')
    mark = '' if a[0] == b[0] and a[3] == b[3] else ' **≠**'
    print(f"| {i} | {a[0]} {a[3]} | {b[0]} {b[3]}{mark} |")

print('\n## Glue (vota_web_wasm.js)\n')
T = open(tj, 'rb').read(); Sg = open(sj, 'rb').read()
p = 0
while p < min(len(T), len(Sg)) and T[p] == Sg[p]: p += 1
q = 0
while q < min(len(T), len(Sg)) - p and T[-1-q] == Sg[-1-q]: q += 1
print(f"target {len(T):,} B, skeleton {len(Sg):,} B; common prefix {p:,} B, common suffix {q:,} B; first differing offset {p}:\n")
print('```\ntarget:   ' + T[max(0,p-60):p+100].decode('utf-8','replace').replace('\n',' ') + '\nskeleton: ' + Sg[max(0,p-60):p+100].decode('utf-8','replace').replace('\n',' ') + '\n```\n')
def em_js(g):
    m = re.search(r'Module\["FS_createLazyFile"\]=FS_createLazyFile;(function (?:js_|wasm_input_).*?)var _\w+', g, re.S)
    return m.group(1) if m else None
a, b = em_js(T.decode('utf-8')), em_js(Sg.decode('utf-8'))
print(f"EM_JS block: target {len(a) if a else 0:,} chars, skeleton {len(b) if b else 0:,} chars, {'IDENTICAL' if a == b else 'DIFFERENT'}"
      + ('' if a == b or not (a and b) else f" (first difference at char {next(i for i in range(min(len(a),len(b))) if a[i]!=b[i])})") + '.\n')
def defs(g):
    out = {}
    pos = [(m.start(), m.group(1) or m.group(2)) for m in re.finditer(r'(?:^|[;}])(?:function\s+(\w+)\s*\(|var\s+(\w+)\s*=)', g)]
    for i, (s, name) in enumerate(pos):
        e = pos[i+1][0] if i + 1 < len(pos) else len(g)
        out.setdefault(name, g[s:e])
    return out
dt, ds = defs(T.decode('utf-8')), defs(Sg.decode('utf-8'))
same = [n for n in dt if n in ds and dt[n] == ds[n]]
diff = [n for n in dt if n in ds and dt[n] != ds[n]]
print(f"Top-level definitions (function NAME / var NAME): target {len(dt)}, skeleton {len(ds)}; same text {len(same)}, "
      f"different text {len(diff)}, only in target {len([n for n in dt if n not in ds])}, only in skeleton {len([n for n in ds if n not in dt])}.\n")
print('- different: ' + ', '.join(f'`{n}`' for n in diff))
print('- only in target: ' + ', '.join(f'`{n}`' for n in dt if n not in ds))
print('- only in skeleton: ' + ', '.join(f'`{n}`' for n in ds if n not in dt))
