// First object on the TSE link line. wasm-ld emits exports in symbol-table order, and the table export
// __indirect_function_table takes its place when the first object that IMPORTS the table (one with a
// call_indirect) is parsed; the target has Eb=_votaInit, Fb=table, Gb=_votaPressKey, so votaInit's TU is
// such an object and no exporting TU precedes it (measured: with a plain `return` here the table export
// moved behind _main, whose libc wrapper imports the table). The call through a function pointer keeps it so.
// Signature from the target export type: func[7840] sig (i32) -> i32.
#include "roots.h"

int (*volatile urna_roots_ptr)(int) = urna_roots;

extern "C" int votaInit(int arg) { return urna_roots_ptr(arg); }
