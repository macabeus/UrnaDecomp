# ecourna: ecourna::app::dados::asn::CConversorMunicipioZona::DoConverte (target fn 9131) in
# app/dados/asn/cconversormunicipiozona.cpp, and the CMunicipioZona data class it reads (app/dados/cmunicipiozona.hpp).
# No record names the file: the class's vtable (ledger/vtables.json @1130576) places 9131 and DoDeconverte 9130, and the
# path follows its siblings in app/dados/asn. Compiled WITHOUT -fPIC like the rest of app/dados
# (docs/reference/facts/app-flags.md §F5), from its /work path, with the source and the headers it includes as
# prerequisites (the generic ecourna rule's header prerequisites do not apply to an object with a recipe of its own).
MUNICIPIOZONA_SRCS := app/dados/asn/cconversormunicipiozona.cpp
ECOURNA_SRCS += $(MUNICIPIOZONA_SRCS)
define MUNICIPIOZONA_NOPIC_RULE
$$(ROOT)/$$(APP)/ecourna/$(subst /,_,$(1)).o: $$(ROOT)/src/ecourna/$(1) $$(ROOT)/src/Makefile $$(ROOT)/src/units/16-ecourna-cconversormunicipiozona.mk $$(wildcard $$(ROOT)/src/ecourna/app/dados/*.hpp $$(ROOT)/src/ecourna/app/dados/asn/*.hpp $$(ROOT)/src/ecourna/api/*/*.hpp $$(ROOT)/src/asn1/*.h $$(ROOT)/src/asn1/modules/*.h)
	$$(H) mkdir -p $$(APP)/ecourna && build/docker-build.sh em++ $$(filter-out -fPIC,$$(ECOURNA_CXXFLAGS)) -fno-pic -c /work/src/ecourna/$(1) -o $$(R)
endef
$(foreach u,$(MUNICIPIOZONA_SRCS),$(eval $(call MUNICIPIOZONA_NOPIC_RULE,$(u))))
# DoDeconverte (target fn 9130) is not reconstructed: 16-ecourna-cconversormunicipiozona-pending.cpp gives the vtable a
# trapping body, compiled with the unit's flags
$(ROOT)/$(APP)/16-ecourna-cconversormunicipiozona-pending.o: $(ROOT)/src/units/16-ecourna-cconversormunicipiozona-pending.cpp $(ROOT)/src/Makefile $(ROOT)/src/units/16-ecourna-cconversormunicipiozona.mk $(wildcard $(ROOT)/src/ecourna/app/dados/*.hpp $(ROOT)/src/ecourna/app/dados/asn/*.hpp $(ROOT)/src/ecourna/api/*/*.hpp $(ROOT)/src/asn1/*.h $(ROOT)/src/asn1/modules/*.h)
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(filter-out -fPIC,$(ECOURNA_CXXFLAGS)) -fno-pic -c src/units/16-ecourna-cconversormunicipiozona-pending.cpp -o $(R)
# the converter's users are not reconstructed yet: 16-ecourna-cconversormunicipiozona-roots.cpp keeps its vtable the way
# the target reaches it
$(ROOT)/$(APP)/16-ecourna-cconversormunicipiozona-roots.o: $(ROOT)/src/units/16-ecourna-cconversormunicipiozona-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/16-ecourna-cconversormunicipiozona-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/16-ecourna-cconversormunicipiozona-pending.o $(APP)/16-ecourna-cconversormunicipiozona-roots.o
