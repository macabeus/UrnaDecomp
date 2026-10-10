// Roots: keep the api/io + api/util units' functions in the skeleton link the way the target keeps them
// (callers in uenux2 TUs not reconstructed yet, or invoke_* table slots): address-taken from a static constructor,
// declared through asm labels with the wasm-level signatures (tools/round/gen_try_roots.py output). Left out on purpose,
// because the target inlines them into their single caller: CFile::SetFileMode (into Open), CFile::Flush/ReadLine (into
// uenux2 callers), CFile::~CFile, CFile::WriteLine (dead), Description (into the CIoError ctor), CBitArray::GetValor(ueqword,
// uebyte) const (into GetValor(uebyte)), CSynchronizer::CreateInst (into CFile::Sync). Compiled as uenux2 code.
#include <string>

#include "ecourna/api/io/cfile.hpp"

// The CFile constructor is defined inline in cfile.hpp; the target's copy (fn 517) is one emitted by a uenux2 TU, so
// this uenux2-flavoured TU constructs CFile from optnone functions (no inlining into them) so the same out-of-line COMDAT
// copy reaches the link.
__attribute__((optnone, noinline)) void rapimisc_use_cfile_1(const std::string& a, const std::string& b) { ecourna::api::io::CFile f(a, b); }
__attribute__((optnone, noinline)) void rapimisc_use_cfile_2(const std::string& a, const std::string& b) { ecourna::api::io::CFile f(a, b, ecourna::api::io::FM_SYNC); }
__attribute__((optnone, noinline)) void rapimisc_use_cfile_3(const std::string& a) { ecourna::api::io::CFile f(a, "rb"); }

extern "C" {
void rapimisc_root_0(int, int) __asm__("_ZN7ecourna3api2io5CFile14ReadFileBinaryERKNSt3__210filesystem4pathE");
void rapimisc_root_1(int, int) __asm__("_ZN7ecourna3api2io5CFile15ReadFileContentERKNSt3__212basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEE");
int rapimisc_root_2(int, int, int, int) __asm__("_ZN7ecourna3api2io5CFile4OpenERKNSt3__212basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEESB_NS1_8FileModeE");
void rapimisc_root_3(int) __asm__("_ZN7ecourna3api2io5CFile5CloseEv");
int rapimisc_root_5(int, int, int, int, int, int) __asm__("_ZN7ecourna3api2io8CIoErrorC2ENS1_8EIoErrorENS1_14EFileOperationERKNSt3__212basic_stringIcNS5_11char_traitsIcEENS5_9allocatorIcEEEEiNS5_15source_locationE");
int rapimisc_root_6(int) __asm__("_ZNK7ecourna3api2io5CFile3EofEv");
int rapimisc_root_7(int, int, int) __asm__("_ZNK7ecourna3api2io5CFile4SeekEii");
void rapimisc_root_8(int) __asm__("_ZNK7ecourna3api2io5CFile4SyncEv");
int rapimisc_root_9(int, int, int) __asm__("_ZNK7ecourna3api2io5CFile7RawReadEPvj");
int rapimisc_root_10(int) __asm__("_ZNK7ecourna3api2io5CFile8PositionEv");
int rapimisc_root_11(int, int, int) __asm__("_ZNK7ecourna3api2io5CFile8RawWriteEPKvj");
int rapimisc_root_12(int, int) __asm__("_ZN7ecourna3api4util9CBitArray8GetValorEh");
}

namespace {

void* volatile rapimisc_sink;

struct SRapimiscRoots {
    SRapimiscRoots()
    {
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_0);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_1);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_2);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_3);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_5);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_6);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_7);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_8);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_9);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_10);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_11);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_root_12);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_use_cfile_1);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_use_cfile_2);
        rapimisc_sink = reinterpret_cast<void*>(&rapimisc_use_cfile_3);
    }
} rapimisc_roots;

} // namespace
