"""Check that the std::source_location records our objects hold are records of the target.

    python3 -I -B tools/round/check_srcloc.py [--strict] [objects…]   # make -C src check-srcloc (after make -C src link)

TSE's exception constructors take a std::source_location, so every throw site of a decompiled function compiles to
a 16-byte record {file name, function name, line, column} in the object's read-only data. The target holds 1,939 of
them (ledger/source-locations.tsv). A record of ours that is not one of the target's, with the same file, function,
line and column, means the code that throws sits at another line or column than in TSE's file: a blank line was lost
or added above it, or a stub block took a line it should not have.

The check reads the records out of the objects the plain link was built from (the app objects in
build/out/wasm/vota_web/link.cmd, or the objects given), resolving the two string pointers and the code that loads
the record through the object's relocations. Each record the target does not have is printed with the functions
that load it and the target's line and column for the same file and function. It fails when such a record is loaded
by a matched function (its symbol is in splat/matched.txt): a match cannot hold with a moved record, so that is a
line a change broke. A record loaded only by unmatched functions is reconstruction not finished yet and is printed
as such; `--strict` fails on those too.

Reads build/out/ (make -C src link writes it), so it needs a link but not the image; CI has neither.
"""
import collections
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
LEDGER = os.path.join(ROOT, 'ledger', 'source-locations.tsv')
MATCHES = os.path.join(ROOT, 'splat', 'matched.txt')
LINK_CMD = os.path.join(ROOT, 'build', 'out', 'wasm', 'vota_web', 'link.cmd')

R_WASM_MEMORY_ADDR_I32 = 5
# relocation types that carry an addend (the wasm object-file convention, tool-conventions/Linking.md)
WITH_ADDEND = {3, 4, 5, 8, 9, 11, 14, 15, 16, 17, 21, 22, 23, 25}


def uleb(b, i):
    r = s = 0
    while True:
        c = b[i]
        i += 1
        r |= (c & 0x7f) << s
        s += 7
        if not c & 0x80:
            return r, i


def sleb(b, i):
    r = s = 0
    while True:
        c = b[i]
        i += 1
        r |= (c & 0x7f) << s
        s += 7
        if not c & 0x80:
            if c & 0x40:
                r -= 1 << s
            return r, i


def name(b, i):
    n, i = uleb(b, i)
    return b[i:i + n].decode('latin-1'), i + n


