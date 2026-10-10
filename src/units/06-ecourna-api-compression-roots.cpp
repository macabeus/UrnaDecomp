// Roots: keep the api/compression units' functions in the skeleton link the way the target keeps them (table
// slots through invoke_*, virtual slots, or callers in uenux2 TUs not reconstructed yet): address-taken from a static
// constructor, declared through asm labels with the wasm-level signatures (tools/round/gen_try_roots.py output, minus the
// functions the target inlines: ICompressor::Add(const path&), CZip::ConvertToOpen, CZip::AssertZipIsOpened, the 7-Zip
// context ctor; plus the CLzmaCompress destructors the header defines inline). Compiled as uenux2 code.
extern "C" {
int rcomp_root_0(int, int) __asm__("_ZN7ecourna3api11compression11ICompressor3AddERKNSt3__23mapINS3_10filesystem4pathES6_NS3_4lessIS6_EENS3_9allocatorINS3_4pairIKS6_S6_EEEEEE");
void rcomp_root_1(int, int) __asm__("_ZN7ecourna3api11compression11ICompressor3AddERKNSt3__26vectorINS3_10filesystem4pathENS3_9allocatorIS6_EEEE");
int rcomp_root_2(int) __asm__("_ZN7ecourna3api11compression11ICompressorC2Ev");
void rcomp_root_3(int) __asm__("_ZN7ecourna3api11compression11ICompressorD0Ev");
int rcomp_root_4(int) __asm__("_ZN7ecourna3api11compression11ICompressorD1Ev");
void rcomp_root_5(int) __asm__("_ZN7ecourna3api11compression4CZip13CreateZipFileEv");
void rcomp_root_6(int, long long, long long, int) __asm__("_ZN7ecourna3api11compression4CZip17AtualizaProgressoEyyRKNSt3__212basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEE");
int rcomp_root_7(int) __asm__("_ZN7ecourna3api11compression4CZip22ConvertToCompressLevelEv");
void rcomp_root_8(int) __asm__("_ZN7ecourna3api11compression4CZip5CloseEv");
void rcomp_root_9(int, int, int) __asm__("_ZN7ecourna3api11compression4CZip5DoAddERKNSt3__210filesystem4pathES7_");
void rcomp_root_10(int) __asm__("_ZN7ecourna3api11compression4CZip9SafeCloseEv");
void rcomp_root_11(int) __asm__("_ZN7ecourna3api11compression4CZipD0Ev");
int rcomp_root_12(int) __asm__("_ZN7ecourna3api11compression4CZipD2Ev");
void rcomp_root_13(int, long long, int) __asm__("_ZN7ecourna3api11compression13CLzmaCompress17AtualizaProgressoEyRKNSt3__212basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEE");
void rcomp_root_14(int, int, int) __asm__("_ZN7ecourna3api11compression13CLzmaCompress5DoAddERKNSt3__210filesystem4pathES7_");
int rcomp_root_15(int, int, int, int, int, int) __asm__("_ZN4lzma8CompactaERNS_9CContextoERKNSt3__26vectorINS2_12basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEENS7_IS9_EEEERKS9_SD_bNS2_8functionIFvySF_EEE");
int rcomp_root_16(int) __asm__("_ZN4lzma9CContextoD1Ev");
void rcomp_root_17(int) __asm__("_ZN7ecourna3api11compression13CLzmaCompressD0Ev");
int rcomp_root_18(int) __asm__("_ZN7ecourna3api11compression13CLzmaCompressD2Ev");
}

namespace {

void* volatile rcomp_sink;

struct SRcompRoots {
    SRcompRoots()
    {
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_0);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_1);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_2);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_3);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_4);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_5);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_6);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_7);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_8);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_9);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_10);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_11);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_12);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_13);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_14);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_15);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_16);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_17);
        rcomp_sink = reinterpret_cast<void*>(&rcomp_root_18);
    }
} rcomp_roots;

} // namespace
