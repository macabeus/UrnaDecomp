"""List the functions to pick from for decompiling: the stubs of a tree joined with the scan, or the TSE `differs` rows.

    python3 -I -B tools/round/pick_pool.py src/ecourna                # stubs of one tree, 40 to 600 B, smallest first
    python3 -I -B tools/round/pick_pool.py --tse                      # stubs of src/uenux2, src/ecourna and src/asn1
    python3 -I -B tools/round/pick_pool.py src/uenux2 --min 9 --max 200
    python3 -I -B tools/round/pick_pool.py --differs                  # TSE code whose body of ours differs

A function not decompiled yet has a comment stub (`// f<index> …`) in the tree its code belongs to (src/README.md
"Stubs"); the stub pool prints, per stub, `index verdict size group file:line` from splat/scan.tsv, tab-separated.
Go by the tree that holds the stub, not by the `group` column, which says who calls a function
(docs/how-to-decompile.md step 1). The differs pool is step 1's filter: `differs` rows of TSE groups whose body was
compiled from src/uenux2, src/ecourna or src/asn1 (the `object` column), printed as `index symbol detail size group
object`. Both are sorted by size, then index. Reads the tracked files only; the sources are Latin-1.
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SCAN = os.path.join(ROOT, 'splat', 'scan.tsv')
TSE_TREES = ['src/uenux2', 'src/ecourna', 'src/asn1']
TSE_GROUPS = {'vota', 'comum', 'api', 'simulador', 'vota-web', 'ecourna', 'asn1-modules', 'ASN1'}
STUB = re.compile(rb'^// f([0-9]+) ')


def scan_rows():
    with open(SCAN, encoding='utf-8') as f:
        head = next(f).rstrip('\n').split('\t')
        for line in f:
            yield dict(zip(head, line.rstrip('\n').split('\t')))


def stubs(tree):
    """index -> 'file:line' of every stub under `tree` (the first one, if a function has several)."""
    out = {}
    for d, dirs, files in os.walk(os.path.join(ROOT, tree)):
        dirs.sort()
        for name in sorted(files):
            path = os.path.join(d, name)
            with open(path, 'rb') as f:
                for n, line in enumerate(f, 1):
                    m = STUB.match(line)
                    if m:
                        out.setdefault(int(m.group(1)), f'{os.path.relpath(path, ROOT)}:{n}')
    return out


def main(argv):
    args = list(argv)

    def value(flag, default):
        if flag not in args:
            return default
        i = args.index(flag)
        if i + 1 >= len(args) or not args[i + 1].isdigit():
            sys.exit(f'{flag} takes a size in bytes')
        v = int(args[i + 1])
        del args[i:i + 2]
        return v

    lo, hi = value('--min', 40), value('--max', 600)
    differs = '--differs' in args
    tse = '--tse' in args
    trees = [a for a in args if a not in ('--differs', '--tse')]
    if tse:
        trees += TSE_TREES
    unknown = [a for a in trees if a.startswith('-') or not os.path.isdir(os.path.join(ROOT, a))]
    if unknown or (differs and trees) or (not differs and not trees):
        sys.exit(__doc__.split('\n\n')[0] + '\n' + __doc__.split('\n\n')[1])
    rows = list(scan_rows())
    if differs:
        keep = [r for r in rows if r['verdict'] == 'differs' and r['group'] in TSE_GROUPS
                and re.match(r'app/(uenux2|ecourna|asn1)/', r['object'])]
        for r in sorted(keep, key=lambda r: (int(r['size']), int(r['index']))):
            print('\t'.join([r['index'], r['symbol'], r['detail'], r['size'], r['group'], r['object']]))
        return 0
    where = {}
    for t in trees:
        for i, loc in stubs(t).items():
            where.setdefault(i, loc)
    keep = [r for r in rows if int(r['index']) in where and lo <= int(r['size']) <= hi]
    for r in sorted(keep, key=lambda r: (int(r['size']), int(r['index']))):
        print('\t'.join([r['index'], r['verdict'], r['size'], r['group'], where[int(r['index'])]]))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
