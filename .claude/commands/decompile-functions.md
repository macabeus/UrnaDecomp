---
description: Run a decompilation round on urna, from picking target functions to landed, independently verified splat matches, with one worktree per lane, a message board and a doc-friction log
argument-hint: <index> [<index> ...] | <count> <component> | <count> tse [agents=<N>] [round=<NAME>]
---

Arguments: **$ARGUMENTS**

- `<index> [<index> …]`: the target function indices to match, one lane each (`9132 9131`).
- `<count> <component>`: how many functions to match from that component (`2 ecourna`); the PICK stage chooses
  them. The component names are the `group` column of `splat/scan.tsv` (`docs/status.md` "By component").
- `<count> tse`: how many functions to match from TSE's own code, the origin "TSE code" of `docs/status.md` "By
  origin" (`vota`, `comum`, `api`, `simulador`, `vota-web`, `ecourna`, `asn1-modules`, `ASN1`). PICK chooses across
  all of them, and the lanes may come from different trees. A request in words ("3 functions from the TSE's own
  code") is this form. The other origins are not: upstream library rows are fixed by flags, roots and link order,
  not by writing C++ (how-to "Library functions").
- `agents=<N>`: how many agents may run at once. **The default is 2.** Raise it only when the user says so.
- `round=<NAME>`: the round's handle, used in post names and agent handles (default `DECOMP<n>`, with `<n>` one
  more than the highest such number among the board folders in `$HOME/.cache/urna-board/`).

If `$ARGUMENTS` names neither indices nor a count with a component or `tse`, ask the user which functions or which
component and how many before doing anything else. Do not choose the component yourself.

**You are the coordinator**: the top-level session. You create the board and the worktrees, launch every stage as
its own subagent, own the urna checkout's `main` and are the only one who talks to the user. This file adds no
decompiling method of its own. How to decompile one function is
[docs/how-to-decompile.md](../../docs/how-to-decompile.md), and how several agents share a checkout and a board is
[docs/agents.md](../../docs/agents.md). Do not restate them in briefs. Link to them, so a docs fix reaches the next
round.

The round has six stages, each run by its own agent:

| stage | agents | where | writes |
| --- | --- | --- | --- |
| 0. board | you | `$HOME/.cache/urna-board/<run-id>` | the board |
| 1. PICK | 1 | the urna checkout, read-only | a pick post |
| 2. MATCH | 1 per lane | the lane's worktree | the lane's branch |
| 3. VERIFY | 1 per lane, not the matcher | the lane's worktree, read-only | a verdict post |
| 4. LAND | 1 | the urna checkout's `main` | merge commits on `main` |
| 5. DOCS | 1 | the urna checkout's `main` | a docs commit |

Never run more than `agents` (default 2) of them at once, counting every stage. Two whole links at once take
about 7 minutes each against one minute alone (how-to step 5), so a third agent mostly makes the other two slower.
A lane's VERIFY may start as soon as its MATCH returns, while the other lane is still matching.

---

## The rules every agent is held to

Put this list in every brief, word for word. It is the standard a match is judged by.

1. **Docs only.** Learn how to decompile only from the repository's documentation, read from your own checkout:
   `README.md`, `docs/getting-started.md`, `docs/how-to-decompile.md`, `docs/glossary.md`, `docs/agents.md`,
   `docs/reference/**`, `src/README.md`, `src/units/README.md`, `tools/README.md`, `asmlift-wasm/README.md`, and
   the code they point to. When a doc is silent, wrong or ambiguous, resolve it the best way you can **and** post a
   friction entry (below).
2. **The exact splat check is the only verdict.** A match counts when `TMPDIR=/tmp make -C src splat` prints the
   target's sha256 `8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2` with it in
   `splat/matched.txt`: byte-identical after re-pointing. No blinding, no census tier, no score.
3. **The real function under its real name.** The body is plausible original C++ for that function: the name the
   evidence gives (records, RTTI, vtable slot), the project's real classes and headers, source types, Latin-1 TSE
   sources, every `std::source_location` throw on its record's line through the existing gap and `#line`
   mechanism.
4. **No lifter scaffolding** in a source that lands: no `volatile` stores, no empty `asm` barriers, no
   `may_alias` casts, no `goto` a loop or an `if` would express (how-to step 3).
