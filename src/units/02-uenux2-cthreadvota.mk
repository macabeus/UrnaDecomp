# uenux2 vota::CThreadVota (uenux2 src/app/vota/cthreadvota.cpp) and api::CTickManager (src/api/util/ctickmanager.cpp).
# Target fn 700 is CThreadVota::StartTick with CTickManager::StartTick inlined by binaryen (its only caller).
UENUX2_SRCS += src/api/util/ctickmanager.cpp src/app/vota/cthreadvota.cpp
# These compile from the absolute path so -ffile-prefix-map rewrites __FILE__ (the src/Makefile rule passes a relative
# one, which the map never matches): the std::source_location records of CTickManager::StartTick (target 0x182fe4, in
# fn 700) and of CWait's ctor (0x183044, in fn 5447) hold /home/rubio/tse/uenux2/src/api/util/<file>.cpp.
# The headers are named here: $(UENUX2_HDRS) is still empty when the fragments are read.
CTHREADVOTA_HDRS := $(addprefix $(ROOT)/src/uenux2/src/,api/util/ctickmanager.h api/util/cwait.h api/util/eueutilerror.h api/ipc/cthread.h app/vota/cthreadvota.h)
define CTHREADVOTA_ABS_RULE
$$(ROOT)/$$(APP)/uenux2/$(1).o: $$(ROOT)/src/uenux2/$(1) $$(ROOT)/src/Makefile $$(ROOT)/src/units/02-uenux2-cthreadvota.mk $$(CTHREADVOTA_HDRS)
	$$(H) mkdir -p $$(dir $$(R)) && build/docker-build.sh em++ $$(UENUX2_CXXFLAGS) -c /work/src/uenux2/$(1) -o $$(R)
endef
$(foreach u,src/api/util/ctickmanager.cpp src/app/vota/cthreadvota.cpp src/api/util/cwait.cpp,$(eval $(call CTHREADVOTA_ABS_RULE,$(u))))
$(ROOT)/$(APP)/02-uenux2-cthreadvota-roots.o: $(ROOT)/src/units/02-uenux2-cthreadvota-roots.cpp $(ROOT)/src/Makefile $(CTHREADVOTA_HDRS)
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/02-uenux2-cthreadvota-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/02-uenux2-cthreadvota-roots.o
