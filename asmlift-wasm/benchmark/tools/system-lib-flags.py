# Prints, as JSON, the compile flags emscripten builds each named system library with, for a link
# like the target's: -fexceptions (DISABLE_EXCEPTION_CATCHING=0, legacy JS exceptions) at -Oz
# without assertions, so the non-debug variants. Run inside the pinned image:
#   python3 system-lib-flags.py libc libc_optz libmalloc …   (each a Library class's `name`)
import json
import os
import sys

EMSCRIPTEN = '/emsdk/upstream/emscripten'
sys.path.insert(0, EMSCRIPTEN)
os.chdir(EMSCRIPTEN)

from tools import system_libs  # noqa: E402
from tools.settings import settings  # noqa: E402

settings.DISABLE_EXCEPTION_CATCHING = 0
settings.ASSERTIONS = 0


def find(name):
    for value in vars(system_libs).values():
        if isinstance(value, type) and issubclass(value, system_libs.Library) and getattr(value, 'name', None) == name:
            return value
    raise SystemExit(f'no system library named {name}')


out = {}
for name in sys.argv[1:]:
    lib = find(name).get_default_variation()
    lib.build_dir = '/tmp/system-lib-flags'
    out[name] = lib.get_cflags()
print(json.dumps(out))