5. **Stand-ins add only what the docs sanction.** A `-roots.cpp` reaches a function the way the target does, and
   a `-pending.cpp` holds trapping members, as `src/units/README.md` "Companion files" says. Nothing else
   changes to make a match land.
6. **Generated files are regenerated, never edited**: stubs and gaps, `splat/scan.tsv`, `splat/symbol_addrs.txt`
   above its hand-edit marker, the status blocks of `docs/status.md`, `ledger/functions.tsv` and
   its snapshot. The commands that write them are how-to steps 7 to 9.
7. **House rules** ([docs/agents.md](../../docs/agents.md)): `TMPDIR=/tmp` on every docker command, no wildcard
   `rm`, no `git stash`, `/usr/bin/grep` over the Latin-1 sources, never push. Commit messages end with the
   co-author trailer your session's instructions give.
8. **The board**: read its `README.md` and `inbox/` at the start of your stage (the inbox is the user's word and
   outranks your brief), post a `rounds/` claim of the paths you own before editing, and post what you learned as
   `facts/` and `traps/`, with the command behind every number.

### The friction log

The round exists to make the docs right for the next contributor, so every friction is a deliverable. A friction
is any point where a doc in rule 1 was missing, wrong, ambiguous or misleading. Each one is its own post,
`board/friction/<stamp>-<HANDLE>-<slug>.md`, with:

- **doc**: the path and the section (or line);
- **expected**: what the doc led you to expect;
- **happened**: the command you ran and what it printed;
- **resolved**: what you did instead, and the doc change you suggest.

Post it when you hit it, not at the end of your stage.

---

## Stage 0: the board and the lanes (you)

1. **Check the checkout.** In the urna checkout, `git status --porcelain` must print nothing and
   `make -n -C src link` must say `Nothing to be done`: the lanes clone its build outputs and mtimes, so a dirty
   tree or a stale link is copied into every lane. If either fails, tell the user and stop.
2. **Create the board** at `$HOME/.cache/urna-board/<run-id>-<ROUND>`, where `<run-id>` is the output of
   `date -u +%Y%m%dT%H%M%SZ`. It is outside the repository and never named in a commit or a doc. Print its
   absolute path to the user in your first message: it is their handle on the round if this session dies.
   Create, folders first:

   ```
   board/facts/  board/traps/  board/rounds/  board/friction/  queue/  inbox/ack/
   README.md     INDEX.md
   ```

   `README.md` states the round's purpose, then the board rules of [docs/agents.md](../../docs/agents.md) "The
   message board" and "Working in one shared checkout", and the friction format above. `INDEX.md` starts as one
   header line.
3. **Write the user's request into `inbox/`** as `<stamp>-USER-request.md`, verbatim, marked as transcribed by
   you. Every later word of the user's that changes the round goes there too, before you act on it.
4. **Run PICK** (Stage 1) when the arguments give a component; when they give indices, PICK still checks each one
   (stub, file, callees) and assigns files, but chooses nothing.
5. **Assign each lane a fragment number** before any matcher starts: the next free `NN` after the last
   `src/units/<NN>-*.mk`, one per lane in lane order (A 15, B 16). Two lanes that each take "the next free
   number" collide ([src/units/README.md](../../src/units/README.md) "Names").
