// The four votaXxx exports after the table: Gb _votaPressKey (i32)->nil, Hb _votaTick ()->i32,
// Ib _votaSetAudioEnabled (i32)->nil, Jb _votaGetStateJson ()->i32 (target export types).
#include "roots.h"

extern "C" void votaPressKey(int key) { urna_roots(key); }
extern "C" int votaTick(void) { return urna_roots(1); }
extern "C" void votaSetAudioEnabled(int enabled) { urna_roots(enabled + 2); }
extern "C" const char *votaGetStateJson(void) { return urna_roots(3) ? "{}" : ""; }
