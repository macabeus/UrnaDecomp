// The committed rows against today's repo. The dataset is frozen (selection.ts), so the rows are not
// re-derived; each one must still hold: its target function, its setup and that setup's flags as the
// build uses them, the object defining its symbol, and its reference. Every check returns one line per
// broken row, `<id>: <reason>`.
import { existsSync } from 'node:fs';
import { join } from 'node:path';

import { type Module, funcAt } from '@asmlift/wasm';

import { URNA_ROOT } from './paths';
import type { Definitions } from './provenance';
import type { DatasetRow, Reference, Setup } from './schema';
import type { SetupCheck } from './setups';
import { inImage } from './truth';

/** Each row's function is a defined function of the target with the size the row records. */
export function targetProblems(rows: readonly DatasetRow[], m: Module): string[] {
  return rows.flatMap((r) => {
    const fn = funcAt(m, r.index);
    if (fn === null) {
      return [`${r.id}: ${r.index} is not a defined function of the target`];
    }
    return fn.size === r.size ? [] : [`${r.id}: the target function is ${fn.size} bytes, the row says ${r.size}`];
  });
}

/** Each row's setup (and a Boost row's instantiating setup) is in the registry with the flags the row
 *  records, and the registry check finds no drift between that setup and the build. */
export function setupProblems(rows: readonly DatasetRow[], setups: readonly Setup[], checks: readonly SetupCheck[]): string[] {
  return rows.flatMap((r) => {
    const setup = setups.find((s) => s.key === r.setup);
    if (setup === undefined) {
      return [`${r.id}: setup '${r.setup}' is not in setups.json`];
    }
    const flagsFrom = r.instantiatedBy === undefined ? setup : setups.find((s) => s.key === r.instantiatedBy);
    if (flagsFrom === undefined) {
      return [`${r.id}: instantiating setup '${r.instantiatedBy}' is not in setups.json`];
    }
    const flags = flagsFrom.flags?.[r.lang];
    if (flags !== r.flags) {
      return [`${r.id}: setup '${flagsFrom.key}' compiles ${r.lang} with \`${flags ?? 'nothing'}\`, the row says \`${r.flags}\``];
    }
    const check = checks.find((c) => c.key === flagsFrom.key);
    return check?.status === 'drift' ? [`${r.id}: setup '${flagsFrom.key}' drifted from the build: ${check.detail.join('; ')}`] : [];
  });
}

/** Each symbol row's object still defines its symbol, and that object is still its setup's (a Boost
 *  row's: its instantiating setup's). */
export function symbolProblems(rows: readonly DatasetRow[], d: Definitions): string[] {
  const byObject = new Map<string, Map<string, string | null>>();
  for (const defs of Object.values(d.symbols)) {
    for (const def of defs) {
      const object = def.member === undefined ? def.object : `${def.object}:${def.member}`;
      const setup = def.setup ?? d.standInSetups[def.object] ?? null;
      (byObject.get(object) ?? byObject.set(object, new Map()).get(object)!).set(def.mangled, setup);
    }
  }
  return rows.flatMap((r) => {
    if (r.evidence !== 'symbol') {
      return [];
    }
    const symbols = byObject.get(r.object!);
    if (symbols === undefined) {
      return [`${r.id}: ${r.object} defines no function in today's build (not a link input, or not built)`];
    }
    if (!symbols.has(r.symbol!)) {
      return [`${r.id}: ${r.object} no longer defines ${r.symbol}`];
    }
    const want = r.instantiatedBy ?? r.setup;
    const got = symbols.get(r.symbol!);
    return got === want ? [] : [`${r.id}: ${r.object} now compiles under setup '${got ?? 'none (a stand-in)'}', the row says '${want}'`];
  });
}

/** Where a reference lives: a file git tracks, a fetched or built tree (`third_party/`, `build/`), or
 *  the pinned image. */
export function referenceHome(ref: Reference): 'tracked' | 'tree' | 'image' {
  if (ref.path.startsWith('emsdk:')) {
    return 'image';
  }
  return /^(third_party|build)\//.test(ref.path) ? 'tree' : 'tracked';
}

/** The fetched or built trees the rows' references point into (`third_party/<lib>`, `build/out/<lib>`). */
export function referenceTrees(rows: readonly DatasetRow[]): string[] {
  const trees = rows.flatMap((r) =>
    r.reference !== null && referenceHome(r.reference) === 'tree'
      ? [r.reference.path.split('/').slice(0, r.reference.path.startsWith('build/') ? 3 : 2).join('/')]
      : [],
  );
  return [...new Set(trees)].sort();
}

/** Each reference that lives in the checkout (`tracked` or `tree`) is there. */
export function referenceProblems(rows: readonly DatasetRow[], home: 'tracked' | 'tree'): string[] {
  return rows.flatMap((r) =>
    r.reference === null || referenceHome(r.reference) !== home || existsSync(join(URNA_ROOT, r.reference.path))
      ? []
      : [`${r.id}: reference ${r.reference.path} is not there`],
  );
}

/** Each system-library reference is a file in the image. */
export function imageReferenceProblems(rows: readonly DatasetRow[], image: string): string[] {
  const refs = rows.filter((r) => r.reference !== null && referenceHome(r.reference) === 'image');
  const paths = [...new Set(refs.map((r) => r.reference!.path.slice('emsdk:'.length)))];
  const missing = new Set(
    inImage(image, `for p in ${paths.map((p) => `'${p}'`).join(' ')}; do [ -f "$p" ] || echo "$p"; done`)
      .split('\n')
      .filter((l) => l !== ''),
  );
  return refs
    .filter((r) => missing.has(r.reference!.path.slice('emsdk:'.length)))
    .map((r) => `${r.id}: reference ${r.reference!.path} is not in ${image}`);
}
