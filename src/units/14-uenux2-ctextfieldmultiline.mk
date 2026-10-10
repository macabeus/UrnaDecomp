# uenux2 api::CTextFieldMultiLine ctor (target fn 5498) in src/uenux2/src/api/gui/ctextfieldmultiline.cpp, with the
# gui headers it needs (primitives.h, itext.h, iformfield.h, eueguierror.h); paths relative to src/uenux2.
UENUX2_SRCS += src/api/gui/ctextfieldmultiline.cpp
# compiled from its /work path, not the generic rule's relative one, so -ffile-prefix-map turns __FILE__ in the ctor's
# std::source_location record into /home/rubio/tse/uenux2/src/api/gui/ctextfieldmultiline.cpp as the target has it
$(ROOT)/$(APP)/uenux2/src/api/gui/ctextfieldmultiline.cpp.o: $(ROOT)/src/uenux2/src/api/gui/ctextfieldmultiline.cpp $(ROOT)/src/Makefile $(ROOT)/src/units/14-uenux2-ctextfieldmultiline.mk $(wildcard $(ROOT)/src/uenux2/src/api/gui/*.h $(ROOT)/src/ecourna/api/exception/*.hpp)
	$(H) mkdir -p $(dir $(R)) && build/docker-build.sh em++ $(UENUX2_CXXFLAGS) -c /work/src/uenux2/src/api/gui/ctextfieldmultiline.cpp -o $(R)
# the class's members not reconstructed yet (trapping bodies the vtable needs; see the file header), and two callers of
# the ctor plus two more CUeGuiError throw sites (14-uenux2-ctextfieldmultiline-roots.cpp) so binaryen neither inlines the ctor nor the
# CBaseError<EUeGuiError> ctor thunk, and drops the two constant TColor arguments as the target's wasm-opt did
$(ROOT)/$(APP)/14-uenux2-ctextfieldmultiline-pending.o: $(ROOT)/src/units/14-uenux2-ctextfieldmultiline-pending.cpp $(ROOT)/src/Makefile $(wildcard $(ROOT)/src/uenux2/src/api/gui/*.h)
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(UENUX2_CXXFLAGS) -c src/units/14-uenux2-ctextfieldmultiline-pending.cpp -o $(R)
$(ROOT)/$(APP)/14-uenux2-ctextfieldmultiline-roots.o: $(ROOT)/src/units/14-uenux2-ctextfieldmultiline-roots.cpp $(ROOT)/src/Makefile $(wildcard $(ROOT)/src/uenux2/src/api/gui/*.h)
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/14-uenux2-ctextfieldmultiline-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/14-uenux2-ctextfieldmultiline-pending.o $(APP)/14-uenux2-ctextfieldmultiline-roots.o
