// The blanked-target check of a match (splat-review), on the real modules of this checkout: the target's split
// (build/out/splat/pieces), the plain link with its scratch map and operand-kind table, splat/symbol_addrs.txt and
// splat/matched.txt. urna's docs/reference/splat.md "Reviewing a match" describes the two ways.
import { copyFileSync, existsSync, mkdtempSync, readFileSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { describe, expect, it, vi } from 'vitest';

import { type SplatManifest, readManifest, sha256 } from '../src/splat';
import { defaultKindsPath, readOperandKinds } from '../src/splat-kinds';
import { blankBody, blankSplit, confirmingPart, reviewMatch } from '../src/splat-review';
import { parseMatchList, readPieceBodies, verifyMatches, withMatches } from '../src/splat-sub';
import { runSplatSubCommand } from '../src/splat-sub-cli';
import { type OurSide, type TargetSide, loadOurSide, loadTargetSide, symbolMaps } from '../src/splat-symbols';
import { URNA_ROOT } from './helpers';

const URNA = process.env.URNA_SPLAT_ROOT ?? URNA_ROOT;
const TARGET = `${URNA}/target/vota_web_wasm.wasm`;
const GLUE = `${URNA}/target/vota_web_wasm.js`;
const PLAIN = `${URNA}/build/out/wasm/vota_web/vota_web_wasm.wasm`;
const MAP_DIR = `${URNA}/build/out/wasm-map/vota_web`;
const KINDS = defaultKindsPath(PLAIN);
const SYMBOLS = `${URNA}/splat/symbol_addrs.txt`;
const MATCHED = `${URNA}/splat/matched.txt`;
const PIECES = `${URNA}/build/out/splat/pieces`;
const TARGET_SHA = '8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2';
const real =
  existsSync(`${MAP_DIR}/names.tsv`) &&
  existsSync(KINDS) &&
  existsSync(SYMBOLS) &&
  existsSync(MATCHED) &&
  existsSync(`${PIECES}/manifest.json`);

// 9670 is the __get_deleter of shared_ptr<userdict::correction>(userdict::insertion*); russian_info's has its shape
// and compares another type name
const RUSSIAN_DELETER =
  '_ZNKSt3__220__shared_ptr_pointerIPN7RHVoice12russian_infoENS_10shared_ptrINS1_13language_infoEE27__shared_ptr_default_deleteIS5_S2_EENS_9allocatorIS2_EEE13__get_deleterERKSt9type_info';

let cache: { ours: OurSide; target: TargetSide } | null = null;
function load() {
  if (cache === null) {
    const ours = loadOurSide(PLAIN, MAP_DIR);
    cache = { ours, target: loadTargetSide(TARGET, SYMBOLS, `${URNA}/ledger`, ours.stackSize) };
  }
  return cache;
}

function maps(kinds = true) {
  const { ours, target } = load();
  return symbolMaps(ours, target, kinds ? readOperandKinds(KINDS, ours.module.bytes) : undefined);
}

function matches() {
  return parseMatchList(readFileSync(MATCHED, 'utf8')).matches;
}

/** loadBlankedTarget as splat-review does it: the blanked module beside a copy of the target's glue */
function blankedLoader(stackSize: number) {
  const dir = mkdtempSync(join(tmpdir(), 'splat-review-'));
  return (bytes: Uint8Array): TargetSide => {
    writeFileSync(join(dir, 'vota_web_wasm.wasm'), bytes);
    copyFileSync(GLUE, join(dir, 'vota_web_wasm.js'));
    return loadTargetSide(join(dir, 'vota_web_wasm.wasm'), SYMBOLS, `${URNA}/ledger`, stackSize);
  };
}

describe('the blank', () => {
  it('is a body that parses and holds nothing: no locals, unreachable, end', () => {
    expect([...blankBody(5)]).toEqual([0x00, 0x00, 0x00, 0x00, 0x0b]);
    expect([...blankBody(2)]).toEqual([0x00, 0x0b]);
    expect(() => blankBody(1)).toThrow(/at least 2 bytes/);
  });

  it("replaces one piece in a copy of the split's manifest, with the blank's sha256", () => {
    const f = (index: number, size: number) => ({
      index,
      typeIndex: 0,
      size,
      sha256: 'x',
      path: `asm/g/${index}.bin`,
      name: null,
      group: 'g',
    });
    const manifest = { functions: [f(136, 4), f(137, 9)] } as unknown as SplatManifest;
    const kept = new Uint8Array([1, 2, 3, 4]);
    const { overlay, piece, blank } = blankSplit(manifest, 137, new Map([[136, kept]]));
    expect(piece.index).toBe(137);
    expect(overlay.manifest.functions[1].sha256).toBe(sha256(blankBody(9)));
    expect(overlay.manifest.functions[0]).toEqual(manifest.functions[0]);
    expect(manifest.functions[1].sha256).toBe('x');
    expect(overlay.files.get('asm/g/137.bin')).toEqual(blank);
    expect(overlay.files.get('asm/g/136.bin')).toBe(kept);
    expect(() => blankSplit(manifest, 138)).toThrow(/not a defined function of the split \(136\.\.137\)/);
  });
});

describe('reviewMatch', { timeout: 300_000 }, () => {
  it.skipIf(!real)('9218: both ways give the target, every operand at its first reading', () => {
    const m = maps();
    const r = reviewMatch(m, PIECES, matches(), 9218, blankedLoader(m.ours.stackSize));
    expect(r.expected).toBe(TARGET_SHA);
    expect(r.blanked.sha256).not.toBe(TARGET_SHA);
    expect(r.kindUnverified).toBeUndefined();
    expect(r.own).toMatchObject({ reproduces: true, sha256: TARGET_SHA });
    expect(r.own.bytes!.length).toBe(311);
    expect(r.own.chosen.length).toBeGreaterThan(0);
    expect(r.preferred).toMatchObject({ reproduces: true, sha256: TARGET_SHA, differsAt: null });
    expect(r.needed).toEqual([]);
  });

  it.skipIf(!real)('9218 without operand kinds: the preferred readings are 313 B, and it names what the target picked', () => {
    const m = maps(false);
    const r = reviewMatch(m, PIECES, matches(), 9218, blankedLoader(m.ours.stackSize));
    expect(r.kindUnverified).toBe('no operand-kind table');
    expect(r.own).toMatchObject({ reproduces: true, sha256: TARGET_SHA });
    expect(r.preferred.reproduces).toBe(false);
    expect(r.preferred.bytes!.length).toBe(313);
    expect(r.needed.length).toBeGreaterThan(0);
    for (const n of r.needed) {
      expect(n.reading).not.toBe(n.preferred);
    }
    // the target body read an integer as a table slot, or an address as an integer: it picked the kinds
    expect(r.needed.some((n) => n.preferred === 'literal' && n.reading.startsWith('table slot'))).toBe(true);
  });

  it.skipIf(!real)("12829: the preferred readings fail legitimately, on the table slot copy the target body picked", () => {
    const m = maps();
    const r = reviewMatch(m, PIECES, matches(), 12829, blankedLoader(m.ours.stackSize));
    expect(r.own).toMatchObject({ reproduces: true, sha256: TARGET_SHA });
    expect(r.preferred.reproduces).toBe(false);
    expect(r.preferred.bytes!.length).toBe(134);
    expect(r.needed).toHaveLength(1);
    expect(r.needed[0].instr).toBe('i32.const 2044');
    expect(r.needed[0].reading).toMatch(/^table slot copy 11892 of symbol /);
    expect(r.needed[0].preferred).toMatch(/^table slot copy \d+ of symbol /);
  });

  it.skipIf(!real)('loads the preferred readings\' target side from the blanked module only', () => {
    const m = maps();
    expect(() => reviewMatch(m, PIECES, matches(), 9218, () => m.target)).toThrow(/is not the blanked module/);
    expect(() => reviewMatch(m, PIECES, [], 9218, () => m.target)).toThrow(/9218 is not in the match list/);
  });

  it.skipIf(!real)('confirms a self-voted name through the matches that may use it, not the whole list', () => {
    // 9218's call to DesconverteDataHora is that name's only vote; other matches call it too
    const m = withMatches(maps(), matches());
    const list = matches();
    const match = list.find((x) => x.index === 9218)!;
    const manifest = readManifest(PIECES);
    const bodies = readPieceBodies(PIECES, manifest);
    const alone = verifyMatches(m, manifest, bodies, [match]);
    expect(alone.failures.map((f) => f.message).join('\n')).toMatch(/is UNCONFIRMED: self-voted .*DesconverteDataHora/);
    const part = confirmingPart(m, bodies, list, match);
    expect(part.length).toBeGreaterThan(1);
    expect(part.length).toBeLessThan(list.length);
    const v = verifyMatches(m, manifest, bodies, part);
    // a match of the part may be refused itself, for a name of its own the part leaves alone
    expect(v.failures.filter((f) => f.match.index === 9218)).toEqual([]);
    expect(sha256(v.substitutes.get(9218)!)).toBe(sha256(bodies.get(9218)!));
  });

  it.skipIf(!real)("splat-review fails only when the match's own bytes do not give the target", () => {
    const dir = mkdtempSync(join(tmpdir(), 'splat-review-cli-'));
    const wrong = join(dir, 'wrong.txt');
    writeFileSync(wrong, `9670 ${RUSSIAN_DELETER}\n`);
    const out: string[] = [];
    const spy = vi.spyOn(process.stdout, 'write').mockImplementation((s) => {
      out.push(String(s));
      return true;
    });
    const before = process.exitCode;
    try {
      // a match that needs the target's choice of a copy passes the gate
      runSplatSubCommand('splat-review', [TARGET, PLAIN, '12829', '-o', join(dir, 'a')]);
      expect(process.exitCode).toBe(before);
      expect(out.join('')).toMatch(/^the match's own bytes: REPRODUCES the target, sha256 8579ef8d/m);
      expect(out.join('')).toMatch(/^preferred readings: +DIFFERS, sha256 [0-9a-f]{64} \(134 B against the match's 135 B/m);
      expect(out.join('')).toMatch(/^ {2}legitimately: .* of 1 operand/m);
      expect(out.join('')).toMatch(/^the gate is the match's own bytes: pass/m);
      out.length = 0;
      runSplatSubCommand('splat-review', [TARGET, PLAIN, '9670', '--matches', wrong, '-o', join(dir, 'b')]);
      expect(process.exitCode).toBe(1);
      expect(out.join('')).toMatch(/^the match's own bytes: FAILS: function 9670 .* DIFFERS at \+0xb /m);
      expect(out.join('')).toMatch(/^the gate is the match's own bytes: FAIL/m);
      expect(existsSync(join(dir, 'b', 'vota_web_wasm.js'))).toBe(true);
    } finally {
      spy.mockRestore();
      process.exitCode = before;
    }
  });
});
