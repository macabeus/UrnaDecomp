// The blanked-target check of one match (urna docs/reference/splat.md "Reviewing a match"): blank the matched piece
// in a copy of the split, assemble the copy with our body for it, and compare the module's sha256 with the target's.
// The copy lives in memory (SplitOverlay): the manifest with the blank's sha256, every other piece read from the split.
// Our body comes two ways, each through the maps with every match of the list applied first (withMatches), as
// buildSplat applies them:
//   the match's own bytes   verifyMatches on the real split: the bytes the build substitutes, each ambiguous operand at
//                           the reading the target body picked among ours (the match rule). This is the gate.
//   preferred readings      remapBody's bytes, every operand at its first reading, through a target side loaded from
//                           the blanked module: no target body exists anywhere in that run, so it is stricter than the
//                           match rule. A match whose operands need the target's choice (which copy of the same
//                           content or which table slot of a function, and in a kind-unverified function which kind)
//                           fails it legitimately; the review lists those operands.
import { disassemble, instrText } from './disasm';
import { GLOBAL_BASE } from './normalize';
import {
  type SplatFunction,
  type SplatManifest,
  type SplitOverlay,
  assembleSplit,
  readManifest,
  sha256,
} from './splat';
import { remapBody } from './splat-remap';
import {
  type Chosen,
  type ListedMatch,
  attempt,
  readPieceBodies,
  selfVoted,
  verifyMatches,
  withMatches,
} from './splat-sub';
import { type SymbolMaps, type TargetSide, dataObjectAt, symbolMaps } from './splat-symbols';

/** A function body of `size` bytes that parses and holds nothing: an empty locals vector (0x00), `unreachable`s
 *  (0x00), a final `end` (0x0b). Random bytes would make the module fail to parse. */
export function blankBody(size: number): Uint8Array {
  if (size < 2) {
    throw new Error(`a function body has at least 2 bytes, not ${size}`);
  }
  const b = new Uint8Array(size);
  b[size - 1] = 0x0b;
  return b;
}

/** The split `manifest` describes with function `index`'s piece blanked (blankBody) and the blank's sha256 in its
 *  manifest. `bodies` (readPieceBodies) are held in the overlay too, so that an assembly reads no function piece again. */
export function blankSplit(
  manifest: SplatManifest,
  index: number,
  bodies: ReadonlyMap<number, Uint8Array> = new Map(),
): { overlay: SplitOverlay; piece: SplatFunction; blank: Uint8Array } {
  const piece = manifest.functions.find((f) => f.index === index);
  if (piece === undefined) {
    throw new Error(
      `function ${index} is not a defined function of the split (${manifest.functions[0]?.index}..${manifest.functions.at(-1)?.index})`,
    );
  }
  const blank = blankBody(piece.size);
  const functions = manifest.functions.map((f) => (f.index === index ? { ...f, sha256: sha256(blank) } : f));
  const files = new Map<string, Uint8Array>();
  for (const f of manifest.functions) {
    const b = bodies.get(f.index);
    if (b !== undefined) {
      files.set(f.path, b);
    }
  }
  files.set(piece.path, blank);
  return { overlay: { manifest: { ...manifest, functions }, files }, piece, blank };
}

export interface ReviewWay {
  /** our body for the blanked function; null when our side gives none (`why`) */
  bytes: Uint8Array | null;
  why?: string;
  /** the blanked split assembled with `bytes` */
  sha256: string | null;
  /** that module is the target's sha256 */
  reproduces: boolean;
}

/** An operand where the target body picked a reading other than the first one our side offers. */
export interface NeededReading {
  /** body offset of the operand's bytes in the target's piece (Chosen.at) */
  at: number;
  /** body offset of our instruction */
  oursAt: number;
  /** our instruction there */
  instr: string;
  reading: string;
  /** the operand's first reading: the one the preferred-readings way writes */
  preferred: string;
}

export interface MatchReview {
  match: ListedMatch;
  /** our function, undefined when our link has none of that name */
  ours: number | undefined;
  piece: SplatFunction;
  /** the split's source: the target's sha256 */
  expected: string;
  /** the blanked split, assembled without our body */
  blanked: { bytes: Uint8Array; sha256: string };
  /** why our function's operand kinds are not known, when they are not (every ambiguous operand keeps every reading) */
  kindUnverified?: string;
  /** with the readings the target body picked for each ambiguous operand (empty when it gives no body) */
  own: ReviewWay & { chosen: Chosen[] };
  preferred: ReviewWay & { differsAt: number | null };
  needed: NeededReading[];
}

