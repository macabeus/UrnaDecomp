# urna/src — the full link skeleton of vota_web_wasm.{js,wasm,data}.
#
#   make -C src            # libs (through tools/libs/*/build.sh), then `link`: the app objects and the two links
#   make -C src libs       # the pinned library builds alone
#   make -C src census     # toolkit census, ledger/functions.tsv, wasmfn, the progress snapshot in ledger/history/
#   make -C src check-srcloc  # the std::source_location records of the app objects against the target's
#   make -C src splat …    # the splat build, scan, match, status and stubs targets: src/mk/splat-sub.mk
#
# Every compile runs in the pinned image (urna-emsdk:6.0.0 = emscripten/emsdk:6.0.0 + wabt) through
# build/docker-build.sh, which mounts the urna root at /work: all recipes cd to the root and use relative paths.
# The LINK ORDER is the one thing a Makefile controls that a CMake build would scramble: wasm-ld numbers the
# defined functions in input order, binaryen's reorder-functions sorts them by use count and breaks ties by the
# descending decimal string of that ordinal (docs/reference/facts/link-order.md §L1), and exports come in object order.
# LINK_OBJS below is therefore an explicit list; change it only on evidence.
ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
D := cd $(ROOT) && build/docker-build.sh
H := cd $(ROOT) &&
SHELL := /bin/bash
R = $(patsubst $(ROOT)/%,%,$@)   # the target as a root-relative path (what the container sees)

# ---- pinned library builds (tools/libs/*/build.sh; sources in third_party/, outputs under build/out, both gitignored) ----
SQLITE_O     := build/out/sqlite/O3pic2/sqlite3.o
SQLITE_SRC   := third_party/sqlite
# OpenSSL builds out of tree: OPENSSL_DIR is the build directory (objects, generated headers, libcrypto.a)
OPENSSL_DIR  := build/out/openssl/O3
OPENSSL_SRC  := third_party/openssl
LIBCRYPTO    := $(OPENSSL_DIR)/libcrypto.a
# OpenSSL's headers: the generated ones (configuration.h, opensslv.h, …) in its build directory, the rest in its tree
OPENSSL_INCLUDES := -I$(OPENSSL_DIR)/include -I$(OPENSSL_SRC)/include
SEVENZIP_DIR := build/out/7zip/21.02-st-O3
SEVENZIP_SRC := third_party/7zip
# RHVoice at -DNDEBUG: the target imports no __assert_fail, and hts_labeller/phoneme_set/userdict assert without it
RHVOICE_DIR  := build/out/rhvoice/1.14.0-cxx14-O3-ndebug
RHVOICE_SRC  := third_party/RHVoice
BOOST_HDR    := third_party/boost

$(ROOT)/$(SQLITE_O):
	$(D) bash tools/libs/sqlite/build.sh O3pic2
$(ROOT)/$(LIBCRYPTO):
	$(H) URNA_OSSL_CFLAGS=-O3 tools/libs/openssl/configure.sh O3 && tools/libs/openssl/build.sh O3
$(ROOT)/$(SEVENZIP_DIR)/probe.wasm:
	$(H) tools/libs/7zip/build.sh 21.02 st-O3 -O3
$(ROOT)/$(RHVOICE_DIR)/probe.wasm:
	$(H) tools/libs/rhvoice/build.sh 1.14.0 cxx14-O3-ndebug -O3 -std=c++14 -DNDEBUG
$(ROOT)/$(BOOST_HDR)/boost/version.hpp:
	@echo "missing $(BOOST_HDR): tools/setup/bootstrap.sh --sources fetches the Boost 1.89.0 headers"; exit 2

LIBS := $(ROOT)/$(SQLITE_O) $(ROOT)/$(LIBCRYPTO) $(ROOT)/$(SEVENZIP_DIR)/probe.wasm $(ROOT)/$(RHVOICE_DIR)/probe.wasm $(ROOT)/$(BOOST_HDR)/boost/version.hpp
.PHONY: all libs link census check-srcloc clean-app
all: libs
	$(MAKE) -C $(ROOT)/src link
