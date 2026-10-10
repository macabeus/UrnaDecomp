# ecourna app/dados value types - CRegistroIdentificacaoEleitor, CSerialMidia, tiposbasicos (the CBaseType<...>::NOME
# names, CErroLeituraBiometria, and the CBaseType<ETipoIdentificadorEleitor, ...> ctor COMDAT the target links from this
# object), federacoes/CFederacao, processoeleitoral/CConfiguracaoMunicipios.
DADOS_SRCS := app/dados/tiposbasicos.cpp app/dados/cregistroidentificacaoeleitor.cpp app/dados/cserialmidia.cpp \
               app/dados/federacoes/cfederacao.cpp app/dados/processoeleitoral/cconfiguracaomunicipios.cpp
ECOURNA_SRCS += $(DADOS_SRCS)
# Compiled WITHOUT -fPIC (docs/reference/facts/app-flags.md §F5): the target folds every std::format string's {ptr,len}
# into one i64 constant (2666, 2668, 9259) - non-PIC codegen; single-TU scores -fPIC vs -fno-pic: 9047 70 vs 58,
# 2669 36 vs 20, 2666 114 vs 85.
define DADOS_NOPIC_RULE
$$(ROOT)/$$(APP)/ecourna/$(subst /,_,$(1)).o: $$(ROOT)/src/ecourna/$(1) $$(ROOT)/src/Makefile $$(ROOT)/src/units/07-ecourna-app-dados.mk $$(wildcard $$(ROOT)/src/ecourna/app/dados/*.hpp $$(ROOT)/src/ecourna/app/dados/*/*.hpp $$(ROOT)/src/ecourna/api/pattern/*.hpp $$(ROOT)/src/ecourna/api/exception/*.hpp)
	$$(H) mkdir -p $$(APP)/ecourna && build/docker-build.sh em++ $$(filter-out -fPIC,$$(ECOURNA_CXXFLAGS)) -fno-pic -c /work/src/ecourna/$(1) -o $$(R)
endef
$(foreach u,$(DADOS_SRCS),$(eval $(call DADOS_NOPIC_RULE,$(u))))
# the units' bodies have no caller in the skeleton yet: 07-ecourna-app-dados-roots.cpp keeps them the way the target reaches them
$(ROOT)/$(APP)/07-ecourna-app-dados-roots.o: $(ROOT)/src/units/07-ecourna-app-dados-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/07-ecourna-app-dados-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/07-ecourna-app-dados-roots.o
