# ecourna api/security OpenSSL wrappers. The ecourna compile rule has only -Isrc; these TUs include <openssl/*.h>.
ECOURNA_CXXFLAGS += $(OPENSSL_INCLUDES)
ECOURNA_SRCS += api/security/csha.cpp api/security/csiphashmac.cpp api/security/chkdfseed.cpp api/security/ckey.cpp \
    api/security/imacalgorithm.cpp api/security/isymmetriccipher.cpp api/security/caescipher.cpp
# the units' bodies have no caller in the skeleton yet: 11-ecourna-api-security-roots.cpp address-takes them (same mechanism as 05-ecourna-api-security-cepesc-roots.cpp)
$(ROOT)/$(APP)/11-ecourna-api-security-roots.o: $(ROOT)/src/units/11-ecourna-api-security-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/11-ecourna-api-security-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/11-ecourna-api-security-roots.o
