// The compile-setup registry (dataset/setups.json) and its check against the build: every object
// the link takes from a setup's Makefile variables must compile with that setup's flags (make's own
// dry run says how), a library build's recorded flags must be the setup's, and a system library's
// flags must be what emscripten computes. Classifying the objects is the same walk, so the dataset's
// provenance and the drift check cannot disagree.
import { readFileSync } from 'node:fs';

import { effectiveFlags } from './flags';
import { SETUPS_JSON } from './paths';
import type { Lang, Setup } from './schema';
import { artefactFlags, emscriptenFlags, hasImage, makeCommands, makePrint } from './truth';

export function loadSetups(path = SETUPS_JSON): Setup[] {
  const setups = JSON.parse(readFileSync(path, 'utf8')) as Setup[];
  const keys = new Set<string>();
  for (const s of setups) {
    if (keys.has(s.key)) {
      throw new Error(`${path}: setup '${s.key}' twice`);
    }
    keys.add(s.key);
  }
  return setups;
}

export function setupFlags(setup: Setup, lang: Lang): string {
  const f = setup.flags?.[lang];
  if (f === undefined) {
    throw new Error(`setup '${setup.key}' has no ${lang} flags`);
  }
  return f;
}

/** The one language a setup compiles, or null when it has both. */
function onlyLang(setup: Setup): Lang | null {
  const langs = Object.keys(setup.flags ?? {}) as Lang[];
  return langs.length === 1 ? langs[0] : null;
}

/** An object's language from its name (`x.c.o`, `x.cpp.o`), else the setup's only one. */
function langOf(object: string, setup: Setup): Lang {
  if (/\.c\.o$/.test(object)) {
    return 'c';
  }
  if (/\.(cpp|cc|cxx)\.o$/.test(object)) {
    return 'c++';
  }
  return onlyLang(setup) ?? 'c++';
}

export interface ClassifiedObject {
  /** repo-relative; `emsdk:<archive>` for a system library */
  object: string;
  setup: string;
  lang: Lang;
  /** repo-relative source, when the build names it */
  source: string | null;
}

export interface SetupCheck {
  key: string;
  status: 'ok' | 'drift' | 'unverified';
  objects: number;
  detail: string[];
}

export interface Classification {
  objects: ClassifiedObject[];
  /** link inputs no setup claims: hand-written stand-ins for code not reconstructed yet */
  standIns: string[];
  checks: SetupCheck[];
}

export function classifyObjects(setups: readonly Setup[], image: string): Classification {
  const vars = new Set<string>(['LINK_OBJS']);
  for (const s of setups) {
    if (s.truth.kind === 'make' || s.truth.kind === 'artefact') {
      s.truth.vars.forEach((v) => vars.add(v));
    }
  }
  const printed = makePrint([...vars]);
  const objects: ClassifiedObject[] = [];
  const claimed = new Set<string>();
  const checks = new Map<string, SetupCheck>(
    setups.map((s) => [s.key, { key: s.key, status: 'ok', objects: 0, detail: [] }]),
  );
  const drift = (key: string, line: string): void => {
    const c = checks.get(key)!;
    c.status = 'drift';
    c.detail.push(line);
  };

  // make: each object's own compile command decides which of the setups naming its variable it belongs to
  const makeSetups = setups.filter((s) => s.truth.kind === 'make');
  const makeObjects = [
    ...new Set(makeSetups.flatMap((s) => (s.truth as { vars: string[] }).vars.flatMap((v) => printed.get(v)!))),
  ];
  const commands = makeCommands(makeObjects);
  for (const object of makeObjects) {
    const owners = makeSetups.filter((s) =>
      (s.truth as { vars: string[] }).vars.some((v) => printed.get(v)!.includes(object)),
    );
    const cmd = commands.get(object);
    if (cmd === undefined) {
      owners.forEach((s) => drift(s.key, `${object}: make has no compile command for it`));
      continue;
    }
    const actual = effectiveFlags(cmd.flags);
    const fits = owners.filter((s) => s.flags?.[cmd.lang] !== undefined && effectiveFlags(s.flags[cmd.lang]!) === actual);
    if (fits.length !== 1) {
      owners.forEach((s) =>
        drift(
          s.key,
          `${object} compiles with \`${cmd.flags.join(' ')}\` (${cmd.lang}), which ${fits.length === 0 ? 'no setup has' : `${fits.map((f) => f.key).join(' and ')} all have`}`,
        ),
      );
      continue;
    }
    objects.push({ object, setup: fits[0].key, lang: cmd.lang, source: cmd.source });
    claimed.add(object);
  }
  for (const s of makeSetups) {
    if (!objects.some((o) => o.setup === s.key)) {
      drift(s.key, `no object of ${(s.truth as { vars: string[] }).vars.join(', ')} compiles with these flags`);
    }
  }

  // artefact: the variables' objects not claimed above; flags from what the library build recorded
  for (const s of setups) {
    if (s.truth.kind !== 'artefact') {
      continue;
    }
    const truth = s.truth;
    for (const object of truth.vars.flatMap((v) => printed.get(v)!)) {
      if (claimed.has(object)) {
        continue;
      }
      objects.push({ object, setup: s.key, lang: langOf(object, s), source: null });
      claimed.add(object);
    }
    const recorded = artefactFlags(truth);
    if (recorded === null) {
      const c = checks.get(s.key)!;
      c.status = 'unverified';
      c.detail.push(`${truth.file} is absent: the library build has not run in this checkout`);
      continue;
    }
    for (const lang of Object.keys(s.flags ?? {}) as Lang[]) {
      const expect = lang === 'c' ? recorded.filter((t) => !/^-std=(c|gnu)\+\+/.test(t)) : [...recorded, ...(truth.cxxOnly ?? [])];
      if (effectiveFlags(expect) !== effectiveFlags(setupFlags(s, lang))) {
        drift(s.key, `${truth.file} records \`${expect.join(' ')}\` for ${lang}`);
      }
    }
  }

  // emscripten: the image's prebuilt archives, flags as emscripten computes them
  const system = setups.filter((s) => s.truth.kind === 'emscripten');
  const imageUp = system.length > 0 && hasImage(image);
  const computed = imageUp
    ? emscriptenFlags(
        image,
        system.map((s) => (s.truth as { library: string }).library),
      )
    : null;
  for (const s of system) {
    const truth = s.truth as Extract<Setup['truth'], { kind: 'emscripten' }>;
    objects.push({ object: `emsdk:${truth.archive}`, setup: s.key, lang: onlyLang(s) ?? 'c', source: null });
    if (computed === null) {
      const c = checks.get(s.key)!;
      c.status = 'unverified';
      c.detail.push(`no ${image} image to ask emscripten`);
      continue;
    }
    const got = computed.get(truth.library) ?? [];
    for (const lang of Object.keys(s.flags ?? {}) as Lang[]) {
      if (effectiveFlags(got) !== effectiveFlags(setupFlags(s, lang))) {
        drift(s.key, `emscripten builds ${truth.library} with \`${got.join(' ')}\``);
      }
    }
  }

  for (const o of objects) {
    checks.get(o.setup)!.objects++;
  }
  const standIns = printed.get('LINK_OBJS')!.filter((o) => !claimed.has(o));
  return { objects, standIns, checks: [...checks.values()] };
}

