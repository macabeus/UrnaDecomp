#!/bin/bash
# Build the pinned toolchain image and run a command inside it with the current directory mounted at /work.
# usage: build/docker-build.sh                    -> builds the image urna-emsdk:6.0.0
#        build/docker-build.sh <cmd...>           -> runs <cmd> in the image, cwd mounted at /work
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
IMAGE=urna-emsdk:6.0.0
if ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
  docker build -t "$IMAGE" "$HERE"
fi
if [ $# -gt 0 ]; then
  exec docker run --rm -v "$PWD:/work" -w /work "$IMAGE" "$@"
fi
