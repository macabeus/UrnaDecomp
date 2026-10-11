# uenux2 vota::CThreadVota (uenux2 src/app/vota/cthreadvota.cpp) and api::CTickManager (src/api/util/ctickmanager.cpp).
# Target fn 700 is CThreadVota::StartTick with CTickManager::StartTick inlined by binaryen (its only caller).
UENUX2_SRCS += src/api/util/ctickmanager.cpp src/app/vota/cthreadvota.cpp
# The roots' headers are named here: $(UENUX2_HDRS) is still empty when the fragments are read.
CTHREADVOTA_HDRS := $(addprefix $(ROOT)/src/uenux2/src/,api/util/ctickmanager.h api/util/cwait.h api/util/eueutilerror.h api/ipc/cthread.h app/vota/cthreadvota.h)
$(ROOT)/$(APP)/02-uenux2-cthreadvota-roots.o: $(ROOT)/src/units/02-uenux2-cthreadvota-roots.cpp $(ROOT)/src/Makefile $(CTHREADVOTA_HDRS)
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/02-uenux2-cthreadvota-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/02-uenux2-cthreadvota-roots.o