def records(path):
    """The source_location records of one wasm relocatable object: [(file, function, line, column)]."""
    b = open(path, 'rb').read()
    if b[:8] != b'\0asm\x01\0\0\0':
        raise ValueError(f'{path}: not a wasm object')
    sections, i = [], 8
    while i < len(b):
        sid = b[i]
        n, j = uleb(b, i + 1)
        sections.append((sid, j, j + n))
        i = j + n
    data = code = None   # (section index, content start)
    segments = []        # (content-relative start, bytes)
    bodies = []          # (content-relative start, end) of each defined function's body
    imported = 0         # imported functions, which come first in the function index space
    symbols = []         # per symbol: ('data', segment, offset), ('func', function index, name) or None
    relocs = []          # (target section, [(type, offset, symbol, addend)])
    for k, (sid, a, e) in enumerate(sections):
        if sid == 2:
            n, j = uleb(b, a)
            for _ in range(n):
                _, j = name(b, j)
                _, j = name(b, j)
                kind = b[j]
                j += 1
                if kind == 0:
                    imported += 1
                    _, j = uleb(b, j)
                elif kind == 1:                         # table: reftype, limits
                    flags, j = uleb(b, j + 1)
                    _, j = uleb(b, j)
                    if flags & 1:
                        _, j = uleb(b, j)
                elif kind == 2:                         # memory: limits
                    flags, j = uleb(b, j)
                    _, j = uleb(b, j)
                    if flags & 1:
                        _, j = uleb(b, j)
                elif kind == 3:                         # global: type, mutability
                    j += 2
                elif kind == 4:                         # tag
                    j += 1
                    _, j = uleb(b, j)
        elif sid == 10:
            code = (k, a)
            n, j = uleb(b, a)
            for _ in range(n):
                size, j = uleb(b, j)
                bodies.append((j - a, j - a + size))
                j += size
        elif sid == 11:
            data = (k, a)
            n, j = uleb(b, a)
            for _ in range(n):
                flags, j = uleb(b, j)
                if flags & 2:
                    _, j = uleb(b, j)
                if not flags & 1:                       # i32.const / i64.const / global.get, then end
                    op = b[j]
                    _, j = (uleb if op == 0x23 else sleb)(b, j + 1)
                    assert b[j] == 0x0b, f'{path}: unexpected segment offset expression'
                    j += 1
                size, j = uleb(b, j)
                segments.append((j - a, b[j:j + size]))
                j += size
        elif sid == 0:
            cname, j = name(b, a)
            if cname == 'linking':
                _, j = uleb(b, j)                       # version
                while j < e:
                    kind = b[j]
                    size, j = uleb(b, j + 1)
                    end = j + size
                    if kind == 8:                       # WASM_SYMBOL_TABLE
                        count, j = uleb(b, j)
                        for _ in range(count):
                            skind = b[j]
                            flags, j = uleb(b, j + 1)
                            undefined = flags & 0x10
                            if skind in (0, 2, 4, 5):   # function, global, tag, table
                                index, j = uleb(b, j)
                                sname = None
                                if not undefined or flags & 0x40:
                                    sname, j = name(b, j)
                                symbols.append(('func', index, sname) if skind == 0 else None)
                            elif skind == 1:            # data
                                _, j = name(b, j)
                                if undefined:
                                    symbols.append(None)
                                else:
                                    seg, j = uleb(b, j)
                                    off, j = uleb(b, j)
                                    _, j = uleb(b, j)
                                    symbols.append(('data', seg, off))
                            elif skind == 3:            # section
                                _, j = uleb(b, j)
                                symbols.append(None)
                            else:
                                raise ValueError(f'{path}: symbol kind {skind}')
                    j = end
            elif cname.startswith('reloc.'):
                target, j = uleb(b, j)
                count, j = uleb(b, j)
                entries = []
                for _ in range(count):
                    rtype = b[j]
                    off, j = uleb(b, j + 1)
                    idx, j = uleb(b, j)
                    add = 0
                    if rtype in WITH_ADDEND:
                        add, j = sleb(b, j)
                    entries.append((rtype, off, idx, add))
                relocs.append((target, entries))
    if data is None:
        return []
    by_offset = {}
    referrers = collections.defaultdict(set)    # (segment, offset) -> names of the functions whose code uses it
    func_name = {s[1]: s[2] for s in symbols if s and s[0] == 'func' and s[2]}
    for target, entries in relocs:
        if target == data[0]:
            for rtype, off, idx, add in entries:
                if rtype == R_WASM_MEMORY_ADDR_I32:
                    by_offset[off] = (idx, add)
        elif code is not None and target == code[0]:
            for rtype, off, idx, add in entries:
                sym = symbols[idx] if idx < len(symbols) else None
                if rtype not in WITH_ADDEND or sym is None or sym[0] != 'data':
                    continue
                for k, (lo, hi) in enumerate(bodies):
                    if lo <= off < hi:
                        referrers[(sym[1], sym[2] + add)].add(func_name.get(imported + k, f'<function {imported + k}>'))
                        break

    def string(idx, add):
        sym = symbols[idx] if idx < len(symbols) else None
        if sym is None or sym[0] != 'data':
            return None
        _, seg, off = sym
        raw = segments[seg][1]
        start = off + add
        endz = raw.find(b'\0', start)
        return raw[start:endz].decode('latin-1') if endz >= 0 else None

    out = []
    for seg, (start, raw) in enumerate(segments):
        for p in range(0, len(raw) - 15, 4):
            f = by_offset.get(start + p)
            g = by_offset.get(start + p + 4)
            if f is None or g is None:
                continue
            file, func = string(*f), string(*g)
            if file is None or func is None or not file.startswith('/'):
                continue
            line = int.from_bytes(raw[p + 8:p + 12], 'little')
            col = int.from_bytes(raw[p + 12:p + 16], 'little')
            out.append(((file, func, line, col), frozenset(referrers.get((seg, p), ()))))
    return out


def link_objects():
    if not os.path.exists(LINK_CMD):
        sys.exit(f'no {os.path.relpath(LINK_CMD, ROOT)}: run TMPDIR=/tmp make -C src link first')
    words = open(LINK_CMD).read().split()
    return [os.path.join(ROOT, 'build', 'out', w) for w in words if w.startswith('app/') and w.endswith('.o')]


def main(argv):
    strict = '--strict' in argv
    objects = [os.path.abspath(a) for a in argv if a != '--strict'] or link_objects()
    target = set()
    by_function = collections.defaultdict(list)
    with open(LEDGER, encoding='utf-8') as f:
        next(f)
        for line in f:
            p = line.rstrip('\n').split('\t')
            target.add((p[1], p[4], int(p[2]), int(p[3])))
            by_function[(p[1], p[4])].append(f'{p[2]}:{p[3]}')
    matched = set()
    for line in open(MATCHES):
        p = line.split()
        if len(p) >= 2 and not p[0].startswith('#'):
            matched.add(p[1].split('@')[0])
    broken, unfinished, seen, files = [], [], 0, set()
    for obj in objects:
        for rec, users in sorted(set(records(obj))):
            seen += 1
            files.add(rec[0])
            if rec in target:
                continue
            file, func, line, col = rec
            where = ', '.join(by_function.get((file, func), [])) or 'no record of this function in this file'
            mine = sorted(users & matched)
            msg = (f'{os.path.relpath(obj, ROOT)}: {func} at {file}:{line}:{col}; the target has {where}; '
                   f'loaded by {", ".join(sorted(users)) or "no function"}')
            (broken if mine else unfinished).append(msg + (f' (matched: {", ".join(mine)})' if mine else ''))
    for m in broken:
        print('MOVED ' + m, file=sys.stderr)
    for m in unfinished:
        print(('MOVED ' if strict else 'unmatched ') + m, file=sys.stderr)
    print(f'{seen} source_location records in {len(objects)} objects ({len(files)} files): '
          f'{seen - len(broken) - len(unfinished)} are records of the target; {len(broken)} that matched functions '
          f'load are not; {len(unfinished)} that only unmatched functions load are not')
    return 1 if broken or (strict and unfinished) else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
