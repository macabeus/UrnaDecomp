// The RHVoice lane's keep-alive driver compiled as app code (see roots_7zip.cpp).
#include <emscripten.h>
#undef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#include "../../tools/libs/rhvoice/driver.cpp"