6. **Create one worktree per lane**, from the urna checkout's root, branch `decomp/<lane>` from `main`, as a
   sibling directory of the checkout:

   ```sh
   LANE=ecourna-a                                    # one name per lane
   W="$(cd .. && pwd)/urna-worktrees/$LANE"
   git worktree add -b "decomp/$LANE" "$W" main
   # tracked files: copy the checkout's mtimes, or the first link recompiles everything (docs/agents.md)
   git ls-files -z | while IFS= read -r -d '' f; do touch -h -r "$f" "$W/$f"; done
   # every ignored input (build/out, build/dump, build/probe/target.fns.jsonl, third_party/*, target/fs,
   # the node_modules of asmlift-wasm and its packages), cloned with its timestamps
   git ls-files -z --others --ignored --exclude-standard --directory | while IFS= read -r -d '' p; do
     p=${p%/}
     case "$p" in *.DS_Store|*__pycache__) continue ;; esac
     [ -e "$W/$p" ] && continue                      # already inside a directory cloned above
     mkdir -p "$W/$(dirname "$p")"
     cp -cpR "$p" "$W/$p"                            # APFS clone; on Linux: cp -a --reflink=auto
   done
   make -n -C "$W/src" link                          # must print: Nothing to be done for `link'
   ```

   It takes about a minute, and the clones share disk blocks with the checkout (its `build/out/` alone is
   gigabytes). Anything but `Nothing to be done` means the lane would rebuild from scratch: find out why before
   launching its matcher. Do not launch an agent into a worktree another agent is using.

---

## Stage 1: PICK (one agent, read-only)

Brief: the rules above, the board path, the arguments and the lane names. It changes no tracked file. It returns,
and posts as `rounds/<stamp>-<ROUND>-PICK-pick.md`, one target per lane with its evidence, and its frictions.

How to pick, from [how-to-decompile.md](../../docs/how-to-decompile.md) step 1:

- **The pool.** A function not decompiled yet has a stub in its tree, so start from the stubs and join the scan
  (`src/ecourna` for `ecourna`; `src/uenux2` for `vota`, `comum`, `api`, `simulador`, `vota-web`; `src/asn1` for
  `ASN1` and `asn1-modules`; all three for `tse`). Go by the tree that holds the stub, not by the `group` column
  (step 1 explains why they differ):

  ```sh
  python3 -I -B tools/round/pick_pool.py src/ecourna    # src/uenux2, src/asn1, or --tse; index, verdict, size, group, stub
  python3 -I -B tools/round/pick_pool.py --differs      # the TSE differs rows (how-to step 1)
  ```

  The first lists the stubs of 40 to 600 B (`--min`/`--max` change it), smallest first. The `differs` rows of TSE
  code (the second) are the other pool: a body of ours exists and the work is fixing its C++.

  Claude Code replaces a `$` followed by a digit in this file with one of the command's argument words before the
  session reads it (a dollar and 0 become the first word; Claude Code's skills docs, "Available string substitutions"), fenced code
  included, so the file holds no such text, and a query belongs in a script, not here:
  `/usr/bin/grep -nE '\$[0-9]' .claude/commands/decompile-functions.md` must print nothing.
- **Every callee must resolve.** Read each candidate with `pnpm wasm fn` and lift it with `pnpm wasm lift … --check`
  (how-to steps 2 and 3). `fn` does not name the function behind an `invoke_*` table slot; the lift does
  (`/* &fN */`). A callee resolves when it is matched (`/usr/bin/grep -E '^N ' splat/matched.txt` prints a line) or
  named (`/usr/bin/grep -F '= func:N;' splat/symbol_addrs.txt` prints one). A function whose callees are unnamed
  needs those first. The name that resolves can be a *twin*'s: binaryen merges byte-identical bodies and keeps one
  name, so 760, matched as `vector<pair<unsigned long, unsigned long>>::push_back`, is also the `push_back` of
  another vector of an 8-byte element in the target: 11361, `CConversorSituacoesEleicoes`'s converter, which builds a
  `vector<CSituacoesEleicoes>`, calls 760 (`pnpm wasm fn ../target/vota_web_wasm.wasm 11361` shows `call 760`). A
  caller that uses that other instantiation still resolves when its body is the same (how-to step 3, "A callee can
  be a twin").
- **The stub may sit over code we have**: search our sources for the class before choosing (step 1).
- **The name and the file**: from the records, the vtable slot, `ledger/vtables.json` and `ledger/classes.json`;
  for an `unplaced.cpp` stub, the file rules of how-to step 3.
- **Lanes must not overlap.** Different classes and different files, so no two lanes edit one file or emit one
  vtable. Two virtual members of one class belong to one lane (the second can be that lane's stretch goal).

Check the pick yourself before launching the matchers: each target's stub is where the post says, and no two lanes
share a file.

---

## Stage 2: MATCH (one agent per lane)

Brief: the rules above, the board path, the lane's handle (`<ROUND>-A`), the absolute worktree path and branch,
the target index (and any stretch target), its fragment number, and the PICK post's evidence **as hypotheses**.

The matcher works only in its worktree and follows [how-to-decompile.md](../../docs/how-to-decompile.md) steps 1
to 10 as written, including steps 6 to 10 in the worktree: link, census, symbols, match, the splat build, the
checks, and one commit on its branch. It never touches the urna checkout's `main` and never merges.

It returns, and posts as a `rounds/` result: the verdict (matched, or not matchable yet and why), the commit(s), the
splat build's sha256 and match count, the matches added and lost against `main`, the files it owns,
its stretch target's state, and the number of friction posts. A target that cannot be matched yet is a result:
post the blocker as a `facts/` post (which callee, which codegen) and leave it pending. Nothing lands from a lane
without a match.

Count the matches by target index, not by line. `git diff main -- splat/matched.txt` shows the added lines, but a
line removed there is not always a lost match: a twin whose surviving name moved is removed under its old symbol and
added back under the new one. The lost matches are the indices `main` matches and the branch does not, which the
`comm` of [how-to-decompile.md](../../docs/how-to-decompile.md) step 10 ("A twin whose name moved") prints.

---

## Stage 3: VERIFY (one agent per lane, independent)

A different agent from the matcher, given only: the rules above, the board path, the worktree, the branch, the
commit range, the target index and the match line. Not the matcher's transcript. Its job is to refute the match.
It changes no tracked file, works in scratch under `build/out/verify/` (ignored), and posts a verdict,
**CONFIRMED** or **REFUTED** with the check that failed:

1. **Fresh objects.** Move the unit's objects aside by name (into `build/out/verify/old-objs/`), run
   `TMPDIR=/tmp make -C src splat`, and `cmp` the rebuilt objects with the old ones. A stale object means the
   source edits never rebuilt (`src/units/README.md` "What a fragment holds" item 3).
2. **The splat build** prints the target's sha256, and no index `main` matches is lost (how-to step 10's `comm`). A
   line removed and its index added back under another symbol is a twin rename: check that the old symbol is gone
   from the link (`build/out/wasm-map/vota_web/names.tsv`) and the new one is a byte-identical body.
3. **The blanked-target check**, as [docs/reference/splat.md](../../docs/reference/splat.md) "Reviewing a match"
   describes: for each target index the lane matched, from `asmlift-wasm/`,
   `pnpm --silent wasm splat-review ../target/vota_web_wasm.wasm ../build/out/wasm/vota_web/vota_web_wasm.wasm <index> -o ../build/out/verify/<index>`.
   It blanks the matched piece in a copy of the split, assembles it with our body two ways, and prints whether each
   gives the target's sha256. The match's own bytes are the gate: the command exits non-zero only when they fail.
   Report the preferred-readings way too, but a match whose operands need the target's choice (splat.md §3) fails it
   legitimately, and the command lists each reading the target body picked. Say which way gave which result.
4. **The checks**: `make -C src stubs-check`, `python3 -I -B tools/round/status.py --check`,
   `python3 -I -B tools/round/stubs.py --check`, `python3 -I -B tools/round/check_encoding.py`,
   `TMPDIR=/tmp make -C src check-srcloc` with no wrong record of a matched function (compare its list with
   `main`'s; the records `main` already lists are not the lane's), and from `asmlift-wasm/`
   `TMPDIR=/tmp pnpm test:bench`. The last one
   fails when the lane compiles an object with a flag set that no setup of
   `asmlift-wasm/benchmark/dataset/setups.json` has (how-to step 4, "When only a flag closes the gap"); every other
   check passes on such a lane.
5. **Generated files are what the generators write.** Re-running `make -C src splat-symbols` leaves
   `git status` clean. A rescan changes `splat/scan.tsv` in one way only, the `source` column of a new match
   (how-to step 9). Restore anything a re-run rewrote with `git checkout -- <file>`, and report it.
6. **The source**, against rules 3 to 5: the real mangled name and where its evidence comes from, real classes, no
   scaffolding in the landed TSE files, Latin-1, throws on their record lines, and roots and pending files in the
   shapes `src/units/README.md` sanctions.

A REFUTED lane does not land. Send its finding back to the lane's matcher once, as a new MATCH run, then to a new
VERIFY. If it is refuted again, report it to the user.

---

## Stage 4: LAND (one agent, on `main`)

Brief: the rules above, the board path, the CONFIRMED lanes in landing order, and the match count and commit of
`main` before the round. It posts a `rounds/` claim of `main` first, and keeps its logs under `build/out/land/`.

1. **The first lane** fast-forwards when `main` has not moved: `git merge --ff-only decomp/<lane>`. Its matches were
   measured on this `main`, so nothing needs to be rerun.
2. **Each later lane** merges with `git merge --no-ff --no-commit decomp/<lane>` and resolves the conflicts as
   [how-to-decompile.md](../../docs/how-to-decompile.md) step 10 "Landing onto a moved main" says, then runs steps 6
   to 10 again on the merge (link, census, symbols, match, build, checks) and commits it all as the merge commit.
   Do not pick generated lines by hand.
3. **Check the landing**: `TMPDIR=/tmp make -C src splat` prints sha256
   `8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2`, the match count is the old count plus every
   lane's matches, and no index of the old `main` is lost (how-to step 10's `comm`, with `<old main>` for `main`). A
   merged link can add matches no lane had (a name that moved, a vtable the merged link places by content). List
   them, with why, as a `facts/` post. Then
   run the checks of VERIFY step 4, and from `asmlift-wasm/` `pnpm test`, `pnpm typecheck` and `pnpm test:bench`.
4. **Clean up**: for each landed lane whose worktree is clean (`git -C <worktree> status --porcelain` prints
   nothing) and whose tip is in `main` (`git merge-base --is-ancestor decomp/<lane> main`), run
   `git worktree remove <worktree>`. Keep the branches. Never push.

---

## Stage 5: DOCS (one agent, on `main`)

Brief: the rules above, the board path, and the docs it may edit (rule 1's list, the help text of the tools they
describe, and this command file). It reads every `board/friction/` post (and every post that corrects one) and
changes the docs so the next contributor does not hit it. It runs every command it writes into a doc, as written,
before committing.
Facts that an edited doc depends on move into `docs/reference/facts/` (no doc cites the board). A fix that needs code
(a Makefile rule, a tool option) is not made here: it is listed as a follow-up for the user. It checks that the
relative links of every edited doc resolve and that `status.py --check`, `stubs.py --check` and
`check_encoding.py` still pass, commits, and posts its result: the commit, and for each friction post the doc
change that answers it, or why none does.

---

## What to report

When the round ends, or when the user stops it (then stop launching agents, let running ones finish, and report
what exists), tell the user:

- **per target**: the verdict (matched, pending and why, refuted), the VERIFY verdict and which blanked-target
  ways it ran, and the lane's commits;
- **the landing**: how each lane merged (fast-forward or merge commit), the conflicts and how each was resolved,
  the regeneration and how long the link took, the splat build's sha256, the match count before and after, every
  new match including those no lane had, and that none was lost;
- **the checks** and their results;
- **the friction log**: how many posts, and the DOCS commit; the follow-ups that need code;
- **what is left**: the worktrees removed (quote `git worktree list`), the branches kept, that nothing was pushed,
  and the board's path.

---

## Example: `2 ecourna`

- **PICK** starts from the ecourna stubs (`pick_pool.py src/ecourna`). Of the small ones, those whose callees all
  resolve are mostly template instantiations and the `CConversor*` ASN.1 converter hooks. It gives lane A 9132
  `CConversorHorariosUrna::DoDeconverte` (134 B) and lane B 9131 `CConversorMunicipioZona::DoConverte` (54 B), with
  9130, B's sibling slot, as B's stretch goal, and you assign the fragment numbers 15 and 16.
- **MATCH A** writes the `CHorariosUrna` data class (its out-of-line constructor gives the target's 32-byte frame)
  and the converter in `app/dados/asn/`, with a `-pending.cpp` for 9133 and a `-roots.cpp` for the vtable. Its
  scan also matches 9218 (`CConversorCabecalhoEntidade::DoDeconverte`), a body the lane did not write: the match
  found by content confirms a callee's name, which resolves another function. The lane reports both matches.
  **MATCH B** matches 9131. Its stretch goal 9130 stays pending: it needs constructor thunks that binaryen merged and
  that have no names yet, which the lane reports as a `facts/` post.
- **VERIFY** confirms both lanes, the blanked-target check included.
- **LAND** fast-forwards lane A and merges lane B. The merge conflicts in every generated file and in the census
  snapshot both lanes renamed, and is resolved as how-to step 10 "Landing onto a moved main" says. After the
  regeneration the splat build prints the target's sha256 with every lane's matches plus 6122, a libc++ function
  that neither lane had: the merged link drops a self-voted vtable name, and the vtable is then placed by its
  content. No match is lost.
- **DOCS** answers each friction post with a doc change, and lists as follow-ups the fixes that need code (a
  Makefile rule, a tool command).
