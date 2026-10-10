# uenux2 api::CBmpConversor::VerticalFlip and InvertColors (target fns 3664 and 3663), the class's two static members,
# in src/uenux2/src/api/gui/cbmpconversor.cpp, the file their std::source_location records name (lines 23 and 27 col
# 15, the CUeGuiError 4900 and 4901 throws; line 60 col 15, the 4903 throw), with the class header
# (api/gui/cbmpconversor.h) and the three enumerators of api/gui/eueguierror.h. Paths relative to src/uenux2; the
# generic uenux2 rule depends on the source and every uenux2 header.
CBMPCONVERSOR_SRCS := src/api/gui/cbmpconversor.cpp
UENUX2_SRCS += $(CBMPCONVERSOR_SRCS)
# the uenux2 rule compiles src/uenux2/<path> (relative), which UENUX2_PREFIX_MAP's /work/src/uenux2 never matches; the
# record must name /home/rubio/tse/uenux2/<path> as the target's does (as in 13-uenux2-crdvvota.mk)
$(addprefix $(ROOT)/$(APP)/uenux2/,$(addsuffix .o,$(CBMPCONVERSOR_SRCS))): \
  UENUX2_CXXFLAGS += -ffile-prefix-map=src/uenux2=/home/rubio/tse/uenux2
# the three functions that call both are not reconstructed yet: 22-uenux2-cbmpconversor-roots.cpp gives each two call
# sites
$(ROOT)/$(APP)/22-uenux2-cbmpconversor-roots.o: $(ROOT)/src/units/22-uenux2-cbmpconversor-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/22-uenux2-cbmpconversor-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/22-uenux2-cbmpconversor-roots.o
