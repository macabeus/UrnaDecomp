# ecourna: the media descriptor (app/dados/midias: CAplicativo, CAutenticacao, CDadosGeracaoMidia, CInformacaoMidia), the
# media generator's identity (app/dados/cidentificadorgeradormidia.cpp) and their five ASN.1 converters
# (app/dados/asn/midias/*, app/dados/asn/cconversoridentificadorgeradormidia.cpp).
# Compiled WITHOUT -fPIC like the rest of app/dados (docs/reference/facts/app-flags.md §F5) and WITH -fno-strict-overflow
# (docs/reference/facts/app-flags.md §F4: without it 9042, 9174, 9185 and 9200 keep 1-8 structural rows - the
# target's unfolded `x - (-64)` loads and its field-load order). ECOURNA_CXXFLAGS carries it too (src/mk/ecourna-flags.mk),
# so naming it in the rule below is redundant but harmless.
DADOS_MIDIAS_SRCS := app/dados/asn/cconversoridentificadorgeradormidia.cpp \
                app/dados/asn/midias/cconversoraplicativo.cpp app/dados/asn/midias/cconversorautenticacao.cpp \
                app/dados/asn/midias/cconversordadosgeracaomidia.cpp app/dados/asn/midias/cconversorinformacaomidia.cpp \
                app/dados/cidentificadorgeradormidia.cpp \
                app/dados/midias/caplicativo.cpp app/dados/midias/cautenticacao.cpp app/dados/midias/cdadosgeracaomidia.cpp \
                app/dados/midias/cinformacaomidia.cpp

# DADOS_MIDIAS_TRY=1 in the environment leaves these TUs and their roots out, so tools/round/try-tu.sh can compile the same TUs
# as candidates.
ifndef DADOS_MIDIAS_TRY
ECOURNA_SRCS += $(DADOS_MIDIAS_SRCS)
define DADOS_MIDIAS_RULE
$$(ROOT)/$$(APP)/ecourna/$(subst /,_,$(1)).o: $$(ROOT)/src/ecourna/$(1) $$(ROOT)/src/Makefile $$(ROOT)/src/units/08-ecourna-app-dados-midias.mk $$(wildcard $$(ROOT)/src/ecourna/app/dados/*.hpp $$(ROOT)/src/ecourna/app/dados/*/*.hpp $$(ROOT)/src/ecourna/app/dados/asn/*/*.hpp $$(ROOT)/src/ecourna/api/*/*.hpp $$(ROOT)/src/ecourna/api/*/*/*.hpp $$(ROOT)/src/asn1/*.h $$(ROOT)/src/asn1/modules/*.h)
	$$(H) mkdir -p $$(APP)/ecourna && build/docker-build.sh em++ $$(filter-out -fPIC,$$(ECOURNA_CXXFLAGS)) -fno-pic -fno-strict-overflow -c /work/src/ecourna/$(1) -o $$(R)
endef
$(foreach u,$(DADOS_MIDIAS_SRCS),$(eval $(call DADOS_MIDIAS_RULE,$(u))))
# the converters' users are not reconstructed yet: 08-ecourna-app-dados-midias-roots.cpp keeps them the way the target reaches them
$(ROOT)/$(APP)/08-ecourna-app-dados-midias-roots.o: $(ROOT)/src/units/08-ecourna-app-dados-midias-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/08-ecourna-app-dados-midias-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/08-ecourna-app-dados-midias-roots.o
endif
