# Running several agents on this repository

Rules for a run in which several agents work on urna at once, coordinated through a message board. Each rule is
here because breaking it costs a run time or gives a wrong number. A single person or a single agent working alone
needs only "House rules" and "Shared build outputs".

## The message board

The board is a folder **outside** the repository, created for one run; the run's brief names it. It has:

| folder | holds |
| --- | --- |
| `board/facts/` | what is true of the repo, the target or a tool, with the command that shows it |
| `board/traps/` | commands or files that give a plausible wrong answer |
| `board/rounds/` | a stage's claim (the paths it owns) and its result |
| `queue/` | requests to the coordinator (blocked on something outside one's area, a question the board does not answer); the reply is `<same name>.reply.md` |
| `inbox/` | the user's notes; the coordinator acknowledges each in `inbox/ack/` |

- A post is `<stamp>-<HANDLE>-<slug>.md`, with `<stamp>` the output of `date -u +%Y%m%d-%H%M%SZ` **run as a
  command**. It opens with `from:`, `date:`, `tags:`, and `corrects: <file>` when it corrects another post.
- Posts are append-only: never edit or delete one; correct it with a new post. Before acting on a post, look for
  posts that correct it (`grep -rl "corrects: <filename>" board/`).
- Append one line per post to `INDEX.md` with `printf '%s\n' "<path>  <hook>" >> INDEX.md`, nothing else.
- A number is posted with the command that produced it, or not at all. Post what you learned, not status.
- The inbox is the user's word and outranks every brief. Every agent reads it at the start of each stage; only
  the user (or the coordinator transcribing the user, marked as such) writes there.
- The board is not part of the repository and does not outlive the run. Before a run ends, move every fact a
  tracked file depends on into `docs/reference/facts/` and cite it there: code and docs never cite a board post.

## Working in one shared checkout

- **Claim before editing.** Post a `rounds/` claim listing the files or directories you own. Do not edit a path
  another active stage claimed; file a `queue/` request instead.
- **Commit only your own paths**: `git add <paths> && git commit -m "…" -- <paths>`. The `-- <paths>` keeps other
  agents' staged changes out of your commit.
- If `.git/index.lock` exists, wait and retry. Remove it only when it is older than ten minutes and no git process
  runs.
- Never `git stash` (the stash is shared with every checkout of the repository); use a WIP commit. Never push
  unless the user asks.
- A fresh `git worktree add` gives every tracked file today's mtime, so the first link recompiles everything.
  Copy the mtimes from a clean checkout at the same commit
  (`git ls-files -z | while IFS= read -r -d '' f; do touch -h -r "$f" "<new>/$f"; done`), then
  `make -n -C src link` should report nothing to do.

## Shared build outputs

- `build/out/` is shared. Every link rewrites `build/out/wasm/vota_web/vota_web_wasm.wasm`; freeze a copy of the
  module before deriving a table or an anchor file from it.
- Measure a flag set in a **private link**: `make -C src link APP=build/out/<name>/app OUT=… OUT_NAMED=…`
  (`APP` moves every app object, so the first private link rebuilds them all). Never write the shared
  `build/out/app/…` objects with experimental flags: an object rule depends on the Makefile's mtime, not on the
  flag value, so the object would survive into everyone's link. Add the flag with `UENUX2_EXTRA=…` (or
  `ECOURNA_EXTRA=…`), never a command-line `UENUX2_CXXFLAGS`, which drops the fragments' target-specific additions
  and reaches the stand-ins. [facts/app-flags.md](reference/facts/app-flags.md) §F9 item 6 has the command and the
  splat check of such a link.
- A flag change in a `src/units/*.mk` fragment does not rebuild stale objects: touch the sources. An edit of a
  source or a header does: each object depends on its own source and on every header of its tree.
- Two units landing in one run each need their own fragment number: the coordinator assigns them before the
  matchers start, and the unit that lands second re-runs link, census, symbols, match and the splat build on top of
  the first (`docs/how-to-decompile.md` step 10, "Landing onto a moved main").
- An edit to a shared header recompiles every unit that includes it, and a half-finished edit breaks every other
  agent's link until it is fixed. Keep shared headers compiling at every commit.
- Every agent's link recompiles on the same machine. With a dozen agents the load average passes 130 and one
  compile of eight objects takes six minutes; size the fan-out to the machine, and prefer a lone compile
  (`tools/round/try-tu.sh --single`) or a library probe for questions that do not need the whole link.
- A splat build or scan runs the toolkit's **working tree** (`asmlift-wasm/`). If another agent is editing the
  toolkit, a measurement can depend on code nobody committed; measure against a committed tree (`git archive
  <commit> asmlift-wasm | tar -x -C <dir>`) when that matters.
- To build an earlier commit, run `tools/round/build-at.sh <commit> <dir>` from a checkout whose build is current,
  then `TMPDIR=/tmp make -C <dir>/src splat`. A bare `git worktree add` or `git archive` of the commit cannot build:
  it lacks every ignored input (`third_party/*`, `target/fs/`, `asmlift-wasm/node_modules/`, the library outputs
  under `build/out/`; without node_modules the split fails right after `splat-split` with no message), and every
  file gets a time newer than any object, so the first link recompiles everything. The script clones those inputs
  in, copies this checkout's mtimes, and touches the files the commit has differently, so make rebuilds exactly
  what depends on them; it prints how many objects the link will recompile. Remove the tree with
  `git worktree remove <dir>`.
- `make -C src splat-symbols` after a new link re-picks the arbitrary name of byte-identical twins, so two runs'
  symbol files conflict on such lines: regenerate after merging rather than pick a side. `splat-scan` and
  `splat-match` can disagree on a count; trust the splat build's verification, and read `git diff
  splat/matched.txt` for lines that a data change matched by accident.

## House rules

- Never delete with a wildcard (`rm -f ./*` and the like). Name each file, or move files into a `stale/` folder;
  `git rm <name>` is fine.
- Run docker-backed commands with `TMPDIR=/tmp` on macOS: docker cannot mount the default `/var/folders` temp
  directory.
- Never byte-compare an `--emit-symbol-map` build with the target: the map changes the function order of an -Oz
  link ([facts/link-order.md §L1](reference/facts/link-order.md)).
- Run the Python tools with `python3 -I -B`: `-B` keeps `__pycache__` out of the tree.
- In zsh, pass flag lists as arrays ([facts/tool-traps.md §X3](reference/facts/tool-traps.md)). zsh does not split an
  unquoted `$list` into words, and it expands an unquoted glob in an argument (`--include=*.hpp` fails with "no
  matches found"): quote it.
- In an agent's shell, `grep` can be a shell function that prints nothing for a search over the Latin-1 sources.
  Use `/usr/bin/grep` (or `command grep`) there.
- The official progress check is `TMPDIR=/tmp make -C src splat`: it must print the target's sha256. A stage that
  touches sources, fragments or flags runs `TMPDIR=/tmp make -C src link` and then the splat build before it
  commits.
