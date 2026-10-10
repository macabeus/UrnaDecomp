# ecourna: the core app/dados/asn converters (CConversorTurno, Fase, CargoID, Foto, CabecalhoEntidade,
# IdentificadorEleitor, parametrizacaourna/LabelParametrizado, TituloRelatorio, ParametrosUrna), the data classes they
# build that have out-of-line bodies (CFoto, CCabecalhoEntidade) and the ASN.1 date-time helpers (asn/datahoraasn.cpp).
# Compiled WITHOUT -fPIC like app/dados (docs/reference/facts/app-flags.md §F5; measured on 9192/9191 too: 23/20 rows
# operand-only non-PIC vs 34/29 with 3/2 structural rows -fPIC).
DADOS_ASN_SRCS := app/dados/asn/cconversorturno.cpp app/dados/asn/cconversorfase.cpp app/dados/asn/cconversorcargoid.cpp \
                 app/dados/asn/cconversorfoto.cpp app/dados/asn/cconversorcabecalhoentidade.cpp \
                 app/dados/asn/cconversoridentificadoreleitor.cpp app/dados/asn/datahoraasn.cpp \
                 app/dados/asn/parametrizacaourna/cconversorlabelparametrizado.cpp \
                 app/dados/asn/parametrizacaourna/cconversortitulorelatorio.cpp \
                 app/dados/asn/parametrizacaourna/cconversorparametrosurna.cpp \
                 app/dados/cfoto.cpp app/dados/ccabecalhoentidade.cpp app/dados/parametrizacaourna/cparametrosurna.cpp

# The converters reference the records of the generated ASN.1 modules (built by src/mk/asn1.mk), so the modules
# join the link here.
override EXTRA_OBJS += $(ASN1MOD_OBJS)

# DADOS_ASN_TRY=1 in the environment leaves the converter TUs and their roots out, so tools/round/try-tu.sh can compile the
# same TUs as candidates while the module objects above stay linked.
ifndef DADOS_ASN_TRY
ECOURNA_SRCS += $(DADOS_ASN_SRCS)
define DADOS_ASN_NOPIC_RULE
$$(ROOT)/$$(APP)/ecourna/$(subst /,_,$(1)).o: $$(ROOT)/src/ecourna/$(1) $$(ROOT)/src/Makefile $$(ROOT)/src/units/04-ecourna-app-dados-asn.mk $$(wildcard $$(ROOT)/src/ecourna/app/dados/*.hpp $$(ROOT)/src/ecourna/app/dados/*/*.hpp $$(ROOT)/src/ecourna/app/dados/asn/*/*.hpp $$(ROOT)/src/ecourna/api/*/*.hpp $$(ROOT)/src/ecourna/api/*/*/*.hpp $$(ROOT)/src/asn1/*.h $$(ROOT)/src/asn1/modules/*.h)
	$$(H) mkdir -p $$(APP)/ecourna && build/docker-build.sh em++ $$(filter-out -fPIC,$$(ECOURNA_CXXFLAGS)) -fno-pic -c /work/src/ecourna/$(1) -o $$(R)
endef
$(foreach u,$(DADOS_ASN_SRCS),$(eval $(call DADOS_ASN_NOPIC_RULE,$(u))))
# the converters' users are not reconstructed yet: 04-ecourna-app-dados-asn-roots.cpp keeps them the way the target reaches them
$(ROOT)/$(APP)/04-ecourna-app-dados-asn-roots.o: $(ROOT)/src/units/04-ecourna-app-dados-asn-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/04-ecourna-app-dados-asn-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/04-ecourna-app-dados-asn-roots.o
endif
