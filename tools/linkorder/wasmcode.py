"""Decode the code section of a wasm module far enough to count how binaryen's
reorder-functions pass counts uses: direct `call` sites + `ref.func` + funcref
table (elem) entries + function exports.

    from wasmcode import Module, uses
    m = Module(path); u = uses(m)        # dict func_index -> use count
    m.calls[f]                          # list of callees per defined function

Builds on tools/extract/wasmmod.py (sections, imports, exports, elem, code bounds).
Covers MVP + sign-ext + nontrapping-fptoint + bulk-memory + reference-types
(the feature set of emscripten 6.0.0's default target); SIMD/threads/EH
opcodes raise so a wrong assumption is loud, not silent."""
import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'extract'))
from wasmmod import Module as _Module, read_uleb, read_sleb

# opcodes with no immediate: 0x00 0x01 0x05 0x0b 0x0f 0x1a 0x1b 0x45..0xc4 0xd1
_NOIMM = {0x00, 0x01, 0x05, 0x0b, 0x0f, 0x1a, 0x1b, 0xd1} | set(range(0x45, 0xc5))
_U32 = {0x0c, 0x0d, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0xd2}   # one uleb (0x10 handled apart)
_MEM = set(range(0x28, 0x3f))                                        # memarg: align, offset


def walk(b, i, end, on_call=None, on_reffunc=None):
    """Walk one function body's instructions from i to end; call hooks with the immediate."""
    while i < end:
        op = b[i]; i += 1
        if op in _NOIMM: continue
        if op in (0x02, 0x03, 0x04):              # block type: s33
            _, i = read_sleb(b, i); continue
        if op == 0x10:
            f, i = read_uleb(b, i)
            if on_call: on_call(f)
            continue
        if op == 0x11:
            _, i = read_uleb(b, i); _, i = read_uleb(b, i); continue
        if op in _U32:
            v, i = read_uleb(b, i)
            if op == 0xd2 and on_reffunc: on_reffunc(v)
            continue
        if op == 0x0e:                            # br_table
            n, i = read_uleb(b, i)
            for _ in range(n + 1): _, i = read_uleb(b, i)
            continue
        if op == 0x1c:                            # select t*
            n, i = read_uleb(b, i); i += n; continue
        if op in _MEM:
            a, i = read_uleb(b, i)
            if a & 0x40: _, i = read_uleb(b, i)   # multi-memory index
            _, i = read_uleb(b, i); continue
        if op in (0x3f, 0x40): i += 1; continue
        if op == 0x41: _, i = read_sleb(b, i); continue
        if op == 0x42: _, i = read_sleb(b, i); continue
        if op == 0x43: i += 4; continue
        if op == 0x44: i += 8; continue
        if op == 0xd0: i += 1; continue
        if op == 0xfc:
            sub, i = read_uleb(b, i)
            if sub <= 7: continue
            if sub == 8: _, i = read_uleb(b, i); i += 1; continue
            if sub == 9: _, i = read_uleb(b, i); continue
            if sub == 10: i += 2; continue
            if sub == 11: i += 1; continue
            if sub in (12, 14): _, i = read_uleb(b, i); _, i = read_uleb(b, i); continue
            if sub in (13, 15, 16, 17): _, i = read_uleb(b, i); continue
            raise ValueError(f'0xfc sub-opcode {sub} at {i}')
        raise ValueError(f'opcode {op:#x} at {i-1}')
    return i


class Module(_Module):
    def __init__(self, path):
        super().__init__(path)
        b = self.bytes
        self.calls = {}
        self.reffuncs = {}
        self.body = {}                           # func index -> (instr_start, end) after the locals
        for f, s, e in self.code:
            n, i = read_uleb(b, s)
            for _ in range(n):
                _, i = read_uleb(b, i); i += 1
            self.body[f] = (i, e)
            cs = []; rf = []
            walk(b, i, e, cs.append, rf.append)
            self.calls[f] = cs; self.reffuncs[f] = rf

    @property
    def n_funcs(self):
        return self.n_imported_funcs + len(self.func_types)

    def names(self):
        """function index -> name from the "name" custom section (empty if none)."""
        b = self.bytes; out = {}
        for sid, a, e in self.sections:
            if sid != 0: continue
            n, i = read_uleb(b, a)
            if b[i:i+n] != b'name': continue
            i += n
            while i < e:
                sub = b[i]; i += 1; ln, i = read_uleb(b, i); j = i + ln
                if sub == 1:
                    cnt, i = read_uleb(b, i)
                    for _ in range(cnt):
                        idx, i = read_uleb(b, i); ln2, i = read_uleb(b, i)
                        out[idx] = b[i:i+ln2].decode('utf-8', 'replace'); i += ln2
                i = j
        return out


