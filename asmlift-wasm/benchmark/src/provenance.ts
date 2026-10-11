// Which setup compiled a target function. The ledger names a target function after the skeleton
// function it pairs with (byte, fingerprint or addr); the skeleton links exactly the classified
// objects, so the object that defines that name says the setup. Every function symbol of every link
// input is read once with llvm-nm in the pinned image and cached under build/out/bench.
import { createHash } from 'node:crypto';
import { existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { dirname } from 'node:path';

import { DEFINITIONS_JSON } from './paths';
import { effectiveFlags } from './flags';
import type { Lang, Setup } from './schema';
import { type ClassifiedObject, classifyObjects } from './setups';
import { inImage, makeCommands } from './truth';

const SYSROOT = '/emsdk/upstream/emscripten/cache/sysroot/lib/wasm32-emscripten';
const NM = '/emsdk/upstream/bin/llvm-nm';
const DWARFDUMP = '/emsdk/upstream/bin/llvm-dwarfdump';

/** The link's archive order: libcrypto (after the app objects), then emcc's system libraries as the
 *  -Oz -fexceptions link passes them. A symbol two archives define comes from the first. */
const ARCHIVE_ORDER = [
  'build/out/openssl/O3/libcrypto.a',
  'emsdk:libc_optz.a',
  'emsdk:libc.a',
  'emsdk:libdlmalloc.a',
  'emsdk:libcompiler_rt.a',
  'emsdk:libc++.a',
  'emsdk:libc++abi.a',
];

export interface Definition {
  /** repo-relative object or archive (`emsdk:<archive>` for a system library) */
  object: string;
  /** the archive member, for an archive */
  member?: string;
  mangled: string;
  weak: boolean;
  /** null for a stand-in object */
  setup: string | null;
}

export interface Definitions {
  /** what the cache was built from: the registry's text and this file's format */
  key: string;
  objects: ClassifiedObject[];
  standIns: string[];
  /** demangled name → its definitions */
  symbols: Record<string, Definition[]>;
  /** `<archive>:<member>` → the source its debug info names (system libraries only) */
  memberSources: Record<string, string>;
  /** stand-in object → the make setup whose flags it compiles with, if any */
  standInSetups: Record<string, string | null>;
}

function imagePath(object: string): string {
  return object.startsWith('emsdk:') ? `${SYSROOT}/${object.slice(6)}` : object;
}

/** Parse `llvm-nm -A --defined-only` lines into [file, member, type, name]; functions only. */
function nmLines(text: string): ([string, string | undefined, string, string] | null)[] {
  return text.split('\n').map((line) => {
    const m = /^(.*?): [0-9a-f]{8} ([A-Za-z]) (.*)$/.exec(line);
    if (m === null) {
      return null;
    }
    const [file, member] = m[1].split(':');
    return [file, member, m[2], m[3]];
  });
}

export function buildDefinitions(setups: readonly Setup[], image: string): Definitions {
  const { objects, standIns } = classifyObjects(setups, image);
  const setupOf = new Map<string, string | null>(objects.map((o) => [o.object, o.setup]));
  standIns.forEach((o) => setupOf.set(o, null));
  const byImagePath = new Map([...setupOf.keys()].map((o) => [imagePath(o), o]));
  const files = [...byImagePath.keys()].map((f) => `'${f}'`).join(' ');
  const marker = '@@@demangled@@@';
  const out = inImage(
    image,
    `${NM} -A --defined-only ${files} 2>/dev/null; echo '${marker}'; ${NM} -A -C --defined-only ${files} 2>/dev/null`,
  );
  const [plain, demangled] = out.split(`${marker}\n`);
  const a = nmLines(plain);
  const b = nmLines(demangled);
  if (a.length !== b.length) {
    throw new Error(`llvm-nm gave ${a.length} mangled and ${b.length} demangled lines`);
  }
  const symbols: Record<string, Definition[]> = {};
  for (let i = 0; i < a.length; i++) {
    const x = a[i];
    const y = b[i];
    if (x === null || y === null) {
      continue;
    }
    if (x[0] !== y[0] || x[1] !== y[1] || x[2] !== y[2]) {
      throw new Error(`llvm-nm lines ${i} disagree: ${x.join(' ')} / ${y.join(' ')}`);
    }
    const [file, member, type, mangled] = x;
    if (!'TtWw'.includes(type)) {
      continue;
    }
    const object = byImagePath.get(file);
    if (object === undefined) {
      continue;
    }
    const def: Definition = {
      object,
      ...(member !== undefined ? { member } : {}),
      mangled,
      weak: type === 'W' || type === 'w',
      setup: setupOf.get(object) ?? null,
    };
    (symbols[y[3]] ??= []).push(def);
  }
  const archives = objects.filter((o) => o.object.startsWith('emsdk:')).map((o) => imagePath(o.object));
  const dwarf = inImage(image, `cd ${SYSROOT} && ${DWARFDUMP} --debug-info --recurse-depth=0 ${archives.map((f) => `'${f.slice(SYSROOT.length + 1)}'`).join(' ')} 2>/dev/null`);
  const memberSources: Record<string, string> = {};
  let current: string | null = null;
  for (const line of dwarf.split('\n')) {
    const head = /^(\S+\.a)\(([^)]+)\):\s+file format/.exec(line);
    if (head !== null) {
      current = `emsdk:${head[1]}:${head[2]}`;
      continue;
    }
    const name = /DW_AT_name\s+\("([^"]+)"\)/.exec(line);
    if (name !== null && current !== null && memberSources[current] === undefined) {
      memberSources[current] = name[1].replace(/^\/emsdk\/emscripten\//, '/emsdk/upstream/emscripten/');
    }
  }
  const standInCommands = makeCommands(standIns);
  const standInSetups: Record<string, string | null> = {};
  for (const o of standIns) {
    const cmd = standInCommands.get(o);
    const fits =
      cmd === undefined
        ? []
        : setups.filter(
            (s) =>
              (s.truth.kind === 'make' || s.truth.kind === 'stand-ins') &&
              s.flags?.[cmd.lang] !== undefined &&
              effectiveFlags(s.flags[cmd.lang]!) === effectiveFlags(cmd.flags),
          );
    standInSetups[o] = fits.length === 1 ? fits[0].key : null;
  }
  return { key: cacheKey(setups), objects, standIns, symbols, memberSources, standInSetups };
}

