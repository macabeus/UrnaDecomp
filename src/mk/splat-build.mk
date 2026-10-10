# The splat build: the target split into pieces (build/out/splat/pieces: asm/<group>/<index>-<name>.bin + .wat, data/,
# manifest.json) and the module re-assembled from them after the toolchain, through the toolkit (WASM_KIT, as in
# tools/round/try-tu.sh). wasm-opt and metadce re-optimise anything that goes through the link, so a decompiled body
# enters here, as a replacement keyed by target function index, never through em++.
#
#   make -C src splat-split                                  # pieces + manifest; fails unless they reassemble to the target
#   make -C src splat-assemble [SPLAT_SUBSTITUTE='<dir|file>…'] # build/out/splat/vota_web_wasm.wasm from the pieces alone
#
# SPLAT_SUBSTITUTE: files named <index>.bin / <index>-<name>.bin holding one function body (the bytes after its size LEB,
# the same form as a piece), or directories of them, root-relative or absolute; each replaces that function's piece.
WASM_KIT ?= $(ROOT)/asmlift-wasm
SPLAT_DIR := build/out/splat
SPLAT_PIECES := $(SPLAT_DIR)/pieces
SPLAT_WASM := $(SPLAT_DIR)/vota_web_wasm.wasm
SPLAT_SUBSTITUTE ?=
SPLAT_KIT = cd $(WASM_KIT) && pnpm --silent exec tsx src/cli.ts

.PHONY: splat-split splat-assemble
splat-split: $(ROOT)/$(SPLAT_PIECES)/manifest.json
$(ROOT)/$(SPLAT_PIECES)/manifest.json: $(ROOT)/target/vota_web_wasm.wasm $(wildcard $(ROOT)/ledger/functions.tsv $(ROOT)/ledger/func-attribution.tsv)
	$(SPLAT_KIT) splat-split $(ROOT)/target/vota_web_wasm.wasm -o $(ROOT)/$(SPLAT_PIECES)

splat-assemble: $(ROOT)/$(SPLAT_PIECES)/manifest.json
	$(SPLAT_KIT) splat-assemble $(ROOT)/$(SPLAT_PIECES) -o $(ROOT)/$(SPLAT_WASM) \
	  $(foreach s,$(SPLAT_SUBSTITUTE),--substitute $(if $(filter /%,$(s)),$(s),$(ROOT)/$(s)))
