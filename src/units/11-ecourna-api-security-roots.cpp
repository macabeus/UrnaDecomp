// Roots: keep the api/security units' functions in the skeleton link the way the target keeps them: every one of
// them is reached through invoke_* (a table slot) or has at most one caller that is not reconstructed yet, so they are
// address-taken from a static constructor like tools/round/gen_try_roots.py does (bodies intact, no export, no inlining).
// Declared through asm labels with the wasm-level signatures so no header is needed here. Compiled as uenux2 code.
extern "C" {
void rsec_root_0(int, int) __asm__("_ZN7ecourna3api8security10CAesCipher6SetKeyERKNS1_7CAesKeyE");
void rsec_root_1(int, int, int) __asm__("_ZN7ecourna3api8security10CAesCipher7DecryptERKNSt3__26vectorIhNS3_9allocatorIhEEEERS7_");
void rsec_root_2(int, int, int) __asm__("_ZN7ecourna3api8security10CAesCipher7EncryptERKNSt3__26vectorIhNS3_9allocatorIhEEEERS7_");
int rsec_root_3(int, int, int, int) __asm__("_ZN7ecourna3api8security10CAesCipherC1ENS1_15EOperationModesENS1_23ESymmetricCipherKeySizeENSt3__210shared_ptrINS1_4IRngEEE");
void rsec_root_4(int, int) __asm__("_ZN7ecourna3api8security9CHKDFSeed7GetSeedEv");
int rsec_root_5(int, int, int, int, int, int) __asm__("_ZN7ecourna3api8security4CKeyC1ERKNSt3__212basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjNS1_7KeyTypeERKNS3_6vectorIhNS7_IhEEEESB_");
int rsec_root_6(int, int) __asm__("_ZN7ecourna3api8security8CKeyDataC1ERKNSt3__26vectorIhNS3_9allocatorIhEEEE");
void rsec_root_7(int) __asm__("_ZN7ecourna3api8security4CSha5ResetEv");
void rsec_root_8(int, int) __asm__("_ZN7ecourna3api8security4CSha6FinishEv");
void rsec_root_9(int, int) __asm__("_ZN7ecourna3api8security4CSha6UpdateERKNSt3__26vectorIhNS3_9allocatorIhEEEE");
int rsec_root_10(int, int, int) __asm__("_ZN7ecourna3api8security4CShaC1ENSt3__212basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEm");
void rsec_root_11(int) __asm__("_ZN7ecourna3api8security4CShaD0Ev");
int rsec_root_12(int) __asm__("_ZN7ecourna3api8security4CShaD2Ev");
int rsec_root_13(int) __asm__("_ZN7ecourna3api8security7CSha512C1Ev");
void rsec_root_14(int, int, int, int) __asm__("_ZNK7ecourna3api8security11CSiphashMac5DoMacERKNSt3__26vectorIhNS3_9allocatorIhEEEERS7_S9_");
int rsec_root_15(int, int, int, int) __asm__("_ZNK7ecourna3api8security11CSiphashMac8DoVerifyERKNSt3__26vectorIhNS3_9allocatorIhEEEES9_S9_");
void rsec_root_16(int, int, int, int) __asm__("_ZNK7ecourna3api8security13IMacAlgorithm3MacERKNSt3__26vectorIhNS3_9allocatorIhEEEERS7_S9_");
int rsec_root_17(int, int) __asm__("_ZN7ecourna3api8security16ISymmetricCipherC2ERKNS1_7CAesKeyE");
int rsec_root_18(int) __asm__("_ZN7ecourna3api8security7CAesKeyD1Ev");
}

namespace {

void* volatile rsec_sink;

struct SRsecRoots {
    SRsecRoots()
    {
        rsec_sink = reinterpret_cast<void*>(&rsec_root_0);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_1);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_2);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_3);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_4);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_5);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_6);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_7);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_8);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_9);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_10);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_11);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_12);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_13);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_14);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_15);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_16);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_17);
        rsec_sink = reinterpret_cast<void*>(&rsec_root_18);
    }
} rsec_roots;

} // namespace
