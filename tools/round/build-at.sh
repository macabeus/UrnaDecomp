#!/usr/bin/env bash
# Prepare a worktree of an earlier (or any) commit that `make -C src splat` can build, reusing this checkout's build.
#
#   tools/round/build-at.sh <commit> <dir>        # then: TMPDIR=/tmp make -C <dir>/src splat
#
# A plain `git worktree add` (or a `git archive`) of a commit cannot build: it lacks every ignored input (third_party/*,
# target/fs/, the node_modules of asmlift-wasm and its packages, the library outputs under build/out/), and every file
# gets the time of the checkout, newer than any object, so the first link would recompile everything. This script:
#   1. adds a detached worktree of <commit> at <dir> (git worktree list shows it; remove it with git worktree remove);
#   2. gives each tracked file this checkout's mtime, then the time of now to every file that differs between <commit>
#      and this checkout's working tree (uncommitted edits included, as the build outputs are), so make rebuilds
#      exactly what depends on them, headers included (each unit's rule depends on every header of its tree);
#   3. clones every ignored input of this checkout into <dir> with its timestamps (APFS clones: no extra disk), as
#      Stage 0 of .claude/commands/decompile-functions.md does for each worktree.
# It ends with `make -n -C <dir>/src link`, which lists what the build will recompile. Run it from a checkout whose
# build is current (`make -n -C src link` says "Nothing to be done"), or step 2 rebuilds less than it should.
set -euo pipefail
[ $# -eq 2 ] || { sed -n '2,4p' "$0" | sed 's/^# \{0,1\}//' >&2; exit 2; }
COMMIT=$1 DIR=$2
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
cd "$ROOT"
rev=$(git rev-parse --verify "$COMMIT^{commit}")
[ ! -e "$DIR" ] || { echo "$DIR exists: give a new directory" >&2; exit 2; }
git worktree add -q --detach "$DIR" "$rev"
W=$(cd "$DIR" && pwd)

# 2. this checkout's mtimes, then now for what differs
git ls-files -z | while IFS= read -r -d '' f; do [ -e "$W/$f" ] && touch -h -r "$f" "$W/$f"; done
changed=$(git diff --name-only "$rev" | wc -l | tr -d ' ')
git diff -z --name-only "$rev" | while IFS= read -r -d '' f; do [ -e "$W/$f" ] && touch -h "$W/$f"; done

# 3. the ignored inputs, cloned with their timestamps
git ls-files -z --others --ignored --exclude-standard --directory | while IFS= read -r -d '' p; do
  p=${p%/}
  case "$p" in *.DS_Store|*__pycache__) continue ;; esac
  [ -e "$W/$p" ] && continue
  mkdir -p "$W/$(dirname "$p")"
  if [ "$(uname)" = Darwin ]; then cp -cpR "$p" "$W/$p"; else cp -a --reflink=auto "$p" "$W/$p"; fi
done

n=$(make -n -C "$W/src" link 2>/dev/null | grep -cE 'docker-build\.sh (em\+\+|emcc) .* -c ' || true)
echo "$W: $rev; $changed files differ from this checkout; the link will recompile $n objects"
echo "next: TMPDIR=/tmp make -C $DIR/src splat"
