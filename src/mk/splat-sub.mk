# The splat build with substitution: a target function listed in splat/matched.txt is assembled from OUR bytes, the
# body the plain skeleton link produces for that symbol, re-pointed into the target's function, type, table, global
# and data spaces through splat/symbol_addrs.txt and the link's map (build/out/wasm-map). The build fails, naming the
# function and its first differing instruction, when a matched body does not re-point into exactly the target's;
# every other function comes from its piece. The result must be the target, sha256 = the split's source.
#
#   make -C src splat           # link (if stale) → link-map (if stale) → split (if stale) → build/out/splat/vota_web_wasm.wasm + meter
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
SPLAT_INPUTS := $(ROOT)/$(SPLAT_PLAIN) $(ROOT)/$(SPLAT_NAMES) $(ROOT)/$(SPLAT_PIECES)/manifest.json $(ROOT)/$(SPLAT_SYMBOLS)

# link-map.sh refuses to write maps that do not describe the plain link, so a stale names.tsv is always rebuilt here
$(ROOT)/$(SPLAT_NAMES): $(ROOT)/$(SPLAT_PLAIN)
	$(H) tools/round/link-map.sh

.PHONY: splat splat-scan splat-match splat-symbols status stubs stubs-check
splat: $(SPLAT_INPUTS) $(wildcard $(ROOT)/$(SPLAT_MATCHES))
	$(SPLAT_KIT) splat-build $(ROOT)/target/vota_web_wasm.wasm $(ROOT)/$(SPLAT_PLAIN) --matches $(ROOT)/$(SPLAT_MATCHES) -o $(ROOT)/$(SPLAT_WASM)

splat-scan: $(SPLAT_INPUTS)
	$(SPLAT_KIT) splat-scan $(ROOT)/target/vota_web_wasm.wasm $(ROOT)/$(SPLAT_PLAIN) -o $(ROOT)/$(SPLAT_SCAN)
	$(H) python3 -I -B tools/round/status.py
	$(H) python3 -I -B tools/round/stubs.py

splat-match: $(SPLAT_INPUTS)
	$(SPLAT_KIT) splat-scan $(ROOT)/target/vota_web_wasm.wasm $(ROOT)/$(SPLAT_PLAIN) -o $(ROOT)/$(SPLAT_SCAN) --match $(ROOT)/$(SPLAT_MATCHES)
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
