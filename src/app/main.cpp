// `int main()` with no parameters: the glue's callMain passes argc=0, argv=0 and run() takes no args
// (emcc MAIN_READS_PARAMS=0). Export Kb _main has the wasm type (i32,i32)->i32 of the libc wrapper.
#include "roots.h"

int main() { return urna_roots(4); }
