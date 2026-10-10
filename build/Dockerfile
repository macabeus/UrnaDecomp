# Pinned toolchain for reproducing target/vota_web_wasm.wasm byte for byte.
#
# emsdk 6.0.0 = emscripten afa15e0c56d1292e073c2c91bafc1d5e0cdf0dd3
#             + llvm-project c6c2ad52586c28e74a724570985a48dc0022d250 (clang 23.0.0git)
#             + binaryen     b9c72894fba2ee5bef8e4002fb176c1917a556ad
# Pinned by: SQLite's COMPILER=clang-23.0.0 string (emsdk 5.0.0..6.0.3 ship LLVM 23) intersected with
# glue-text markers (CppException-always >= 5.0.5, `programArgs` >= 6.0.0, synchronous run() <= 6.0.0),
# then confirmed by byte-identical dlmalloc/musl bodies at -Oz. See build/README.md.
#
# IMPORTANT: do NOT rebuild the system libraries here (no `embuilder clear`, no `emcc --clear-cache`).
# The prebuilt sysroot shipped in this image already carries the target's libc++abi __FILE__ strings
# (/emsdk/emscripten/system/lib/libcxxabi/...); a rebuild from /emsdk/upstream/emscripten would change them.
FROM emscripten/emsdk:6.0.0

# Host-side inspection tools used by tools/ (wabt: wasm-objdump/wasm2wat). Optional for the build itself.
RUN apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends wabt \
 && rm -rf /var/lib/apt/lists/*

# The TSE build tree lives at these absolute paths (they are baked into __FILE__/__PRETTY_FUNCTION__ strings).
# Mount the reconstructed sources there: -v $PWD/src/uenux2:/home/rubio/tse/uenux2 -v $PWD/src/ecourna:/home/rubio/.conan2/p/b/libecea1da310e5107/b
RUN mkdir -p /home/rubio/tse/uenux2 /home/rubio/.conan2/p/b/libecea1da310e5107/b

WORKDIR /work