def uses(m):
    """binaryen reorder-functions use count per function index (imports included in the keys)."""
    u = {f: 0 for f in range(m.n_funcs)}
    for cs in m.calls.values():
        for f in cs: u[f] += 1
    for rf in m.reffuncs.values():
        for f in rf: u[f] += 1
    for f in m.table.values(): u[f] += 1
    for nm, kind, idx in m.exports:
        if kind == 0: u[idx] += 1
    return u


def strcmp_key(name):
    """Sort key reproducing C strcmp order on the UTF-8 bytes."""
    return name.encode('utf-8')


if __name__ == '__main__':
    m = Module(sys.argv[1]); u = uses(m)
    seq = [u[f] for f in range(m.n_imported_funcs, m.n_funcs)]
    bad = [i for i in range(1, len(seq)) if seq[i] > seq[i-1]]
    print('defined', len(seq), 'calls', sum(len(c) for c in m.calls.values()), 'table', len(m.table),
          'use-order violations', len(bad), bad[:10])


def norm_body(m, f):
    """Normalised body of defined function f: opcodes with immediates, except `call` targets (-> F) and
    i32.const / memarg offsets >= 1024 (-> A), so that identical code at different addresses hashes alike."""
    b = m.bytes; i, e = m.body[f]; out = []
    def rec(op, imm): out.append((op, imm))
    # re-walk with a recording hook
    j = i
    while j < e:
        op = b[j]; j += 1
        if op in _NOIMM: rec(op, None); continue
        if op in (0x02, 0x03, 0x04): v, j = read_sleb(b, j); rec(op, v); continue
        if op == 0x10: _, j = read_uleb(b, j); rec(op, 'F'); continue
        if op == 0x11: t, j = read_uleb(b, j); tb, j = read_uleb(b, j); rec(op, (t, tb)); continue
        if op in _U32: v, j = read_uleb(b, j); rec(op, v); continue
        if op == 0x0e:
            n, j = read_uleb(b, j); vs = []
            for _ in range(n + 1): v, j = read_uleb(b, j); vs.append(v)
            rec(op, tuple(vs)); continue
        if op == 0x1c: n, j = read_uleb(b, j); rec(op, bytes(b[j:j+n])); j += n; continue
        if op in _MEM:
            a, j = read_uleb(b, j)
            if a & 0x40: _, j = read_uleb(b, j)
            off, j = read_uleb(b, j); rec(op, (a, 'A' if off >= 1024 else off)); continue
        if op in (0x3f, 0x40): j += 1; rec(op, None); continue
        if op == 0x41: v, j = read_sleb(b, j); rec(op, 'A' if v >= 1024 or v < 0 else v); continue
        if op == 0x42: v, j = read_sleb(b, j); rec(op, v); continue
        if op == 0x43: rec(op, bytes(b[j:j+4])); j += 4; continue
        if op == 0x44: rec(op, bytes(b[j:j+8])); j += 8; continue
        if op == 0xd0: rec(op, b[j]); j += 1; continue
        if op == 0xfc:
            sub, j = read_uleb(b, j); rec(op, sub)
            if sub <= 7: continue
            if sub == 8: _, j = read_uleb(b, j); j += 1; continue
            if sub == 9: _, j = read_uleb(b, j); continue
            if sub == 10: j += 2; continue
            if sub == 11: j += 1; continue
            if sub in (12, 14): _, j = read_uleb(b, j); _, j = read_uleb(b, j); continue
            if sub in (13, 15, 16, 17): _, j = read_uleb(b, j); continue
        raise ValueError(f'opcode {op:#x}')
    return tuple(out)
