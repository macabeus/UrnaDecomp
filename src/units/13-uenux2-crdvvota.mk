# uenux2 comum::CRdvVota::Candidato (target fn 1931, CRdvVota vtable slot 3) with the CVotosEleicoesVota totals it
# calls; wasm-opt inlines CVotosEleicoesVota::Candidato (one caller) into it, as in the target. Paths relative to src/uenux2.
CRDVVOTA_SRCS := src/app/comum/dados/crdvvota.cpp src/app/comum/dados/md/rdv/cvotoseleicoesvota.cpp \
  src/app/comum/dados/md/rdv/cvotoscargos.cpp src/app/comum/dados/md/rdv/cvotos.cpp src/app/comum/dados/md/rdv/cvoto.cpp
UENUX2_SRCS += $(CRDVVOTA_SRCS)
# the roots that keep the units linked (see the file header)
$(ROOT)/$(APP)/13-uenux2-crdvvota-roots.o: $(ROOT)/src/units/13-uenux2-crdvvota-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/13-uenux2-crdvvota-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/13-uenux2-crdvvota-roots.o
