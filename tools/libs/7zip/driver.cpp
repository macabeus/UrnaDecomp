// Keep-alive driver for the 7-Zip probe: the library's entry points the vota app reaches (RTTI in the target:
// CUpdateCallbackConsole, COpenCallbackConsole, CArchiveUpdateCallback, CCodecs, COpenCallbackImp,
// CArchiveOpenCallback_Offset, CExtractCallback_To_OpenCallback, NArchive::NParser::CHandler), so that
// wasm-opt keeps the same reachable set as in the app instead of exporting every function (an export blocks
// the one-caller inlining and dead-argument elimination the target's functions went through).
#include <emscripten.h>

#include "CPP/Common/Common.h"  // what every 7-Zip TU gets from its StdAfx.h (MY_ARRAY_NEW, UNUSED_VAR, ...)

#include "CPP/7zip/UI/Common/ArchiveExtractCallback.h"
#include "CPP/7zip/UI/Common/LoadCodecs.h"
#include "CPP/7zip/UI/Common/OpenArchive.h"
#include "CPP/7zip/UI/Common/Update.h"
#include "CPP/7zip/UI/Console/OpenCallbackConsole.h"
#include "CPP/7zip/UI/Console/UpdateCallbackConsole.h"

extern "C" {

EMSCRIPTEN_KEEPALIVE int probe_update(const char *arcPath) {
  CCodecs *codecs = new CCodecs;
  CMyComPtr<IUnknown> codecsRef = codecs;
  if (codecs->Load() != S_OK) {
    return 1;
  }
  CObjectVector<COpenType> types;
  NWildcard::CCensor censor;
  CUpdateOptions options;
  CUpdateErrorInfo errorInfo;
  CUpdateCallbackConsole callback;
  COpenCallbackConsole openCallback;
  UString arc;
  arc = arcPath;
  return (int)UpdateArchive(codecs, types, arc, censor, options, errorInfo, &openCallback, &callback, true);
}

EMSCRIPTEN_KEEPALIVE int probe_extract(const char *arcPath) {
  CCodecs *codecs = new CCodecs;
  CMyComPtr<IUnknown> codecsRef = codecs;
  if (codecs->Load() != S_OK) {
    return 1;
  }
  CObjectVector<COpenType> types;
  CArchiveLink link;
  COpenOptions op;
  op.codecs = codecs;
  op.types = &types;
  op.filePath = arcPath;
  COpenCallbackConsole openCallback;
  if (link.Open3(op, &openCallback) != S_OK) {
    return 2;
  }
  CArchiveExtractCallback *ecs = new CArchiveExtractCallback;
  CMyComPtr<IArchiveExtractCallback> ecsRef = ecs;
  return (int)link.GetArchive()->Extract(NULL, (UInt32)(Int32)-1, 0, ecs);
}
}