libs: $(LIBS)

# the library probe links glob obj/*.o (alphabetical); their drivers are compiled as app code instead (src/app/roots_*.cpp)
# the 7-Zip objects are tools/libs/7zip/sources.txt (the Alone7z set minus the benchmark files), st rows only, in
# list order; MainAr.cpp defines 7-Zip's own main() and is left out; stale objects in obj/ are ignored.
# Both lists are read when make starts, so they are empty until `make -C src libs` has built the libraries: the link
# recipe refuses to run without them.
# src/app/stubs_7zip.cpp supplies the four symbols the set needs and emscripten has not.
SEVENZIP_OBJS := $(wildcard $(addprefix $(ROOT)/$(SEVENZIP_DIR)/obj/,$(addsuffix .o,$(subst /,_,$(shell awk '!/^\#/ && NF==1 && $$1!="CPP/7zip/UI/Console/MainAr.cpp"{print $$1}' $(ROOT)/tools/libs/7zip/sources.txt)))))   # wildcard: build.sh skips sources a release lacks (LzFindPrepare.cpp is 21.03+)
RHVOICE_OBJS  := $(filter-out %/driver.o,$(sort $(wildcard $(ROOT)/$(RHVOICE_DIR)/obj/*.o)))

# ---- app stubs ----------------------------------------------------------------------------------------------
APP := build/out/app
GEN := $(APP)/gen
# ---- the TSE application TUs' flags (docs/reference/app-flags.md; measured on the ecourna units in src/ecourna) ----------
# ecourna (conan package libecea1da310e5107): -O3 -fPIC -fexceptions -std=c++20 -DNDEBUG; the sources sit in the
# conan build dir, so the file names std::source_location records are reproduced with -ffile-prefix-map: the /work
# spelling for the sources, the relative one for the headers found through -Isrc (api/asn/iconversorasn.hpp).
ECOURNA_PREFIX_MAP := -ffile-prefix-map=/work/src/ecourna=/home/rubio/.conan2/p/b/libecea1da310e5107/b/src/ecourna \
                      -ffile-prefix-map=src/ecourna=/home/rubio/.conan2/p/b/libecea1da310e5107/b/src/ecourna
ECOURNA_CXXFLAGS   := -O3 -fPIC -fexceptions -std=c++20 -DNDEBUG $(ECOURNA_PREFIX_MAP) -Isrc -Isrc/asn1
# uenux2 (the app itself, /home/rubio/tse/uenux2, tree mirrored under src/uenux2 so the records' paths map): -O2 -fPIC,
# NO -fexceptions (emcc's default -fignore-exceptions: throw sites call __cxa_throw directly, no landing pads),
# -DNDEBUG, -fno-strict-aliasing (no TBAA: comum's converters reload a vector's begin after an int store, fn 10289);
# measured on the api/util, api/io, api/ipc/posix and comum units (docs/reference/facts/app-flags.md §F6). The
# include root of the app's own headers is src/uenux2/src ("api/util/cwait.h"); ecourna headers come from -Isrc.
UENUX2_PREFIX_MAP  := -ffile-prefix-map=/work/src/uenux2=/home/rubio/tse/uenux2 -ffile-prefix-map=src/uenux2=/home/rubio/tse/uenux2
UENUX2_CXXFLAGS    := -O2 -fPIC -std=c++20 -DNDEBUG -fno-strict-aliasing $(UENUX2_PREFIX_MAP) -Isrc -Isrc/uenux2/src -Isrc/asn1
# the ASN.1 runtime (src/asn1, an app-side tree) has its own flags and compile rule in src/mk/asn1.mk:
# -Os -fno-exceptions -std=c++17, the throwing TUs -fexceptions -fignore-exceptions
# the src/app stand-ins (exported entry points, root callers) compile with the uenux2 flags (-DNDEBUG: the target imports
# no __assert_fail) but keep strict aliasing: the library templates they instantiate (roots_rhvoice.o's std::__tree and
# __split_buffer, roots_boost.o's __tree_remove) win the link, and the target's copies have TBAA (6 matches need it)
APP_CXXFLAGS := $(filter-out -fno-strict-aliasing,$(UENUX2_CXXFLAGS))
APP_CFLAGS   := $(filter-out -std=% -fno-strict-aliasing,$(UENUX2_CXXFLAGS))
# Measuring a flag set: UENUX2_EXTRA and ECOURNA_EXTRA (empty here) are added to a tree's flags after the stand-ins'
# are derived, so a private link adds a flag to that tree's objects alone, the fragments' own additions kept
# (docs/reference/facts/app-flags.md §F9 item 6). make keeps an object whose flags changed: use a new APP directory.
UENUX2_EXTRA  ?=
ECOURNA_EXTRA ?=
UENUX2_CXXFLAGS  += $(UENUX2_EXTRA)
ECOURNA_CXXFLAGS += $(ECOURNA_EXTRA)
SEVENZIP_FLAGS := -DNDEBUG -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_7ZIP_ST -I$(SEVENZIP_SRC)
# the overlay shows the RHVoice build's generated config headers inside the source tree (tools/libs/rhvoice/build.sh)
RHVOICE_CFG    := -ivfsoverlay $(RHVOICE_DIR)/cfg/overlay.yaml
RHVOICE_FLAGS  := -std=c++14 -DRHVOICE=1 '-DPACKAGE="RHVoice"' -I$(RHVOICE_SRC)/src/include -I$(RHVOICE_SRC)/src/hts_engine \
                  -I$(RHVOICE_SRC)/src/third-party/utf8 -I$(RHVOICE_SRC)/src/third-party/rapidxml -I$(BOOST_HDR) $(RHVOICE_CFG)

# every app object also depends on this Makefile: its flags live here
$(ROOT)/$(APP)/%.o: $(ROOT)/src/app/%.cpp $(ROOT)/src/app/roots.h $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/app/$*.cpp -o $(APP)/$*.o
$(ROOT)/$(APP)/wasm_shim.o: $(ROOT)/src/web/wasm_shim.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/web/wasm_shim.cpp -o $(R)
$(ROOT)/$(APP)/stubs_7zip.o: $(ROOT)/src/app/stubs_7zip.cpp $(ROOT)/$(SEVENZIP_DIR)/probe.wasm $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) $(SEVENZIP_FLAGS) -c src/app/stubs_7zip.cpp -o $(R)
$(ROOT)/$(APP)/roots_7zip.o: $(ROOT)/src/app/roots_7zip.cpp $(ROOT)/tools/libs/7zip/driver.cpp $(ROOT)/$(SEVENZIP_DIR)/probe.wasm $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) $(SEVENZIP_FLAGS) -c src/app/roots_7zip.cpp -o $(R)
$(ROOT)/$(APP)/roots_rhvoice.o: $(ROOT)/src/app/roots_rhvoice.cpp $(ROOT)/tools/libs/rhvoice/driver.cpp $(ROOT)/$(RHVOICE_DIR)/probe.wasm $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) $(RHVOICE_FLAGS) -c src/app/roots_rhvoice.cpp -o $(R)
$(ROOT)/$(APP)/roots_boost.o: $(ROOT)/src/app/roots_boost.cpp $(ROOT)/tools/libs/boost/probe.cpp $(ROOT)/$(BOOST_HDR)/boost/version.hpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -I$(BOOST_HDR) -c src/app/roots_boost.cpp -o $(R)

# The reconstructed units: each src/units/<NN>-<component>-<what>.mk appends its sources to ECOURNA_SRCS (compiled
# with ECOURNA_CXXFLAGS from src/ecourna/, which mirrors the conan package's b/src/ecourna) or UENUX2_SRCS
# (UENUX2_CXXFLAGS from src/uenux2/). A new unit lands as a new fragment, not as a line here. The fragments are read
# in name order, which is the order their objects take in the link (src/units/README.md).
# SKIP_UNITS="11-ecourna-api-security.mk …" (make command line) leaves those fragments out: try-tu.sh --skip-units uses it
# to re-try TUs a landed fragment already links (duplicate-symbol trap).
# The build machinery in src/mk/ surrounds them: ecourna-flags.mk first, because fragments add to its flags; asn1.mk
# after, because the ASN.1 runtime's objects follow the fragments' in EXTRA_OBJS; splat-sub.mk after splat-build.mk,
# whose variables it reads.
SKIP_UNITS ?=
ECOURNA_SRCS :=
include $(ROOT)/src/mk/ecourna-flags.mk
-include $(filter-out $(addprefix $(ROOT)/src/units/,$(SKIP_UNITS)),$(sort $(wildcard $(ROOT)/src/units/*.mk)))
include $(addprefix $(ROOT)/src/mk/,asn1.mk splat-build.mk splat-map.mk splat-sub.mk)
UENUX2_SRCS ?=
ECOURNA_OBJS := $(addprefix $(ROOT)/$(APP)/ecourna/,$(addsuffix .o,$(subst /,_,$(ECOURNA_SRCS))))
# Each ecourna object depends on its own source, at any depth (the object's name flattens the path, so the pattern
# rule cannot name it: these prerequisite-only rules do), and on every ecourna header at any depth.
ECOURNA_HDRS := $(shell find $(ROOT)/src/ecourna -name '*.h' -o -name '*.hpp' 2>/dev/null)
$(foreach u,$(ECOURNA_SRCS),$(eval $(ROOT)/$(APP)/ecourna/$(subst /,_,$(u)).o: $(ROOT)/src/ecourna/$(u)))
$(ROOT)/$(APP)/ecourna/%.o: $(ROOT)/src/Makefile $(ECOURNA_HDRS)
	$(H) mkdir -p $(APP)/ecourna && build/docker-build.sh em++ $(ECOURNA_CXXFLAGS) -c /work/src/ecourna/$(subst _,/,$(patsubst %.o,%,$*)) -o $(R)
$(ROOT)/$(APP)/roots_ecourna.o: $(ROOT)/src/app/roots_ecourna.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/app/roots_ecourna.cpp -o $(R)

# the reconstructed uenux2 units: UENUX2_SRCS are paths relative to src/uenux2 (src/api/util/cwait.cpp,
# wasm/vota_web/…); the object keeps the path (build/out/app/uenux2/src/api/util/cwait.cpp.o), so file names may
# hold underscores. Every unit depends on every uenux2/ecourna header (a unit moves headers, not only .cpp files).
# A unit compiles from its /work path, which UENUX2_PREFIX_MAP rewrites: its std::source_location records then name
# /home/rubio/tse/uenux2/<path>, as the target's do (a relative path is never rewritten).
UENUX2_HDRS := $(shell find $(ROOT)/src/uenux2 -name '*.h' -o -name '*.hpp' 2>/dev/null) $(ECOURNA_HDRS)
UENUX2_OBJS := $(addprefix $(ROOT)/$(APP)/uenux2/,$(addsuffix .o,$(UENUX2_SRCS)))
$(ROOT)/$(APP)/uenux2/%.cpp.o: $(ROOT)/src/uenux2/%.cpp $(ROOT)/src/Makefile $(UENUX2_HDRS)
	$(H) mkdir -p $(dir $(R)) && build/docker-build.sh em++ $(UENUX2_CXXFLAGS) -c /work/src/uenux2/$*.cpp -o $(R)
$(ROOT)/$(APP)/roots_uenux2.o: $(ROOT)/src/app/roots_uenux2.cpp $(ROOT)/src/Makefile $(UENUX2_HDRS)
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/app/roots_uenux2.cpp -o $(R)

# generated root callers: tools/libs/openssl/gen-probe-main.py writes a main() that calls each root once with dummy
# arguments; compiled with -Dmain=<name> so it becomes a plain function urna_roots() calls.
$(ROOT)/$(GEN)/sqlite_roots.c: $(ROOT)/$(SQLITE_O) $(ROOT)/tools/libs/sqlite/attribution.tsv $(ROOT)/tools/libs/openssl/gen-probe-main.py
	$(H) mkdir -p $(GEN) && wasm-objdump -x $(SQLITE_O) > $(GEN)/sqlite3.objdump-x.txt \
	  && build/docker-build.sh /emsdk/upstream/bin/llvm-nm -g --defined-only $(SQLITE_O) | awk '$$2=="T"{print $$3}' | sort > $(GEN)/sqlite_defined.txt \
	  && grep -oE '\bsqlite3_[a-z0-9_]+' tools/libs/sqlite/attribution.tsv | sort -u | grep -Fxf $(GEN)/sqlite_defined.txt > $(GEN)/sqlite_roots.txt \
	  && python3 -I tools/libs/openssl/gen-probe-main.py $(GEN)/sqlite_roots.txt $(GEN)/sqlite3.objdump-x.txt $@
$(ROOT)/$(GEN)/openssl_roots.c: $(ROOT)/tools/libs/openssl/roots.txt $(ROOT)/$(LIBCRYPTO) $(ROOT)/tools/libs/openssl/gen-probe-main.py
	$(H) mkdir -p $(GEN) && for o in $$(find $(OPENSSL_DIR) -name '*.o' | sort); do wasm-objdump -x "$$o"; done > $(GEN)/openssl.objdump-x.txt 2>/dev/null \
	  && python3 -I tools/libs/openssl/gen-probe-main.py tools/libs/openssl/roots.txt $(GEN)/openssl.objdump-x.txt $@
$(ROOT)/$(APP)/%_roots.o: $(ROOT)/$(GEN)/%_roots.c $(ROOT)/src/Makefile
	$(H) build/docker-build.sh emcc $(APP_CFLAGS) -Dmain=urna_$*_roots -Dsink=urna_$*_sink -Dsink64=urna_$*_sink64 -Dsinkd=urna_$*_sinkd -c $(GEN)/$*_roots.c -o $(R)

# the libcrypto members the target's ERR strings prove linked, as explicit objects (tools/libs/openssl/link-probe.sh);
# the rest of libcrypto.a is pulled by reference
OSSL_OBJS_FILE := $(APP)/ossl_objs.txt
$(ROOT)/$(OSSL_OBJS_FILE): $(ROOT)/tools/libs/ossl_files.txt $(ROOT)/$(LIBCRYPTO)
	$(H) mkdir -p $(APP) && : > $@.tmp && while read -r f; do case "$$f" in *.c) ;; *) continue;; esac; d=$$(dirname "$$f"); b=$$(basename "$$f" .c); \
	  o=$$(ls $(OPENSSL_DIR)/"$$d"/*-lib-"$$b".o 2>/dev/null | head -1); [ -n "$$o" ] && echo "$$o" >> $@.tmp; done < tools/libs/ossl_files.txt; mv $@.tmp $@

# include paths an app TU may need (the libraries' headers at their pinned builds); tools/round/try-tu.sh adds them
APP_INCLUDES := -I$(BOOST_HDR) -I$(SEVENZIP_SRC) $(OPENSSL_INCLUDES) -I$(SQLITE_SRC) -I$(RHVOICE_SRC)/src/include $(RHVOICE_CFG)

# ---- link order (first object exports votaInit alone; app before libraries; libraries in this order) --------
APP_OBJS := vota_init.o vota_api.o main.o wasm_sound.o wasm_shim.o roots.o roots_ecourna.o roots_uenux2.o sqlite_roots.o openssl_roots.o roots_7zip.o stubs_7zip.o roots_rhvoice.o roots_boost.o
# EXTRA_OBJS: candidate app objects linked after the stubs and before the libraries (tools/round/try-tu.sh sets it;
# root-relative or absolute paths under build/out). OUT/OUT_NAMED are overridable the same way for a side link.
EXTRA_OBJS ?=
# app-side objects (the uenux2 units) precede the conan libraries (ecourna first); the ASN.1 runtime rides in EXTRA_OBJS (src/mk/asn1.mk)
LINK_OBJS := $(addprefix $(ROOT)/$(APP)/,$(APP_OBJS)) $(UENUX2_OBJS) $(ECOURNA_OBJS) $(abspath $(addprefix $(ROOT)/,$(patsubst $(ROOT)/%,%,$(EXTRA_OBJS)))) $(ROOT)/$(SQLITE_O) $(SEVENZIP_OBJS) $(RHVOICE_OBJS)
EXPORTS := _main,_votaInit,_votaPressKey,_votaTick,_votaSetAudioEnabled,_votaGetStateJson,_uenux_wasm_web_sound_wait_cancel_requested,_uenux_wasm_web_sound_wait_finished
LINK_FLAGS := -Oz -fexceptions -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB -sEXPORTED_FUNCTIONS=$(EXPORTS) -sEXPORTED_RUNTIME_METHODS=ccall,FS,callMain
OUT := build/out/wasm/vota_web
OUT_NAMED := build/out/wasm-named/vota_web

# linked from build/out so the packager embeds PACKAGE_NAME="wasm/vota_web/vota_web_wasm.data" like the target;
# the named twin adds --emit-symbol-map, which changes the -Oz function ORDER (never byte-compare it) but names bodies
# The link also writes the symbol lists the lifter names callees with ($(OUT)/symbols.{mangled,demangled}.txt).
link: $(ROOT)/$(OUT)/vota_web_wasm.wasm $(ROOT)/$(OUT_NAMED)/vota_web_wasm.wasm
$(ROOT)/$(OUT)/vota_web_wasm.wasm $(ROOT)/$(OUT_NAMED)/vota_web_wasm.wasm: $(LINK_OBJS) $(ROOT)/$(OSSL_OBJS_FILE) $(ROOT)/$(LIBCRYPTO) $(ROOT)/src/Makefile
	@test -n "$(SEVENZIP_OBJS)" -a -n "$(RHVOICE_OBJS)" || { echo "no 7-Zip or RHVoice objects: run make -C src libs, then link again" >&2; exit 2; }
	$(H) mkdir -p $(OUT) $(OUT_NAMED) && objs="$(patsubst $(ROOT)/build/out/%,%,$(LINK_OBJS)) $$(sed 's#^build/out/##' $(OSSL_OBJS_FILE) | tr '\n' ' ') $(patsubst build/out/%,%,$(LIBCRYPTO))" \
	  && echo "em++ $(LINK_FLAGS) $$objs -o $(patsubst build/out/%,%,$(OUT))/vota_web_wasm.js --preload-file ../../target/fs@/" > $(OUT)/link.cmd \
	  && build/docker-build.sh bash -c "cd build/out && em++ $(LINK_FLAGS) $$objs -o $(patsubst build/out/%,%,$(OUT))/vota_web_wasm.js --preload-file ../../target/fs@/ \
	       && em++ $(LINK_FLAGS) $$objs -o $(patsubst build/out/%,%,$(OUT_NAMED))/vota_web_wasm.js --preload-file ../../target/fs@/ --emit-symbol-map" \
	  && ls -la $(OUT) $(OUT_NAMED) && tools/round/link-symbols.sh $(OUT)

census: $(ROOT)/$(OUT)/vota_web_wasm.wasm
	$(H) tools/round/census.sh

# every std::source_location record the link's app objects hold must be one of the target's (a moved throw site
# fails when a matched function loads it); tools/round/check_srcloc.py
check-srcloc: $(ROOT)/$(OUT)/vota_web_wasm.wasm
	$(H) python3 -I -B tools/round/check_srcloc.py

clean-app:
	$(H) rm -rf build/out/app build/out/wasm build/out/wasm-named

# `make -C src print-APP_CXXFLAGS` → the value, for scripts that must compile with the Makefile's flags
print-%:
	@echo '$($*)'

.PHONY: print-%
