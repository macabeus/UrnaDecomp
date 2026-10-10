# Flags every ecourna unit needs (docs/reference/facts/app-flags.md §F2, §F3):
# -fno-strict-aliasing keeps std::vector's exception guard out of line (CInfoSalt ctor 164 -> 75 rows);
# -fno-delete-null-pointer-checks keeps SSO if/else instead of select (9504 118 -> 105, 9497 128 -> 69).
# The include dirs are the libraries the ecourna wrappers include; APP_INCLUDES is defined after this fragment is read.
# -fno-strict-overflow (docs/reference/facts/app-flags.md §F4): closes the pointer-sum residues (9164 9156 9226 9219 9042 9174 9200 9185)
# while the byte-exact 5093/5112 stay exact.
ECOURNA_CXXFLAGS += -fno-strict-aliasing -fno-delete-null-pointer-checks -fno-strict-overflow
# app/dados is non-PIC in the target (docs/reference/facts/app-flags.md §F5); covers the Makefile's own app/dados units too.
$(ROOT)/$(APP)/ecourna/app_dados_%.o: ECOURNA_CXXFLAGS += -fno-PIC
ECOURNA_CXXFLAGS += $(OPENSSL_INCLUDES) -I$(SQLITE_SRC) -I$(BOOST_HDR)
# The generated ASN.1 module headers (src/asn1/modules) are included by name from ecourna TUs
# ("ModuloEnvelopeChave.h"), and each includes its sibling modules by name.
ECOURNA_CXXFLAGS += -Isrc/asn1/modules
