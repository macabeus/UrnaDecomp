# splat symbol maps (docs/reference/facts/splat.md §S1): `make -C src link-map` relinks the plain link's own command (build/out/wasm/vota_web/link.cmd)
# with wasm-ld --Map and kept intermediates into build/out/wasm-map/vota_web/, fails unless that relink is byte-identical
# to the plain wasm, and names every plain function by replaying binaryen (tools/round/link-map.sh, tools/round/splat_names.py).
# The plain link itself is untouched: names.tsv and vota_web_wasm.map describe it because the bytes are the same.
.PHONY: link-map
link-map: $(ROOT)/build/out/wasm/vota_web/vota_web_wasm.wasm
	$(H) tools/round/link-map.sh
