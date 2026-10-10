"""Minimal wasm binary reader: sections, imports, function types, data segments as a
flat memory image, and the funcref table (elem segment). Enough for appmap scripts."""
import struct

def read_uleb(b, i):
    r = 0; s = 0
    while True:
        c = b[i]; i += 1
        r |= (c & 0x7f) << s; s += 7
        if not c & 0x80: return r, i

def read_sleb(b, i):
    r = 0; s = 0
    while True:
        c = b[i]; i += 1
        r |= (c & 0x7f) << s; s += 7
        if not c & 0x80:
            if c & 0x40: r -= 1 << s
            return r, i

def const_expr(b, i):
    """Parse a constant init expr; return (value, i_after_end)."""
    op = b[i]; i += 1
    if op == 0x41: v, i = read_sleb(b, i)
    elif op == 0x42: v, i = read_sleb(b, i)
    elif op == 0x23: v, i = read_uleb(b, i); v = ('global', v)
    else: raise ValueError(f'unsupported const op {op:#x} at {i-1}')
    assert b[i] == 0x0b, 'expected end'
    return v, i + 1

class Module:
    def __init__(self, path):
        b = self.bytes = open(path, 'rb').read()
        assert b[:8] == b'\0asm\x01\0\0\0'
        self.sections = []  # (id, start, end)
        i = 8
        while i < len(b):
            sid = b[i]; i += 1
            n, i = read_uleb(b, i)
            self.sections.append((sid, i, i + n))
            i += n
        self.types = self._types()
        self.imports = self._imports()
        self.n_imported_funcs = sum(1 for im in self.imports if im[2] == 0)
        self.func_types = self._funcs()          # type index per defined function
        self.exports = self._exports()
        self.table = self._elem()                # table index -> func index
        self.segments = self._data()             # [(addr, bytes)]
        self.code = self._code()                 # [(func_index, body_start, body_end)]

    def _sec(self, sid):
        for s, a, e in self.sections:
            if s == sid: return a, e
        return None

    def _types(self):
        a, e = self._sec(1); b = self.bytes
        n, i = read_uleb(b, a); out = []
        for _ in range(n):
            assert b[i] == 0x60; i += 1
            np, i = read_uleb(b, i); params = list(b[i:i+np]); i += np
            nr, i = read_uleb(b, i); res = list(b[i:i+nr]); i += nr
            out.append((params, res))
        return out

    def _name(self, i):
        n, i = read_uleb(self.bytes, i)
        return self.bytes[i:i+n].decode(), i + n

    def _imports(self):
        s = self._sec(2)
        if not s: return []
        a, e = s; b = self.bytes
        n, i = read_uleb(b, a); out = []
        for _ in range(n):
            mod, i = self._name(i); fld, i = self._name(i)
            kind = b[i]; i += 1
            if kind == 0: idx, i = read_uleb(b, i); out.append((mod, fld, 0, idx))
            elif kind == 1:
                i += 1; flags = b[i]; i += 1; mn, i = read_uleb(b, i)
                if flags & 1: mx, i = read_uleb(b, i)
                out.append((mod, fld, 1, None))
            elif kind == 2:
                flags = b[i]; i += 1; mn, i = read_uleb(b, i)
                if flags & 1: mx, i = read_uleb(b, i)
                out.append((mod, fld, 2, None))
            elif kind == 3:
                i += 2; out.append((mod, fld, 3, None))
            else: raise ValueError(kind)
        return out

    def _funcs(self):
        a, e = self._sec(3); b = self.bytes
        n, i = read_uleb(b, a); out = []
        for _ in range(n):
            t, i = read_uleb(b, i); out.append(t)
        return out

    def _exports(self):
        a, e = self._sec(7); b = self.bytes
        n, i = read_uleb(b, a); out = []
        for _ in range(n):
            nm, i = self._name(i); kind = b[i]; i += 1; idx, i = read_uleb(b, i)
            out.append((nm, kind, idx))
        return out

    def _elem(self):
        s = self._sec(9)
        if not s: return {}
        a, e = s; b = self.bytes
        n, i = read_uleb(b, a); table = {}
        for _ in range(n):
            flags, i = read_uleb(b, i)
            assert flags == 0, f'elem flags {flags} unsupported'
            off, i = const_expr(b, i)
            cnt, i = read_uleb(b, i)
            for k in range(cnt):
                f, i = read_uleb(b, i); table[off + k] = f
        return table

    def _data(self):
        s = self._sec(11)
        if not s: return []
        a, e = s; b = self.bytes
        n, i = read_uleb(b, a); segs = []
        for _ in range(n):
            flags, i = read_uleb(b, i)
            if flags == 1: raise ValueError('passive segment')
            if flags == 2: _, i = read_uleb(b, i)
            addr, i = const_expr(b, i)
            ln, i = read_uleb(b, i)
            segs.append((addr, b[i:i+ln])); i += ln
        return segs

    def _code(self):
        a, e = self._sec(10); b = self.bytes
        n, i = read_uleb(b, a); out = []
        for k in range(n):
            ln, i = read_uleb(b, i)
            out.append((self.n_imported_funcs + k, i, i + ln)); i += ln
        return out

    # --- memory image helpers -------------------------------------------------
    def memory_image(self):
        """Return (lo, bytearray) covering [lo, hi) with zero fill between segments."""
        lo = min(a for a, _ in self.segments)
        hi = max(a + len(d) for a, d in self.segments)
        img = bytearray(hi - lo)
        for a, d in self.segments: img[a-lo:a-lo+len(d)] = d
        return lo, img

    def segment_intervals(self):
        return sorted((a, a + len(d)) for a, d in self.segments)

if __name__ == '__main__':
    import sys
    m = Module(sys.argv[1])
    print('types', len(m.types), 'imports', len(m.imports), 'imported funcs', m.n_imported_funcs,
          'defined funcs', len(m.func_types), 'table entries', len(m.table),
          'data segs', len(m.segments), 'data bytes', sum(len(d) for _, d in m.segments))
    lo, img = m.memory_image()
    print('data range', lo, lo + len(img))
    print('exports', [(n, k, i) for n, k, i in m.exports])
