// The frozen dataset. rows.json is selected once and then kept: project progress changes the ledger
// and the build, which would make a re-run of selection pick other rows and break the comparison with
// the committed baseline. dataset/selection.json records the dataset's version and exactly what its
// selected rows were selected from: the pinned target; the ledger, the registry and the sources references
// were looked up in, as the commit selection ran at had them (copied into dataset/selection-inputs/); and
// the build's definitions of the names selection looked up (kept beside it, since the build is not in
// git). `recordedContext` puts those inputs back together; `writeSelection` records a deliberate
// re-selection.
import { spawnSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import { existsSync, mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { dirname, join, relative } from 'node:path';

import { parseLedgerTsv } from '@asmlift/wasm';

import { type Context, contextOf, selectRows, selectionNames } from './dataset';
import {
  LEDGER_TSV,
  SELECTION_DEFINITIONS_JSON,
  SELECTION_INPUTS_DIR,
  SELECTION_JSON,
  SETUPS_JSON,
  TARGET,
  URNA_ROOT,
} from './paths';
import type { Definitions } from './provenance';
import { type ReadRepoFile, readCheckout } from './reference';
import type { DatasetRow, Selection, Setup } from './schema';

function git(args: string[]): string | null {
  const r = spawnSync('git', ['-C', URNA_ROOT, ...args], { encoding: 'utf8', maxBuffer: 1 << 28 });
  return r.status === 0 ? r.stdout : null;
}

const repoRel = (abs: string): string => relative(URNA_ROOT, abs);

function sha256(bytes: string | Uint8Array): string {
  return createHash('sha256').update(bytes).digest('hex');
}

const fileSha256 = (path: string): string => sha256(readFileSync(join(URNA_ROOT, path)));

export function readSelection(path = SELECTION_JSON): Selection {
  return JSON.parse(readFileSync(path, 'utf8')) as Selection;
}

/** The id git gives a file of these bytes. */
export function blobId(bytes: Uint8Array): string {
  return createHash('sha1').update(`blob ${bytes.length}\0`).update(bytes).digest('hex');
}

/** Where dataset/selection-inputs/ keeps its copy of repo file `path`. */
export const recordedCopy = (path: string): string => join(SELECTION_INPUTS_DIR, path);

/** The recorded copy of `path`. Throws when it is not there or is not blob `blob`. */
function recordedFile(sel: Selection, path: string, blob: string): Buffer {
  const copy = recordedCopy(path);
  if (!existsSync(copy)) {
    throw new Error(`${repoRel(copy)}, the copy of ${path} dataset version ${sel.version} was selected from, is missing`);
  }
  const bytes = readFileSync(copy);
  if (blobId(bytes) !== blob) {
    throw new Error(`${repoRel(copy)} is blob ${blobId(bytes)}, not the recorded ${blob}`);
  }
  return bytes;
}

/** The definitions snapshot as written: one line per object, per symbol and per map entry, so a
 *  re-selection diffs by name. */
function renderDefinitions(d: Definitions): string {
  const lines = (entries: string[]): string => (entries.length === 0 ? '' : `\n${entries.join(',\n')}\n`);
  return [
    '{',
    `"key": ${JSON.stringify(d.key)},`,
    `"objects": [${lines(d.objects.map((o) => JSON.stringify(o)))}],`,
    `"standIns": ${JSON.stringify(d.standIns)},`,
    `"symbols": {${lines(Object.entries(d.symbols).map(([k, v]) => `${JSON.stringify(k)}: ${JSON.stringify(v)}`))}},`,
    `"memberSources": {${lines(Object.entries(d.memberSources).map(([k, v]) => `${JSON.stringify(k)}: ${JSON.stringify(v)}`))}},`,
    `"standInSetups": ${JSON.stringify(d.standInSetups)}`,
    '}',
    '',
  ].join('\n');
}

/** The part of `ctx`'s definitions selection reads: the definitions of every name it looks up, the
 *  objects, archive members and stand-ins those name. */
export function snapshotDefinitions(ctx: Context): Definitions {
  const symbols: Definitions['symbols'] = {};
  for (const name of [...selectionNames(ctx)].sort()) {
    const defs = ctx.defs.symbols[name];
    if (defs !== undefined) {
      symbols[name] = defs;
    }
  }
  const defs = Object.values(symbols).flat();
  const objects = new Set(defs.map((d) => d.object));
  const members = new Set(defs.map((d) => `${d.object}:${d.member}`));
  const sortedEntries = <T>(rec: Record<string, T>, keep: (k: string) => boolean): Record<string, T> =>
    Object.fromEntries(
      Object.entries(rec)
        .filter(([k]) => keep(k))
        .sort(([a], [b]) => (a < b ? -1 : a > b ? 1 : 0)),
    );
  return {
    key: ctx.defs.key,
    objects: ctx.defs.objects.filter((o) => objects.has(o.object)),
    standIns: ctx.defs.standIns.filter((o) => objects.has(o)),
    symbols,
    memberSources: sortedEntries(ctx.defs.memberSources, (k) => members.has(k)),
    standInSetups: sortedEntries(ctx.defs.standInSetups, (k) => objects.has(k)),
  };
}

/** Link inputs the build has moved since the definitions were read: old path → new path, repo-relative. */
export type Moves = ReadonlyMap<string, string>;

/** `--move <old>=<new>` arguments as Moves. */
export function parseMoves(specs: readonly string[]): Moves {
  return new Map(
    specs.map((spec) => {
      const [from, to, extra] = spec.split('=');
      if (!from || !to || extra !== undefined) {
        throw new Error(`a move is <old path>=<new path>: ${spec}`);
      }
      return [from, to];
    }),
  );
}

/** A row's or a definition's object (`<path>` or `<archive>:<member>`) at its moved path. */
function movedObject(object: string, moves: Moves): string {
  for (const [from, to] of moves) {
    if (object === from || object.startsWith(`${from}:`)) {
      return to + object.slice(from.length);
    }
  }
  return object;
}

/** `d` with the objects `moves` names at their new paths: the same definitions, read from a build that has
 *  since moved those link inputs. */
export function movedDefinitions(d: Definitions, moves: Moves): Definitions {
  const m = (object: string): string => movedObject(object, moves);
  const keys = <T>(rec: Record<string, T>): Record<string, T> => Object.fromEntries(Object.entries(rec).map(([k, v]) => [m(k), v]));
  return {
    key: d.key,
    objects: d.objects.map((o) => ({ ...o, object: m(o.object) })),
    standIns: d.standIns.map(m),
    symbols: Object.fromEntries(Object.entries(d.symbols).map(([name, defs]) => [name, defs.map((x) => ({ ...x, object: m(x.object) }))])),
    memberSources: keys(d.memberSources),
    standInSetups: keys(d.standInSetups),
  };
}

/** The recorded definitions snapshot as `sel` recorded it. Throws when it is not that file. */
function recordedDefinitions(sel: Selection): Definitions {
  const text = readFileSync(join(URNA_ROOT, sel.inputs.definitions.path), 'utf8');
  if (sha256(text) !== sel.inputs.definitions.sha256) {
    throw new Error(`${sel.inputs.definitions.path} is not the snapshot dataset version ${sel.version} recorded`);
  }
  return JSON.parse(text) as Definitions;
}

/** Refuses moves that do not describe the build: an old path the recorded snapshot does not name, a new
 *  path the checkout does not have. */
function checkMoves(d: Definitions, moves: Moves): void {
  for (const [from, to] of moves) {
    if (!d.objects.some((o) => o.object === from)) {
      throw new Error(`${from} is no object of the recorded definitions`);
    }
    if (!existsSync(join(URNA_ROOT, to))) {
      throw new Error(`${to} is not in the checkout: build it first`);
    }
  }
}

/** The inputs `sel` recorded, checked against what it recorded of them. Throws when one is not there
 *  or differs: a changed target, an edited copy or snapshot. References are looked up in the recorded
 *  sources, and in the checkout for every other file (the fetched and built trees, each pinned by its own
 *  revision). With `moves`, the snapshot's objects are taken at their moved paths. */
export function recordedContext(sel: Selection = readSelection(), moves: Moves = new Map()): Context {
  const { inputs } = sel;
  for (const f of [inputs.target, inputs.glue]) {
    if (fileSha256(f.path) !== f.sha256) {
      throw new Error(`${f.path} is not the file dataset version ${sel.version} was selected from`);
    }
  }
  const ledger = parseLedgerTsv(recordedFile(sel, inputs.ledger.path, inputs.ledger.blob).toString('utf8'));
  const setups = JSON.parse(recordedFile(sel, inputs.setups.path, inputs.setups.blob).toString('utf8')) as Setup[];
  const sources = new Map(
    Object.entries(inputs.sources).map(([path, blob]) => [path, recordedFile(sel, path, blob).toString('latin1')]),
  );
  const read: ReadRepoFile = (path) => sources.get(path) ?? readCheckout(path);
  const recorded = recordedDefinitions(sel);
  checkMoves(recorded, moves);
  return contextOf({ ledger, setups, defs: movedDefinitions(recorded, moves), read });
}

/** The committed rows with the references selection gives them today on the inputs `sel` recorded, and with
 *  the objects `moves` names at their new paths. Throws when those inputs no longer select the committed rows,
 *  field for field but `reference` and a moved `object`, and when the dataset has hand-added rows, whose
 *  references come from no recorded input. */
export function rereferencedRows(sel: Selection, committed: readonly DatasetRow[], moves: Moves = new Map()): DatasetRow[] {
  const manual = committed.filter((r) => r.origin === 'manual').map((r) => r.id);
  if (manual.length > 0) {
    throw new Error(`hand-added rows have no recorded inputs to take a reference from: ${manual.join(', ')}`);
  }
  const again = selectRows(recordedContext(sel, moves), sel.per);
  const rest = ({ reference: _, ...r }: DatasetRow): string =>
    JSON.stringify({ ...r, object: r.object === null ? null : movedObject(r.object, moves) });
  if (again.length !== committed.length || again.some((r, i) => rest(r) !== rest(committed[i]))) {
    throw new Error(`the inputs of dataset version ${sel.version} no longer select the committed rows; only a re-selection changes rows`);
  }
  return again;
}

/** Record a references update: selection.json at the next version, its rows those of `previous`; with
 *  `moves`, the definitions snapshot names the moved objects at their new paths, and records the moves. */
export function writeReferencesUpdate(previous: Selection, note: string, moves: Moves = new Map()): Selection {
  let definitions = previous.inputs.definitions;
  if (moves.size > 0) {
    const text = renderDefinitions(movedDefinitions(recordedDefinitions(previous), moves));
    writeFileSync(SELECTION_DEFINITIONS_JSON, text);
    definitions = { ...definitions, sha256: sha256(text), moved: { ...definitions.moved, ...Object.fromEntries(moves) } };
  }
  const sel: Selection = {
    ...previous,
    version: previous.version + 1,
    inputs: { ...previous.inputs, definitions },
    references: { rowsOf: previous.references?.rowsOf ?? previous.version, note },
  };
  writeFileSync(SELECTION_JSON, `${JSON.stringify(sel, null, 2)}\n`);
  return sel;
}

/** The inputs a re-selection reads from the checkout that git must hold at HEAD for the record to be
 *  true: the ledger, the registry and the tracked sources references are looked up in. */
const RECORDED_PATHS = [repoRel(LEDGER_TSV), repoRel(SETUPS_JSON), 'src', 'tools/libs/7zip/sources.txt'];

/** Uncommitted changes to the inputs a re-selection records; it refuses to run over any. */
export function uncommittedInputs(): string[] {
  return (git(['status', '--porcelain', '--', ...RECORDED_PATHS]) ?? '').split('\n').filter((l) => l !== '');
}

/** Record a re-selection from today's inputs (`ctx` from `loadContext` over a clean checkout, `reads` the
 *  repo files selection read through it): the definitions snapshot, a copy of every tracked input, and
 *  selection.json at the next version. */
export function writeSelection(ctx: Context, per: number, previous: Selection | null, reads: ReadonlySet<string>): Selection {
  const head = git(['rev-parse', 'HEAD'])?.trim();
  if (head === undefined) {
    throw new Error('the urna checkout has no HEAD commit to record');
  }
  const snapshot = snapshotDefinitions(ctx);
  const text = renderDefinitions(snapshot);
  writeFileSync(SELECTION_DEFINITIONS_JSON, text);
  rmSync(SELECTION_INPUTS_DIR, { recursive: true, force: true });
  const copy = (path: string): string => {
    const bytes = readFileSync(join(URNA_ROOT, path));
    mkdirSync(dirname(recordedCopy(path)), { recursive: true });
    writeFileSync(recordedCopy(path), bytes);
    return blobId(bytes);
  };
  const tracked = (abs: string): { path: string; blob: string } => ({ path: repoRel(abs), blob: copy(repoRel(abs)) });
  const trackedNow = new Set((git(['ls-files', '-z']) ?? '').split('\0'));
  const sources = Object.fromEntries(
    [...reads]
      .filter((path) => trackedNow.has(path))
      .sort()
      .map((path) => [path, copy(path)]),
  );
  const sel: Selection = {
    version: (previous?.version ?? 0) + 1,
    per,
    commit: head,
    inputs: {
      target: { path: repoRel(TARGET), sha256: fileSha256(repoRel(TARGET)) },
      glue: { path: repoRel(TARGET).replace(/\.wasm$/, '.js'), sha256: fileSha256(repoRel(TARGET).replace(/\.wasm$/, '.js')) },
      ledger: tracked(LEDGER_TSV),
      setups: tracked(SETUPS_JSON),
      sources,
      definitions: {
        path: repoRel(SELECTION_DEFINITIONS_JSON),
        sha256: sha256(text),
        names: Object.keys(snapshot.symbols).length,
        build: head,
      },
    },
  };
  writeFileSync(SELECTION_JSON, `${JSON.stringify(sel, null, 2)}\n`);
  return sel;
}
