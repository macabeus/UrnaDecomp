# The splat build with substitution: a target function listed in splat/matched.txt is assembled from OUR bytes, the
# body the plain skeleton link produces for that symbol, re-pointed into the target's function, type, table, global
# and data spaces through splat/symbol_addrs.txt and the link's map (build/out/wasm-map). The build fails, naming the
# function and its first differing instruction, when a matched body does not re-point into exactly the target's;
# every other function comes from its piece. The result must be the target, sha256 = the split's source.
#
#   make -C src splat           # link (if stale) → link-map, link-kinds, split (if stale) → build/out/splat/vota_web_wasm.wasm + meter
#   make -C src link-kinds      # the operand-kind table build/out/wasm-kinds/vota_web/kinds.json (below)
#   make -C src splat-scan      # every target function against our candidate → splat/scan.tsv, then `status` and `stubs`
#   make -C src splat-match     # splat-scan, then append its new matches to splat/matched.txt, then `status` and `stubs`
#   make -C src splat-symbols   # regenerate splat/symbol_addrs.txt from the current link (hand edits below the marker kept)
#   make -C src status          # the progress numbers in docs/status.md, from splat/scan.tsv
#   make -C src stubs           # the comment stub of every TSE function with no match and no code of ours, and the gaps (src/README.md)
#   make -C src stubs-check     # fails on a stub or gap that is missing, extra, stale or out of place (CI runs it)
#
# splat/matched.txt: `<target function index> <our symbol>` per line, `#` comments; the symbol is the name
# build/out/wasm-map/vota_web/names.tsv gives our function (`name@object` when several objects define it).
# splat/scan.tsv is committed: it is the work list (docs/how-to-decompile.md step 1) and the source of the numbers.
SPLAT_PLAIN := build/out/wasm/vota_web/vota_web_wasm.wasm
SPLAT_NAMES := build/out/wasm-map/vota_web/names.tsv
SPLAT_MATCHES := splat/matched.txt
SPLAT_SYMBOLS := splat/symbol_addrs.txt
SPLAT_SCAN := splat/scan.tsv
SPLAT_KINDS_DIR := build/out/wasm-kinds/vota_web
SPLAT_KINDS := $(SPLAT_KINDS_DIR)/kinds.json
SPLAT_INPUTS := $(ROOT)/$(SPLAT_PLAIN) $(ROOT)/$(SPLAT_NAMES) $(ROOT)/$(SPLAT_KINDS) $(ROOT)/$(SPLAT_PIECES)/manifest.json $(ROOT)/$(SPLAT_SYMBOLS)

# link-map.sh refuses to write maps that do not describe the plain link, so a stale names.tsv is always rebuilt here
$(ROOT)/$(SPLAT_NAMES): $(ROOT)/$(SPLAT_PLAIN)
	$(H) tools/round/link-map.sh

# The operand-kind table: which of our constants are addresses, table slots or integers. tools/round/link-kinds.sh
# links the plain link's objects again with the data and the table moved (and names that link's functions); the
# toolkit's splat-kinds compares the two links, aligned by name, and writes kinds.json with the plain wasm's sha256
# (splat-scan and splat-build refuse a table of another link). The relink runs only when the plain link (or its map)
# or the script changed; the table is re-derived when either link or the toolkit (the classifier, the disassembler and
# parser whose offsets key the table, the names reader) changed.
.PHONY: link-kinds
link-kinds: $(ROOT)/$(SPLAT_KINDS)
# (link-kinds.sh checks the relink's inputs against link-map's map of the plain link, hence names.tsv first)
$(ROOT)/$(SPLAT_KINDS_DIR)/names.tsv: $(ROOT)/$(SPLAT_PLAIN) $(ROOT)/$(SPLAT_NAMES) $(ROOT)/tools/round/link-kinds.sh
	$(H) tools/round/link-kinds.sh
$(ROOT)/$(SPLAT_KINDS): $(ROOT)/$(SPLAT_PLAIN) $(ROOT)/$(SPLAT_NAMES) $(ROOT)/$(SPLAT_KINDS_DIR)/names.tsv \
		$(wildcard $(WASM_KIT)/src/*.ts)
	$(SPLAT_KIT) splat-kinds $(ROOT)/$(SPLAT_PLAIN) -o $(ROOT)/$(SPLAT_KINDS) $(if $(wildcard $(ROOT)/$(SPLAT_MATCHES)),--matches $(ROOT)/$(SPLAT_MATCHES))

# A kind-unverified match (the table cannot vouch for its constants) is checked with every reading of each constant
# (address, table slot and literal alike), so it proves less than a kind-verified one; the build fails when there are
# more of them than SPLAT_MAX_KIND_UNVERIFIED, so each one is a decision taken here, in review.
SPLAT_MAX_KIND_UNVERIFIED ?= 0
# Set empty (SPLAT_MAX_KIND_UNVERIFIED=), the build lists those matches without failing on them.
SPLAT_LIMITS = $(if $(SPLAT_MAX_KIND_UNVERIFIED),--max-kind-unverified $(SPLAT_MAX_KIND_UNVERIFIED))

.PHONY: splat splat-scan splat-match splat-symbols status stubs stubs-check
splat: $(SPLAT_INPUTS) $(wildcard $(ROOT)/$(SPLAT_MATCHES))
	$(SPLAT_KIT) splat-build $(ROOT)/target/vota_web_wasm.wasm $(ROOT)/$(SPLAT_PLAIN) --matches $(ROOT)/$(SPLAT_MATCHES) --kinds $(ROOT)/$(SPLAT_KINDS) $(SPLAT_LIMITS) -o $(ROOT)/$(SPLAT_WASM)

splat-scan: $(SPLAT_INPUTS)
	$(SPLAT_KIT) splat-scan $(ROOT)/target/vota_web_wasm.wasm $(ROOT)/$(SPLAT_PLAIN) --kinds $(ROOT)/$(SPLAT_KINDS) -o $(ROOT)/$(SPLAT_SCAN)
	$(H) python3 -I -B tools/round/status.py
	$(H) python3 -I -B tools/round/stubs.py

splat-match: $(SPLAT_INPUTS)
	$(SPLAT_KIT) splat-scan $(ROOT)/target/vota_web_wasm.wasm $(ROOT)/$(SPLAT_PLAIN) --kinds $(ROOT)/$(SPLAT_KINDS) -o $(ROOT)/$(SPLAT_SCAN) --match $(ROOT)/$(SPLAT_MATCHES)
	$(H) python3 -I -B tools/round/status.py
	$(H) python3 -I -B tools/round/stubs.py

status:
	$(H) python3 -I -B tools/round/status.py

stubs:
	$(H) python3 -I -B tools/round/stubs.py

stubs-check:
	$(H) python3 -I -B tools/round/stubs.py --check

splat-symbols: $(ROOT)/$(SPLAT_PLAIN) $(ROOT)/$(SPLAT_NAMES)
	$(SPLAT_KIT) splat-symbols $(ROOT)/target/vota_web_wasm.wasm $(ROOT)/$(SPLAT_PLAIN) -o $(ROOT)/$(SPLAT_SYMBOLS)
