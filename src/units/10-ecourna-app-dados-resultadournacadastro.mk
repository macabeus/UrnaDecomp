# ecourna app/dados/resultadournacadastro - the voter-register result data classes (CResultadoUrnaCadastro,
# CDadosComparecimento, CDadosComparecimentoCifrado, CDadosCifracao, CEstadoComparecimento, CHabilitacaoBiometrica,
# CEstadoHabilitacaoPorCodigo, CComparecimentoMesario; CApresentacaoFotoEleitor/CIdentificacaoJustificativa/
# CComparecimentoSecao are header-only). Their converters (app/dados/asn/resultadournacadastro) are in 09-ecourna-app-dados-asn-resultadournacadastro.mk.
DADOS_RUC_SRCS := app/dados/resultadournacadastro/ccomparecimentomesario.cpp app/dados/resultadournacadastro/cdadoscifracao.cpp \
                 app/dados/resultadournacadastro/cdadoscomparecimento.cpp app/dados/resultadournacadastro/cdadoscomparecimentocifrado.cpp \
                 app/dados/resultadournacadastro/cestadocomparecimento.cpp app/dados/resultadournacadastro/cestadohabilitacaoporcodigo.cpp \
                 app/dados/resultadournacadastro/chabilitacaobiometrica.cpp app/dados/resultadournacadastro/cresultadournacadastro.cpp
ECOURNA_SRCS += $(DADOS_RUC_SRCS)
# Compiled WITHOUT -fPIC like the rest of app/dados (docs/reference/facts/app-flags.md §F5, also measured
# on this unit's own bodies; §F8 and §F9 have its idioms and traps).
define DADOS_RUC_NOPIC_RULE
$$(ROOT)/$$(APP)/ecourna/$(subst /,_,$(1)).o: $$(ROOT)/src/ecourna/$(1) $$(ROOT)/src/Makefile $$(ROOT)/src/units/10-ecourna-app-dados-resultadournacadastro.mk $$(wildcard $$(ROOT)/src/ecourna/app/dados/*.hpp $$(ROOT)/src/ecourna/app/dados/*/*.hpp $$(ROOT)/src/ecourna/api/pattern/*.hpp $$(ROOT)/src/ecourna/api/exception/*.hpp)
	$$(H) mkdir -p $$(APP)/ecourna && build/docker-build.sh em++ $$(filter-out -fPIC,$$(ECOURNA_CXXFLAGS)) -fno-pic -c /work/src/ecourna/$(1) -o $$(R)
endef
$(foreach u,$(DADOS_RUC_SRCS),$(eval $(call DADOS_RUC_NOPIC_RULE,$(u))))
# the units' bodies have no caller in the skeleton yet: 10-ecourna-app-dados-resultadournacadastro-roots.cpp keeps them the way the target reaches them
$(ROOT)/$(APP)/10-ecourna-app-dados-resultadournacadastro-roots.o: $(ROOT)/src/units/10-ecourna-app-dados-resultadournacadastro-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/10-ecourna-app-dados-resultadournacadastro-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/10-ecourna-app-dados-resultadournacadastro-roots.o
