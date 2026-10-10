# target/ — the module this project rebuilds

These files are the TSE (Tribunal Superior Eleitoral) voting simulator, the "Simulador de votação" published at
<https://www.justicaeleitoral.jus.br/simulador-votacao/>. The simulator's host page (`vota-wasm.html`) loads
`wasm/vota_web_wasm.js`, which loads `wasm/vota_web_wasm.wasm` and `wasm/vota_web_wasm.data`. They are owned by
the TSE and are kept here unchanged so that every build can be compared against them; the README's notice says
how they are used.

## The files

| file | bytes | sha256 | what it is |
| --- | ---: | --- | --- |
| `vota_web_wasm.wasm` | 7,713,699 | `8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2` | the WebAssembly module: 14,343 defined functions, 136 imports (module `a`, minified names), no custom sections (no `name`, no `producers`). This is the file the splat build must reproduce byte for byte. |
| `vota_web_wasm.js` | 141,497 | `31e985a01b80449c78c726e4070355bd9875c7a2df7f6fac8b69c61f49248754` | the Emscripten glue: runtime, the app's JS library, and the `file_packager` metadata of the `.data` payload |
| `vota_web_wasm.data` | 14,855,585 | `fbb19714507f63063a4b45773699297d7925e666e1eef407d9403b733457da4c` | the `file_packager` payload preloaded into the module's filesystem: 70 files (12 sign-language GIF avatars, 54 UI images, one font, 3 empty `/etc` files) |
| `manifest.json` | 5,150 | `6dc689e4e08d89b3c102e8847528534712fe72d44aacd3967c38e8754561f882` | the scenario list (`generatedAt` 2026-09-10T18:56:04): 8 training scenarios (general elections, general elections in the Federal District, voting abroad, municipal elections; 1st and 2nd round of each), each naming a per-scenario package `<id>.data` + `<id>.data.js` |
| `SHA256SUMS` | | | the four sums above; check with `shasum -a 256 -c target/SHA256SUMS` from the repository root |
| `fs-manifest.tsv` | | | ours, not TSE's: path, size and sha256 of each file in the `.data` payload, written by `node tools/setup/unpack_fs.mjs target/vota_web_wasm.js target/vota_web_wasm.data target/fs target/fs-manifest.tsv` |
| `fs/` | | | ours, gitignored: the unpacked payload, written by the same command |

The per-scenario packages that `manifest.json` names are served under `wasm/bases/` and are not kept here: they
are runtime election data loaded by the page, not part of the module's bytes. The server has no symbol map and no
source map (`wasm/vota_web_wasm.symbols` and `wasm/vota_web_wasm.wasm.map` answer 404 to the browser).

## Where they come from

All four files were downloaded through a desktop Chrome on 2026-10-07: the `.wasm` at 01:19:55 UTC, the `.js`,
`manifest.json` and `.data` between 03:18:43 and 03:18:46 UTC (the download times macOS recorded on the files).
They are committed unchanged.

The server is behind Akamai and refuses non-browser clients: a plain `curl` of any path under
`/simulador-votacao/`, including paths that exist, gets HTTP 403 "Access Denied". A 403 therefore says nothing
about whether a file exists. To fetch a new copy, open the simulator in a browser and save the files from its
`wasm/` folder.

If the TSE publishes a new build, the sha256 above stops matching what the page serves. This project rebuilds
the copy recorded here, not whatever the server holds today.
