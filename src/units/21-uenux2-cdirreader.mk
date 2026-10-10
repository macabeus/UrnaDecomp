# uenux2 api::CDirReader::NextEntry (target fn 1260) in src/uenux2/src/api/util/cdirreader.cpp, the file its
# std::source_location record names (line 43 col 15, the CUeUtilError 7010 throw), with the class header
# (api/util/cdirreader.h: the directory path, the DIR* and the current dirent*, at +0, +12 and +16) and the 7010
# enumerator of api/util/eueutilerror.h. The constructor (fn 1914) is not reconstructed yet: it calls fn 5470, unnamed.
# Paths relative to src/uenux2; the generic uenux2 rule depends on the source and every uenux2 header.
CDIRREADER_SRCS := src/api/util/cdirreader.cpp
UENUX2_SRCS += $(CDIRREADER_SRCS)
# the uenux2 rule compiles src/uenux2/<path> (relative), which UENUX2_PREFIX_MAP's /work/src/uenux2 never matches; the
# record must name /home/rubio/tse/uenux2/<path> as the target's does (as in 13-uenux2-crdvvota.mk)
$(addprefix $(ROOT)/$(APP)/uenux2/,$(addsuffix .o,$(CDIRREADER_SRCS))): \
  UENUX2_CXXFLAGS += -ffile-prefix-map=src/uenux2=/home/rubio/tse/uenux2
# NextEntry's six callers are not reconstructed yet: 21-uenux2-cdirreader-roots.cpp gives it two call sites
$(ROOT)/$(APP)/21-uenux2-cdirreader-roots.o: $(ROOT)/src/units/21-uenux2-cdirreader-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/21-uenux2-cdirreader-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/21-uenux2-cdirreader-roots.o
