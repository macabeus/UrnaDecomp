// Definitions the 7-Zip 21.02 object set (tools/libs/7zip/sources.txt) needs and emscripten's libc has not:
// sysinfo (Windows/System.cpp GetRamSize), sched_setaffinity (Windows/System.h CProcessAffinity), and the two
// benchmark entry points Console/Main.cpp calls (Bench.cpp and SystemInfo.cpp are not in the list: none of their
// strings is in the target). Whatever the TSE build did about them — patched sources, its own stubs or
// -sERROR_ON_UNDEFINED_SYMBOLS=0 — is unknown; these keep the link line exact. wasm-ld drops them when unreferenced.
#include <sched.h>

#include "CPP/Common/Common.h"
#include "CPP/7zip/UI/Common/Bench.h"

extern "C" int sysinfo(void *info) { (void)info; return -1; }
extern "C" int sched_setaffinity(pid_t pid, size_t size, const cpu_set_t *set) { (void)pid; (void)size; (void)set; return -1; }

HRESULT Bench(DECL_EXTERNAL_CODECS_LOC_VARS IBenchPrintCallback *, IBenchCallback *, const CObjectVector<CProperty> &, UInt32, bool) {
  return E_NOTIMPL;
}
void GetCpuName(AString &s) { s.Empty(); }
