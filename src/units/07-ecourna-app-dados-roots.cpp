// Roots: keep the app/dados units' functions in the skeleton link the way the target keeps them (their callers
// - the dados/asn converters, comum, vota - are not reconstructed yet). Target call shapes (ledger/callgraph.tsv + the
// elem segment): the CRegistroIdentificacaoEleitor ctors/getters, CSerialMidia's ctor, CFederacao's ctor and
// CConfiguracaoMunicipios' ctor are reached through invoke_* (table slots 6901 6900 6892 6896 6733 6697 6850) and
// address-taken here; CErroLeituraBiometria's ctor (fn 2668) has direct callers (9066, 11616) and is address-taken too;
// CBaseType<ETipoIdentificadorEleitor, 1, 3, 40>'s ctor (fn 2666) has TWO direct callers in the target (3939, 9116),
// so two noinline callers below keep binaryen from inlining it (one caller would). CBaseType<..., 34>'s ctor is NOT
// rooted: the target inlines it into fn 2668, its only caller. Declared through asm labels with the wasm-level
// signatures, so this TU instantiates nothing (its own COMDAT copy would compete with tiposbasicos.o's).
extern "C" {
int rdados_registro_1(int, int) __asm__("_ZN7ecourna3app5dados29CRegistroIdentificacaoEleitorC2ENSt3__210shared_ptrINS1_21IIdentificadorEleitorEEE");
int rdados_registro_2(int, int, int) __asm__("_ZN7ecourna3app5dados29CRegistroIdentificacaoEleitorC2ENSt3__210shared_ptrINS1_21IIdentificadorEleitorEEES6_");
void rdados_registro_hab(int, int) __asm__("_ZNK7ecourna3app5dados29CRegistroIdentificacaoEleitor27GetIdentificadorHabilitacaoEv");
void rdados_registro_pri(int, int) __asm__("_ZNK7ecourna3app5dados29CRegistroIdentificacaoEleitor25GetIdentificadorPrincipalEv");
int rdados_serial(int, int) __asm__("_ZN7ecourna3app5dados12CSerialMidiaC2ERKNSt3__212basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEE");
int rdados_federacao(int, int, int, int, int) __asm__("_ZN7ecourna3app5dados10CFederacaoC2E9CBaseTypeItLt100ELt999ELi39EERKNSt3__212basic_stringIcNS5_11char_traitsIcEENS5_9allocatorIcEEEESD_RKNS5_6vectorIhNS9_IhEEEE");
int rdados_configuracoes(int, int, int) __asm__("_ZN7ecourna3app5dados23CConfiguracaoMunicipiosC2ERKNS1_18CCabecalhoEntidadeERKNSt3__23mapI9CBaseTypeIjLj0ELj99999ELi6EENS1_22CConfiguracaoMunicipioENS6_4lessIS9_EENS6_9allocatorINS6_4pairIKS9_SA_EEEEEE");
int rdados_erro_biometria(int, int) __asm__("_ZN7ecourna3app5dados21CErroLeituraBiometriaC2ENS1_21EErroLeituraBiometriaE");
int rdados_tipo_identificador(int, int) __asm__("_ZN9CBaseTypeIN7ecourna3app5dados25ETipoIdentificadorEleitorELS3_1ELS3_3ELi40EEC2ES3_");
// library helpers the target keeps out of line because other (unreconstructed) callers share them: std::regex's
// (const char*, flags) ctor (fn 2416, 4 uses) and the map<TMunicipioID, ...> __find_equal(hint, ...) (fn 3489, 2 callers)
int rdados_regex_ctor(int, int, int) __asm__("_ZNSt3__211basic_regexIcNS_12regex_traitsIcEEEC2B8ne210108EPKcNS_15regex_constants18syntax_option_typeE");
int rdados_find_equal(int, int, int, int, int) __asm__("_ZNSt3__26__treeINS_12__value_typeI9CBaseTypeIjLj0ELj99999ELi6EEN7ecourna3app5dados22CConfiguracaoMunicipioEEENS_19__map_value_compareIS3_NS_4pairIKS3_S7_EENS_4lessIS3_EELb1EEENS_9allocatorISC_EEE12__find_equalIS3_EERPNS_16__tree_node_baseIPvEENS_21__tree_const_iteratorIS8_PNS_11__tree_nodeIS8_SL_EElEERPNS_15__tree_end_nodeISN_EESO_RKT_");
}

namespace {

void* volatile rdados_sink;

__attribute__((noinline)) int UsaTipoIdentificador1(int obj, int tipo) { return rdados_tipo_identificador(obj, tipo) + 1; }
__attribute__((noinline)) int UsaTipoIdentificador2(int obj, int tipo) { return rdados_tipo_identificador(obj + 4, tipo + 1) * 3; }

int UsaTipoIdentificador(int a)
{
    return UsaTipoIdentificador1(a, a + 2) + UsaTipoIdentificador2(a + 8, a);
}

__attribute__((noinline)) int UsaBibliotecas(int a)
{
    return rdados_regex_ctor(a, a + 40, 16) + rdados_find_equal(a, a + 4, a + 8, a + 12, a + 16);
}

struct SRdadosRoots {
    SRdadosRoots()
    {
        rdados_sink = reinterpret_cast<void*>(&rdados_registro_1);
        rdados_sink = reinterpret_cast<void*>(&rdados_registro_2);
        rdados_sink = reinterpret_cast<void*>(&rdados_registro_hab);
        rdados_sink = reinterpret_cast<void*>(&rdados_registro_pri);
        rdados_sink = reinterpret_cast<void*>(&rdados_serial);
        rdados_sink = reinterpret_cast<void*>(&rdados_federacao);
        rdados_sink = reinterpret_cast<void*>(&rdados_configuracoes);
        rdados_sink = reinterpret_cast<void*>(&rdados_erro_biometria);
        rdados_sink = reinterpret_cast<void*>(&UsaTipoIdentificador);
        rdados_sink = reinterpret_cast<void*>(&UsaBibliotecas);
    }
} rdados_roots;

} // namespace
