# ecourna CEPESC envelope classes + the ASN.1 key loader (docs/reference/facts/app-flags.md §F2:
# these bodies only reach the target's shape with -fno-strict-aliasing in ECOURNA_CXXFLAGS).
ECOURNA_SRCS += api/security/cepesc/cinfosalt.cpp api/security/cepesc/ccipheredout.cpp api/security/cepesc/ccipheredin.cpp \
                api/security/cepesc/cplaintext.cpp api/security/asn1/ckeyloader.cpp
# the units' bodies have no caller in the skeleton yet: 05-ecourna-api-security-cepesc-roots.cpp keeps them the way the target reaches them
# (address-taken / two direct callers of the CInfoSalt ctor); compiled as uenux2 code like src/app/roots_ecourna.cpp
$(ROOT)/$(APP)/05-ecourna-api-security-cepesc-roots.o: $(ROOT)/src/units/05-ecourna-api-security-cepesc-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/05-ecourna-api-security-cepesc-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/05-ecourna-api-security-cepesc-roots.o
