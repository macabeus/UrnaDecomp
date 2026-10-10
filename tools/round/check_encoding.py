"""Check that every tracked file a section of .editorconfig gives `charset = latin1` is still Latin-1.

    python3 -I -B tools/round/check_encoding.py      # exit 1 naming each file that looks re-encoded (CI)

A Latin-1 file with accented letters is almost never valid UTF-8, so a file declared latin1 whose non-ASCII bytes
decode as UTF-8 was most likely re-saved by a UTF-8 editor; a file declared utf-8 must decode as UTF-8. Files with
ASCII bytes only pass either way. The section globs support `*`, `**`, `?` and `{a,b}`; the last matching section
wins, as in EditorConfig.
"""
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def glob_regex(glob):
    out, i = '', 0
    while i < len(glob):
        c = glob[i]
        if glob.startswith('**', i):
            out, i = out + '.*', i + 2
            continue
        out += {'*': '[^/]*', '?': '[^/]', '{': '(?:', '}': ')', ',': '|'}.get(c, re.escape(c))
        i += 1
    return re.compile(out + '$') if '/' in glob else re.compile(r'(?:.*/)?' + out + '$')


def sections():
    out, glob = [], None
    with open(os.path.join(ROOT, '.editorconfig'), encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            m = re.fullmatch(r'\[(.+)\]', line)
            if m:
                glob = glob_regex(m.group(1))
            m = re.fullmatch(r'charset\s*=\s*(\S+)', line)
            if m and glob is not None:
                out.append((glob, m.group(1)))
    return out


def main():
    rules = sections()
    files = subprocess.run(['git', 'ls-files', '-z'], cwd=ROOT, capture_output=True, check=True).stdout
    bad = []
    for path in files.decode().split('\0'):
        charset = None
        for glob, cs in rules:
            if glob.match(path):
                charset = cs
        if charset is None:
            continue
        data = open(os.path.join(ROOT, path), 'rb').read()
        if all(b < 0x80 for b in data):
            continue
        try:
            data.decode('utf-8')
            utf8 = True
        except UnicodeDecodeError:
            utf8 = False
        if charset == 'latin1' and utf8:
            bad.append(f'{path}: declared latin1 in .editorconfig, but its non-ASCII bytes are UTF-8')
        elif charset == 'utf-8' and not utf8:
            bad.append(f'{path}: declared utf-8 in .editorconfig, but it is not valid UTF-8')
    for b in bad:
        print(b, file=sys.stderr)
    if not bad:
        print('every file .editorconfig gives a charset is in that charset')
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
