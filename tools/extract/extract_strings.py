"""Dump every NUL-terminated string in the data segments.
usage: python3 -I tools/extract/extract_strings.py <wasm> <strings.tsv>
Columns: addr (decimal) \t len (bytes, without NUL) \t enc (ascii|utf8|latin1) \t text (escaped: \\t \\n \\r \\\\)
A string starts right after a NUL (or at a segment start) and has >= 4 printable bytes."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wasmmod import Module

MIN = 4
def printable(c):
    return 0x20 <= c < 0x7f or c in (9, 10, 13) or c >= 0xa0

def esc(s):
    return s.replace('\\', '\\\\').replace('\t', '\\t').replace('\n', '\\n').replace('\r', '\\r')

def decode(raw):
    if all(c < 0x80 for c in raw): return 'ascii', raw.decode('ascii')
    try: return 'utf8', raw.decode('utf-8')
    except UnicodeDecodeError: return 'latin1', raw.decode('latin-1')

def strings(mod):
    lo, img = mod.memory_image()
    covered = bytearray(len(img))
    for a, d in mod.segments: covered[a-lo:a-lo+len(d)] = b'\1' * len(d)
    i = 0; n = len(img)
    while i < n:
        if not covered[i] or not printable(img[i]):
            i += 1; continue
        # a string must start at a segment start or after a NUL
        if i > 0 and covered[i-1] and img[i-1] != 0:
            i += 1; continue
        j = i
        while j < n and covered[j] and printable(img[j]): j += 1
        if j < n and covered[j] and img[j] == 0 and j - i >= MIN:
            yield lo + i, img[i:j]
        i = j + 1

if __name__ == '__main__':
    mod = Module(sys.argv[1])
    cnt = 0
    with open(sys.argv[2], 'w', encoding='utf-8') as out:
        out.write('addr\tlen\tenc\ttext\n')
        for addr, raw in strings(mod):
            enc, s = decode(raw)
            out.write(f'{addr}\t{len(raw)}\t{enc}\t{esc(s)}\n'); cnt += 1
    print('strings', cnt)
