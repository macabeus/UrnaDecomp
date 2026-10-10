# SQLite block (3.50.4, 1,4xx functions) — reproduction

Compile line that reproduces the target's SQLite functions (see `build.sh`, `census.json`):

```
emcc -O3 -fPIC -DSQLITE_ENABLE_COLUMN_METADATA -DSQLITE_ENABLE_MATH_FUNCTIONS -DSQLITE_ENABLE_RTREE \
     -DSQLITE_ENABLE_UNLOCK_NOTIFY -DHAVE_FDATASYNC -DHAVE_GMTIME_R -DHAVE_LOCALTIME_R \
     -DHAVE_POSIX_FALLOCATE -DHAVE_STRERROR_R -DHAVE_USLEEP -c sqlite3.c
```

= conan-center `sqlite3/3.50.4` with default options, `build_type=Release`, `fPIC=True`. Linked with the
target's link line (`em++ -Oz -fexceptions … -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB`).

```
build/docker-build.sh bash tools/libs/sqlite/build.sh O3pic2        # build the probe (defaults = the line above)
tools/libs/sqlite/census.sh O3pic2 <scratch-dir>                     # census → build/out/sqlite/O3pic2/census.json
```

`census.json`: `functions[<target fn index>] = {cand, kind, class, n, name}`; kinds `byte` (same bytes), `exact`
(same rows after normalising call targets, call_indirect type → signature, static addresses → A), `table` (exact
modulo function-table slot constants whose functions also match), `shape` (toolkit shape only), `none`.
`summary.coverage` counts from the probe's side (probe sqlite functions found in the target).

What cannot match in an isolated probe (whole-program effects of binaryen at link time, not flags): API
functions whose return value the app never uses (DAE drops it: `sqlite3_initialize`, `sqlite3_config`,
`sqlite3_mutex_alloc`, …), libc callees specialised for their call sites (`pthreadMutexAlloc` passes the
constant `1` that the app's single `pthread_mutexattr_settype` caller had folded in), `unixSleep` (nanosleep
inlined or not), duplicate-function groups (one `end`-only body for many names). `main.c`'s `keep()` gives the
libc functions a second caller so that the probe's sqlite bodies are not specialised the other way.
