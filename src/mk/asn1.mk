# ASN.1 runtime fork (src/asn1): compiled with its own flags — -Os -fno-exceptions -std=c++17
# (docs/reference/facts/asn1.md §A1, §A2), except the TUs that throw (BufferSeeder.h users), which take
# -fexceptions -fignore-exceptions (throw compiles, no landing pads: the target's BufferSeeder calls __cxa_throw
# directly and no asn1 function calls an invoke_* import). Nothing in the skeleton calls the runtime yet, so a
# generated roots TU (tools/round/gen_try_roots.py, address-taken functions) keeps the objects linked.
# src/Makefile reads this file after the unit fragments: the runtime's objects follow theirs in EXTRA_OBJS.
# EXTRA_OBJS from a `make EXTRA_OBJS=…` command line (try-tu.sh) overrides these additions.

# The generated ASN.1 modules (src/asn1/modules/*.cxx): their theInfo/nameList/fieldInfos records are the DATA every
# converter that builds an entity references (Turno::theInfo, ...). Compiled with the runtime's flags plus the modules'
# include dir; the converters' fragment (src/units/04-ecourna-app-dados-asn.mk) puts them in the link, where the
# records nothing references are garbage-collected.
ASN1MOD_DIR  := $(APP)/asn1/modules
ASN1MOD_SRCS := $(sort $(notdir $(wildcard $(ROOT)/src/asn1/modules/*.cxx)))
ASN1MOD_OBJS := $(addprefix $(ROOT)/$(ASN1MOD_DIR)/,$(ASN1MOD_SRCS:.cxx=.o))
# This rule comes before the runtime's: make 3.81 takes the first pattern that applies, and the runtime's
# $(ASN1_DIR)/%.o matches modules/<name>.o too.
$(ROOT)/$(ASN1MOD_DIR)/%.o: $(ROOT)/src/asn1/modules/%.cxx $(wildcard $(ROOT)/src/asn1/modules/*.h $(ROOT)/src/asn1/*.h) $(ROOT)/src/mk/asn1.mk
	$(H) mkdir -p $(ASN1MOD_DIR) && build/docker-build.sh em++ $(ASN1_CXXFLAGS) -Isrc/asn1/modules -c src/asn1/modules/$*.cxx -o $(R)

ASN1_DIR      := $(APP)/asn1
ASN1_CXXFLAGS := -Os -fno-exceptions -std=c++17 -DNDEBUG -Isrc/asn1
ASN1_SRCS     := $(sort $(notdir $(wildcard $(ROOT)/src/asn1/*.cxx)))
ASN1_OBJS     := $(addprefix $(ROOT)/$(ASN1_DIR)/,$(ASN1_SRCS:.cxx=.o))
ASN1_FLAGS_BERDecoderPartial := -fexceptions -fignore-exceptions

$(ROOT)/$(ASN1_DIR)/%.o: $(ROOT)/src/asn1/%.cxx $(wildcard $(ROOT)/src/asn1/*.h) $(ROOT)/src/mk/asn1.mk
	$(H) mkdir -p $(ASN1_DIR) && build/docker-build.sh em++ $(ASN1_CXXFLAGS) $(ASN1_FLAGS_$*) -c src/asn1/$*.cxx -o $(ASN1_DIR)/$*.o

$(ROOT)/$(ASN1_DIR)/roots_asn1.cpp: $(ASN1_OBJS) $(ROOT)/tools/round/gen_try_roots.py
	$(H) python3 -I tools/round/gen_try_roots.py --exclude=_ZN4ASN16CHOICEC $(ASN1_DIR)/roots_asn1.cpp $(patsubst $(ROOT)/%,%,$(ASN1_OBJS))
$(ROOT)/$(ASN1_DIR)/roots_asn1.o: $(ROOT)/$(ASN1_DIR)/roots_asn1.cpp $(ROOT)/src/Makefile
	$(H) build/docker-build.sh em++ $(APP_CXXFLAGS) -c $(ASN1_DIR)/roots_asn1.cpp -o $(ASN1_DIR)/roots_asn1.o

override EXTRA_OBJS += $(ASN1_OBJS) $(ROOT)/$(ASN1_DIR)/roots_asn1.o
