// The Boost lane's probe (the header-only parts the app instantiates) compiled as app code (see roots_7zip.cpp).
#include <emscripten.h>
#undef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#include "../../tools/libs/boost/probe.cpp"
