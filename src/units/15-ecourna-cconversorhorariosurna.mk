# ecourna: the urna-schedule converter CConversorHorariosUrna (app/dados/asn/cconversorhorariosurna.cpp, DoDeconverte =
# target fn 9132) and the data class it builds, CHorariosUrna (app/dados/processoeleitoral/chorariosurna.cpp, whose
# constructor binaryen inlines into fn 9132, its only caller). Non-PIC like the rest of app/dados: the generic ecourna
# rule's app_dados_% -fno-PIC (src/mk/ecourna-flags.mk, docs/reference/facts/app-flags.md §F5) covers both sources.
HORARIOS_SRCS := app/dados/asn/cconversorhorariosurna.cpp app/dados/processoeleitoral/chorariosurna.cpp
ECOURNA_SRCS += $(HORARIOS_SRCS)
# prerequisites only (no recipe: the generic rule compiles them): this fragment, and the sources and the app/dados/asn,
# processoeleitoral and api/asn headers, which src/Makefile's generic prerequisites (each object's source, every ecourna
# header) cover as well
$(foreach u,$(HORARIOS_SRCS),$(eval $(ROOT)/$(APP)/ecourna/$(subst /,_,$(u)).o: $(ROOT)/src/ecourna/$(u) $(ROOT)/src/units/15-ecourna-cconversorhorariosurna.mk $(wildcard $(ROOT)/src/ecourna/app/dados/asn/*.hpp $(ROOT)/src/ecourna/app/dados/processoeleitoral/*.hpp $(ROOT)/src/ecourna/api/asn/*.hpp)))
# DoConverte (fn 9133, the key function: the vtable is emitted with it) is not reconstructed yet; it traps here, compiled
# with the unit's flags (15-ecourna-cconversorhorariosurna-pending.cpp)
$(ROOT)/$(APP)/15-ecourna-cconversorhorariosurna-pending.o: $(ROOT)/src/units/15-ecourna-cconversorhorariosurna-pending.cpp $(ROOT)/src/Makefile $(ROOT)/src/units/15-ecourna-cconversorhorariosurna.mk $(wildcard $(ROOT)/src/ecourna/app/dados/asn/*.hpp $(ROOT)/src/ecourna/app/dados/processoeleitoral/*.hpp $(ROOT)/src/ecourna/api/asn/*.hpp)
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(filter-out -fPIC,$(ECOURNA_CXXFLAGS)) -fno-PIC -c /work/src/units/15-ecourna-cconversorhorariosurna-pending.cpp -o $(R)
# the converter's user (the CConfiguracaoMunicipio converter) is not reconstructed yet: the roots file takes the vtable's
# address, as the target's vtable data puts both slots in the table
$(ROOT)/$(APP)/15-ecourna-cconversorhorariosurna-roots.o: $(ROOT)/src/units/15-ecourna-cconversorhorariosurna-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/15-ecourna-cconversorhorariosurna-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/15-ecourna-cconversorhorariosurna-pending.o $(APP)/15-ecourna-cconversorhorariosurna-roots.o