/** What our function `ours` may name: `f:<our function>` for its callees, ref.funcs and the function in every one of
 *  our table slots a constant of it equals, `d:<our data object>` for every object a constant or memarg offset of it
 *  falls in (whether or not the operand is an address or a slot). */
function mayName(maps: SymbolMaps, ours: number): Set<string> {
  const m = maps.ours.module;
  const out = new Set<string>();
  for (const i of disassemble(m.funcs[ours - m.importedFuncCount])) {
    for (const im of i.imms) {
      const f =
        im.kind === 'u32' && im.role === 'func'
          ? im.value
          : im.kind === 's32' && i.opcode === 0x41
            ? maps.ours.elem.get(im.value)
            : undefined;
      if (f !== undefined) {
        out.add(`f:${f}`);
      }
      const v = im.kind === 'memarg' ? im.offset : im.kind === 's32' && i.opcode === 0x41 ? im.value : undefined;
      const d = v === undefined || v < GLOBAL_BASE || v >= maps.ours.dataEnd ? null : dataObjectAt(maps.ours.map.data, v);
      if (d !== null) {
        out.add(`d:${d.key}`);
      }
    }
  }
  return out;
}

/**
 * A part of the match list in which `match` is confirmed when the whole list confirms it, so that verifyMatches need
 * not re-point every match. verifyMatches refuses a match only for a self-voted name (splat-sub.ts selfVoted) that no
 * other confirmed match re-points through, or for a function name, whose function no confirmed match matches; and a
 * match a part of the list confirms, the whole list confirms (more matches only take names off the self-voted ones
 * and add their users). Starting from `match` alone, each round adds the matches that may name a refused name (or
 * match its function), until `match` is confirmed; the whole list when a round adds nothing.
 */
export function confirmingPart(
  maps: SymbolMaps,
  bodies: ReadonlyMap<number, Uint8Array>,
  matches: readonly ListedMatch[],
  match: ListedMatch,
): readonly ListedMatch[] {
  const verified = new Map<number, { chosen: Chosen[]; ours: number } | null>();
  const verify = (m: ListedMatch): { chosen: Chosen[]; ours: number } | null => {
    let x = verified.get(m.index);
    if (x === undefined) {
      const ours = maps.ours.fnByKey.get(m.symbol);
      const body = bodies.get(m.index);
      const a =
        ours === undefined || ours < maps.ours.module.importedFuncCount || body === undefined
          ? null
          : attempt(maps, { ours, symbol: m.symbol, source: 'symbol' }, m.index, body);
      x = a?.verdict === 'match' ? { chosen: a.chosen, ours: ours! } : null;
      verified.set(m.index, x);
    }
    return x;
  };
  let names: Map<number, Set<string>> | undefined;
  const part = new Map([[match.index, match]]);
  for (;;) {
    const confirmed = new Map<number, { chosen: Chosen[]; ours: number }>();
    for (const m of part.values()) {
      const x = verify(m);
      if (x !== null) {
        confirmed.set(m.index, x);
      }
    }
    const refused = selfVoted(maps, confirmed);
    if (!confirmed.has(match.index) || !refused.has(match.index)) {
      return [...part.values()];
    }
    names ??= new Map(
      matches.flatMap((m) => {
        const o = maps.ours.fnByKey.get(m.symbol);
        return o === undefined || o < maps.ours.module.importedFuncCount ? [] : [[m.index, mayName(maps, o)] as const];
      }),
    );
    const before = part.size;
    for (const x of refused.values()) {
      const key = x.bytes === null ? `f:${maps.ours.fnByKey.get(x.symbol)}` : `d:${x.symbol}`;
      const callee = x.bytes === null ? maps.target.symbols.fns.get(x.symbol) : undefined;
      for (const m of matches) {
        if (!part.has(m.index) && (m.index === callee || names.get(m.index)?.has(key))) {
          part.set(m.index, m);
        }
      }
    }
    if (part.size === before) {
      return matches;
    }
  }
}

function firstDifference(a: Uint8Array, b: Uint8Array): number | null {
  const n = Math.min(a.length, b.length);
  for (let k = 0; k < n; k++) {
    if (a[k] !== b[k]) {
      return k;
    }
  }
  return a.length === b.length ? null : n;
}

/**
 * Review the match of target function `index` in `matches` (the match list, splat/matched.txt) both ways.
 * `loadBlankedTarget` loads a target side from the blanked module's bytes (splat-review writes them beside a copy of
 * the target's glue and calls loadTargetSide); its body of `index` must be the blank.
 */
