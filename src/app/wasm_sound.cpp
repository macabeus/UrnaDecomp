// The two uenux_wasm_web_sound_wait_* exports the JS side calls back through Module.ccall from
// js_wasm_web_sound_wait_async: Lb _uenux_wasm_web_sound_wait_cancel_requested (i32)->i32,
// Mb _uenux_wasm_web_sound_wait_finished (i32,i32)->nil.
#include "roots.h"

extern "C" int uenux_wasm_web_sound_wait_cancel_requested(int id) { return urna_roots(id + 5); }
extern "C" void uenux_wasm_web_sound_wait_finished(int id, int ok) { urna_roots(id + ok + 6); }
