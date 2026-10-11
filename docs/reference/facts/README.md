# Measured facts

The facts this project relies on, each with the measurement or command that establishes it. Code and docs cite
these files for the evidence behind a rule; read the section a rule cites before changing what the rule protects.

Cite a fact as `docs/reference/facts/<file>.md §<id>`, for example `docs/reference/facts/link-order.md §L1`.

| file | ids | what it covers |
| --- | --- | --- |
| [toolchain.md](toolchain.md) | T1–T8 | the emsdk release, the stock image's paths, the link line, EM_JS and the packager, export order, `-DNDEBUG`, comparing bodies across links, candidate compiles |
| [link-order.md](link-order.md) | L1–L7 | the function-order law and its mechanism, the band walk, ordinal recovery, the recovered link line, TU slices |
| [app-flags.md](app-flags.md) | F1–F9 | compile flags of ecourna and uenux2 TUs, per-TU `-fPIC`, source idioms the target forced, flag-measuring traps |
| [libraries.md](libraries.md) | B1–B9 | SQLite, libcrypto, 7-Zip, RHVoice, Boost, zlib: versions, flags, root sets, the compile-diff method |
| [asn1.md](asn1.md) | A0–A4 | the ASN.1 runtime's lineage and flags, the `Modulo*` schema read from the target's data |
| [splat.md](splat.md) | S1–S5 | the splat symbol maps, their coverage, matches that verify but are wrong, and the operand kinds the shifted link gives |
| [tool-traps.md](tool-traps.md) | X1–X5 | exported roots and inlining, probe links, zsh, fetching from the TSE site, callers of `std::to_string` at depth 2 |
