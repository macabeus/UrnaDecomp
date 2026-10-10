# uenux2 api::IScreen::GetFontMetrics (target fn 8938), the inline default of the virtual member in
# src/uenux2/src/api/gui/iscreen.h (its throw is the record at iscreen.h line 69, column 15), with the EUeGuiError code
# it throws (eueguierror.h). The unit is a header and has no source of its own: IScreen's vtable, which holds the body
# in the target, is emitted by the TU that builds simulador::CWasmScreen, not reconstructed yet. Until then the roots
# file below emits the inline body and address-takes it (see its header).
# compiled from its /work path with -iquote /work/src/uenux2/src: the quoted include of api/gui/iscreen.h then resolves
# to the absolute path, which -ffile-prefix-map rewrites to /home/rubio/tse/uenux2/src/api/gui/iscreen.h, the file
# name of the target's record (-iquote directories are searched before the -I ones, so the relative -Isrc/uenux2/src
# of APP_CXXFLAGS does not win)
$(ROOT)/$(APP)/19-uenux2-iscreen-roots.o: $(ROOT)/src/units/19-uenux2-iscreen-roots.cpp $(ROOT)/src/Makefile $(ROOT)/src/units/19-uenux2-iscreen.mk $(wildcard $(ROOT)/src/uenux2/src/api/gui/*.h $(ROOT)/src/ecourna/api/exception/*.hpp)
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -iquote /work/src/uenux2/src -c /work/src/units/19-uenux2-iscreen-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/19-uenux2-iscreen-roots.o