export function reviewMatch(
  maps: SymbolMaps,
  dir: string,
  matches: readonly ListedMatch[],
  index: number,
  loadBlankedTarget: (blankedModule: Uint8Array) => TargetSide,
): MatchReview {
  const match = matches.find((m) => m.index === index);
  if (match === undefined) {
    throw new Error(`function ${index} is not in the match list`);
  }
  const manifest = readManifest(dir);
  const bodies = readPieceBodies(dir, manifest);
  const { overlay, piece, blank } = blankSplit(manifest, index, bodies);
  const expected = manifest.source.sha256;
  const blanked = assembleSplit(dir, new Map(), overlay);
  const ours = maps.ours.fnByKey.get(match.symbol);
  const assemble = (bytes: Uint8Array): { sha256: string; reproduces: boolean } => {
    const r = assembleSplit(dir, new Map([[index, bytes]]), overlay);
    return { sha256: r.sha256, reproduces: r.sha256 === expected };
  };

  // the match's own bytes: what the build substitutes, from verifyMatches on a part of the list that confirms the match
  // when the whole list does (confirmingPart), or on the whole list
  const mapsM = withMatches(maps, matches);
  const refused = (x: ReturnType<typeof verifyMatches>): boolean =>
    x.failures.some((f) => f.match.index === index && f.message.includes(' is UNCONFIRMED: '));
  const part = confirmingPart(mapsM, bodies, matches, match);
  let v = verifyMatches(mapsM, manifest, bodies, part);
  if (refused(v) && part !== matches) {
    v = verifyMatches(mapsM, manifest, bodies, matches);
  }
  const ownBytes = v.substitutes.get(index) ?? null;
  const chosen = v.chosen.get(index) ?? [];
  const own: MatchReview['own'] =
    ownBytes === null
      ? {
          bytes: null,
          why: v.failures.find((f) => f.match.index === index)?.message ?? 'no substitute',
          sha256: null,
          reproduces: false,
          chosen: [],
        }
      : { bytes: ownBytes, ...assemble(ownBytes), chosen };

  // the readings the target body picked that are not an operand's first
  const real = ours === undefined ? null : remapBody(mapsM, ours);
  const needed: NeededReading[] = [];
  if (real?.ok) {
    const m = maps.ours.module;
    const instrs = disassemble(m.funcs[ours! - m.importedFuncCount]);
    for (const c of chosen.filter((x) => !x.preferred)) {
      const p = real.pieces.find((x) => !(x instanceof Uint8Array) && x.at === c.oursAt);
      const i = instrs.find((x) => x.offset === c.oursAt);
      needed.push({
        at: c.at,
        oursAt: c.oursAt,
        instr: i === undefined ? '?' : instrText(i, m),
        reading: c.reading,
        preferred: p === undefined || p instanceof Uint8Array ? '?' : p.readings[0],
      });
    }
  }

  // preferred readings, with the blanked module as the target side
  let preferred: MatchReview['preferred'];
  if (ours === undefined) {
    preferred = { bytes: null, why: `our link has no function ${match.symbol}`, sha256: null, reproduces: false, differsAt: null };
  } else {
    const target = loadBlankedTarget(blanked.bytes);
    const tf = target.module.funcs[index - target.module.importedFuncCount];
    if (tf === undefined || Buffer.compare(Buffer.from(tf.bodyBytes), Buffer.from(blank)) !== 0) {
      throw new Error(`the target side for the preferred readings is not the blanked module: its function ${index} is not the blank`);
    }
    const r = remapBody(withMatches(symbolMaps(maps.ours, target, maps.kinds), matches), ours);
    preferred = r.ok
      ? {
          bytes: r.bytes,
          ...assemble(r.bytes),
          differsAt: ownBytes === null ? null : firstDifference(r.bytes, ownBytes),
        }
      : {
          bytes: null,
          why: r.unresolved.map((u) => `+0x${u.at.toString(16)} ${u.instr}: ${u.reason}`).join('; '),
          sha256: null,
          reproduces: false,
          differsAt: null,
        };
  }

  return {
    match,
    ours,
    piece,
    expected,
    blanked: { bytes: blanked.bytes, sha256: blanked.sha256 },
    ...(real?.ok && real.kindUnverified !== undefined ? { kindUnverified: real.kindUnverified } : {}),
    own,
    preferred,
    needed,
  };
}