const FORMAT = 1;

function cacheKey(setups: readonly Setup[]): string {
  return createHash('sha256').update(`${FORMAT}\n${JSON.stringify(setups)}`).digest('hex');
}

/** The definitions, from the cache when it was built from this registry; `rebuild` reads the build again
 *  (after a rebuild of the objects). */
export function loadDefinitions(setups: readonly Setup[], image: string, rebuild = false): Definitions {
  if (!rebuild && existsSync(DEFINITIONS_JSON)) {
    const cached = JSON.parse(readFileSync(DEFINITIONS_JSON, 'utf8')) as Definitions;
    if (cached.key === cacheKey(setups)) {
      return cached;
    }
  }
  const d = buildDefinitions(setups, image);
  mkdirSync(dirname(DEFINITIONS_JSON), { recursive: true });
  writeFileSync(DEFINITIONS_JSON, JSON.stringify(d));
  return d;
}

export type Resolution =
  | {
      kind: 'resolved';
      setup: string;
      def: Definition;
      /** whether `def` is the only object defining it (several static functions may share a name) */
      unique: boolean;
      lang: Lang;
      instantiatedBy?: string;
      /** a header-only function only a stand-in instantiates: `instantiatedBy` is the setup whose
       *  flags the stand-in compiles with, which says something only where the ledger pairs it exactly */
      viaStandIn?: boolean;
    }
  | { kind: 'unresolved'; reason: string };

/** The setup that compiled the function the skeleton names `name`. Link semantics decide between
 *  several definitions: an object's weak definition shadows an archive's, and of two archives the
 *  first in link order wins. A name a stand-in object also defines, or that two setups' objects
 *  define, is left unresolved: the skeleton's copy might not be the target's. A header-only
 *  library's function is the library's, instantiated by the one setup whose objects define it. */
export function resolveName(d: Definitions, name: string, setups: readonly Setup[]): Resolution {
  let defs = d.symbols[name];
  if (defs === undefined || defs.length === 0) {
    return { kind: 'unresolved', reason: 'no link input defines it' };
  }
  const headerOnly = setups.find(
    (s) => s.truth.kind === 'header-only' && name.startsWith((s.truth as { namespace: string }).namespace),
  );
  // a header-only library's probe stand-in instantiates everything; the setups whose real objects
  // also instantiate a function are the evidence of who compiled it
  if (headerOnly !== undefined && defs.some((x) => x.setup !== null)) {
    defs = defs.filter((x) => x.setup !== null);
  } else if (headerOnly !== undefined) {
    const via = [...new Set(defs.map((x) => d.standInSetups[x.object] ?? null))];
    if (via.length === 1 && via[0] !== null) {
      return {
        kind: 'resolved',
        setup: headerOnly.key,
        def: defs[0],
        unique: defs.length === 1,
        lang: 'c++',
        instantiatedBy: via[0],
        viaStandIn: true,
      };
    }
  }
  if (defs.some((x) => x.setup === null)) {
    return { kind: 'unresolved', reason: 'a stand-in object defines it' };
  }
  const isArchive = (x: Definition): boolean => x.member !== undefined;
  if (defs.some((x) => !isArchive(x))) {
    defs = defs.filter((x) => !isArchive(x));
  } else {
    const first = Math.min(...defs.map((x) => ARCHIVE_ORDER.indexOf(x.object)));
    defs = defs.filter((x) => ARCHIVE_ORDER.indexOf(x.object) === first);
  }
  const keys = [...new Set(defs.map((x) => x.setup!))];
  if (keys.length !== 1) {
    return { kind: 'unresolved', reason: `defined by ${keys.join(' and ')}` };
  }
  const def = defs[0];
  const lang = d.objects.find((o) => o.object === def.object)!.lang;
  return {
    kind: 'resolved',
    setup: headerOnly?.key ?? keys[0],
    def,
    unique: new Set(defs.map((x) => `${x.object}:${x.member ?? ''}`)).size === 1,
    lang,
    ...(headerOnly !== undefined ? { instantiatedBy: keys[0] } : {}),
  };
}
