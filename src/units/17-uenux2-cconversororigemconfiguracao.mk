# uenux2 comum::asn::CConversorOrigemConfiguracao::DoConverte (target fn 11367, vtable @1570240 slot 2) in
# src/app/comum/dados/asn/processoeleitoral/cconversororigemconfiguracao.cpp, the record of its DoDesconverte (fn 11366,
# line 38) names the file. With it: the comum::asn::IConversorASN template (src/app/comum/asn/iconversorasn.h, the base
# its RTTI names; only the class and its virtual pair, the throwing defaults at lines 89/98 not recovered) and the
# comum::md::EOrigemConfiguracao enum it converts (src/app/comum/dados/md/processoeleitoral/eorigemconfiguracao.h; no
# record names that header, the directory follows the md family the converter's directory mirrors).
# Paths relative to src/uenux2; the generic uenux2 rule depends on the source and every uenux2 header.
ORIGEMCONFIGURACAO_SRCS := src/app/comum/dados/asn/processoeleitoral/cconversororigemconfiguracao.cpp
UENUX2_SRCS += $(ORIGEMCONFIGURACAO_SRCS)
# DoDesconverte (target fn 11366) is not reconstructed: 17-uenux2-cconversororigemconfiguracao-pending.cpp gives the
# vtable a trapping body, compiled with the unit's flags
$(ROOT)/$(APP)/17-uenux2-cconversororigemconfiguracao-pending.o: $(ROOT)/src/units/17-uenux2-cconversororigemconfiguracao-pending.cpp $(ROOT)/src/Makefile $(ROOT)/src/units/17-uenux2-cconversororigemconfiguracao.mk $(wildcard $(ROOT)/src/uenux2/src/app/comum/asn/*.h $(ROOT)/src/uenux2/src/app/comum/dados/asn/processoeleitoral/*.h $(ROOT)/src/uenux2/src/app/comum/dados/md/processoeleitoral/*.h)
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(UENUX2_CXXFLAGS) -c src/units/17-uenux2-cconversororigemconfiguracao-pending.cpp -o $(R)
# the converter's users are not reconstructed yet: 17-uenux2-cconversororigemconfiguracao-roots.cpp keeps its vtable the
# way the target reaches it
$(ROOT)/$(APP)/17-uenux2-cconversororigemconfiguracao-roots.o: $(ROOT)/src/units/17-uenux2-cconversororigemconfiguracao-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/17-uenux2-cconversororigemconfiguracao-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/17-uenux2-cconversororigemconfiguracao-pending.o $(APP)/17-uenux2-cconversororigemconfiguracao-roots.o
