# ecourna api/io (CFile + CIoError/EFileOperation), api/util (CBitArray, CSynchronizer); header-only units that go
# with them: api/pattern/cbasetype.hpp, api/asn/iconversorasn.hpp, api/io/asn/serialization.hpp.
# cbitarray.cpp includes <boost/dynamic_bitset.hpp>, which ECOURNA_CXXFLAGS alone does not find.
ECOURNA_CXXFLAGS += -I$(BOOST_HDR)
ECOURNA_SRCS += api/io/cfile.cpp
ECOURNA_SRCS += api/io/cioerror.cpp
ECOURNA_SRCS += api/util/cbitarray.cpp
ECOURNA_SRCS += api/util/csynchronizer.cpp
# the units' bodies have no caller in the skeleton yet: 03-ecourna-api-io-util-roots.cpp address-takes the functions the target keeps
# (same mechanism as 06-ecourna-api-compression-roots.cpp; the single-caller functions the target inlines are deliberately not rooted)
$(ROOT)/$(APP)/03-ecourna-api-io-util-roots.o: $(ROOT)/src/units/03-ecourna-api-io-util-roots.cpp $(ROOT)/src/Makefile
	$(H) mkdir -p $(APP) && build/docker-build.sh em++ $(APP_CXXFLAGS) -c src/units/03-ecourna-api-io-util-roots.cpp -o $(R)
override EXTRA_OBJS += $(APP)/03-ecourna-api-io-util-roots.o
# The four TUs are compiled WITHOUT -fPIC: with -fPIC the target's CIoError ctor (fn 3520) differs by 21 structural rows
# (its Description table is folded into the load's memarg, i32.load offset=1112652, which only non-PIC codegen gives) and
# RawWrite/RawRead (1886/598) by 9/9 and 18/18 rows (the std::format string_view packing); without -fPIC all three are
# structurally identical (0 inserted/deleted rows). -fPIC is per CMake target, not per package (docs/reference/facts/app-flags.md §F5).
define API_IO_UTIL_NOPIC_RULE
$$(ROOT)/$$(APP)/ecourna/$(subst /,_,$(1)).o: $$(ROOT)/src/ecourna/$(1) $$(ROOT)/src/Makefile $$(ROOT)/src/units/03-ecourna-api-io-util.mk $$(wildcard $$(ROOT)/src/ecourna/api/io/*.hpp $$(ROOT)/src/ecourna/api/util/*.hpp)
	$$(H) mkdir -p $$(APP)/ecourna && build/docker-build.sh em++ $$(filter-out -fPIC,$$(ECOURNA_CXXFLAGS)) -fno-pic -c /work/src/ecourna/$(1) -o $$(R)
endef
$(foreach u,api/io/cfile.cpp api/io/cioerror.cpp api/util/cbitarray.cpp api/util/csynchronizer.cpp,$(eval $(call API_IO_UTIL_NOPIC_RULE,$(u))))
