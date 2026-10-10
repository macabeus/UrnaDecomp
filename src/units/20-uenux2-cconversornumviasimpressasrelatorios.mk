# uenux2 comum::asn::CConversorNumViasImpressasRelatorios (vtable @1569288): DoConverte (target fn 11386, slot 2) and
# DoDesconverte (fn 11387, slot 3) in src/app/comum/dados/asn/estadoaplicacao/cconversornumviasimpressasrelatorios.cpp,
# and the comum::md::estadoaplicacao::CNumViasImpressasRelatorios data class they convert, with its out-of-line
# constructor (fn 5629, DoDesconverte's callee) in src/app/comum/dados/md/estadoaplicacao/cnumviasimpressasrelatorios.cpp.
# No record names either file: the converter's directory is where its family has records (cconversorajustedatahora.cpp,
# cconversordadocarga.cpp in app/comum/dados/asn/estadoaplicacao/), the file name is the class's (the link-order
# cluster of 11386/11387 in src/uenux2/unplaced.cpp names the same file), and the data class sits in the md
# directory the converter's mirrors (docs/how-to-decompile.md step 3). The class names come from the RTTI of the
# converter's base, comum::asn::IConversorASN<ModuloEstadoGeralDefs::NumViasImpressasRelatorios,
# comum::md::estadoaplicacao::CNumViasImpressasRelatorios>.
# Paths relative to src/uenux2; the generic uenux2 rule depends on the sources and every uenux2 header.
NUMVIAS_SRCS := src/app/comum/dados/asn/estadoaplicacao/cconversornumviasimpressasrelatorios.cpp \
  src/app/comum/dados/md/estadoaplicacao/cnumviasimpressasrelatorios.cpp
UENUX2_SRCS += $(NUMVIAS_SRCS)
# Without TBAA: DoConverte (fn 11386) reloads the entity's `fields` vector begin before each of its four
# ASN1::INTEGER::value stores, which only -fno-strict-aliasing gives (with uenux2's strict aliasing LLVM keeps it in a
# local). Every landed app/ object of uenux2 compiles to the same bytes with and without the flag, so the tree-wide
# setting is still open (docs/reference/facts/app-flags.md §F6); target-specific so the other uenux2 units keep theirs.
$(addprefix $(ROOT)/$(APP)/uenux2/,$(addsuffix .o,$(NUMVIAS_SRCS))): UENUX2_CXXFLAGS += -fno-strict-aliasing
# the converter's users are not reconstructed yet: 20-uenux2-cconversornumviasimpressasrelatorios-roots.cpp keeps its
# vtable and gives the data class constructor its second caller, the way the target reaches them
$(ROOT)/$(APP)/20-uenux2-cconversornumviasimpressasrelatorios-roots.o: $(ROOT)/src/units/20-uenux2-cconversornumviasimpressasrelatorios-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/20-uenux2-cconversornumviasimpressasrelatorios-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/20-uenux2-cconversornumviasimpressasrelatorios-roots.o
