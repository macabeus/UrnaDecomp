// Roots: keep api::CBmpConversor::VerticalFlip and InvertColors (target fns 3664 and 3663) in the skeleton link. The
// target calls both directly from the same three functions (10316 10451 10465, ledger/callgraph.tsv), none
// reconstructed yet and none through the table; two noinline callers of both give each more than one call site, so
// binaryen keeps their bodies out of line as the target does, and ConverteImagem passes every argument through, so
// binaryen's dead-argument elimination folds none of them in. They are address-taken from a static constructor.
// Declared through asm labels with the wasm-level signatures; compiled as uenux2 code.
extern "C" {
void w3664_root_0(int, int, int) __asm__("_ZN3api13CBmpConversor12VerticalFlipEPhii");
void w3663_root_0(int, int, int) __asm__("_ZN3api13CBmpConversor12InvertColorsEPhii");
}

namespace {

__attribute__((noinline)) void ConverteImagem(int dados, int largura, int altura)
{
    w3664_root_0(dados, largura, altura);
    w3663_root_0(dados, largura, altura);
}

__attribute__((noinline)) void ConverteLinha(int dados, int largura)
{
    w3664_root_0(dados, largura, 1);
    w3663_root_0(dados, largura, 1);
}

void* volatile w3663_sink;

struct SW3663Roots {
    SW3663Roots()
    {
        w3663_sink = reinterpret_cast<void*>(&ConverteImagem);
        w3663_sink = reinterpret_cast<void*>(&ConverteLinha);
    }
} w3663_roots;

} // namespace
