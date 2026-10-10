// urna_roots(): one function the stub exports call so that every library entry point the app reaches
// has a real call site (an exported function is never inlined-and-removed by binaryen and so drifts
// from the target's bodies; a called one behaves as in the app — docs/reference/facts/tool-traps.md §X1).
// The calls sit behind a volatile flag and never run.
#pragma once
int urna_roots(int arg);
