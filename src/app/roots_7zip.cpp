// The 7-Zip lane's keep-alive driver compiled as app code: its functions are called from urna_roots()
// instead of being exported (EMSCRIPTEN_KEEPALIVE neutralised before the include). The COM GUIDs (IID_*) come
// from Console/Main.cpp, which includes Common/MyInitGuid.h and is in the linked object set.
#include <emscripten.h>
#undef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#include "../../tools/libs/7zip/driver.cpp"
