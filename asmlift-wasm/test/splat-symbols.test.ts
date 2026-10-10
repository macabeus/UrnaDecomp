// The splat symbol maps on the real modules of this checkout: the target, the plain skeleton link, the scratch
// relink with wasm-ld's map (tools/round/link-map.sh) and the generated splat/symbol_addrs.txt.
import { existsSync, readFileSync } from 'node:fs';
import { describe, expect, it } from 'vitest';

import { disassemble } from '../src/disasm';
import { normalizeModule } from '../src/normalize';
import { parseModule } from '../src/parse';
import { matchesTarget, remapBody } from '../src/splat-remap';
import {
  HAND_EDIT_MARKER,
  SAME_CONTENT_LIMIT,
  cStringAt,
  compareObject,
  dataObjectAt,
  isSourceLocationRecord,
  loadOurSide,
  loadTargetSide,
  memoryImage,
  objectPattern,
  parseLinkMap,
  parseSymbolFile,
  refuteDataNames,
  renderSymbolEntry,
  renderSymbolFile,
  resolveDataAddress,
  resolveRepointed,
  rttiClassAt,
  soleFnVoters,
  soleVoters,
  srclocAnchors,
  srclocKey,
  symbolMaps,
  targetSymbolsOf,
  vtableTypeinfoOffset,
} from '../src/splat-symbols';
import { URNA_ROOT } from './helpers';

const URNA = process.env.URNA_SPLAT_ROOT ?? URNA_ROOT;
const TARGET = `${URNA}/target/vota_web_wasm.wasm`;
const PLAIN = `${URNA}/build/out/wasm/vota_web/vota_web_wasm.wasm`;
const MAP_DIR = `${URNA}/build/out/wasm-map/vota_web`;
const SYMBOLS = `${URNA}/splat/symbol_addrs.txt`;
const TWIN_DIR = `${URNA}/build/out/wasm-named/vota_web`;
const real = existsSync(`${MAP_DIR}/names.tsv`) && existsSync(SYMBOLS);

describe('parseLinkMap', () => {
  it.skipIf(!real)("reads wasm-ld's data symbols, its merged strings and the code entries in link order", () => {
    const map = parseLinkMap(readFileSync(`${MAP_DIR}/vota_web_wasm.map`, 'utf8'));
    const ours = parseModule(new Uint8Array(readFileSync(PLAIN)));
    expect(map.outputs.map((o) => o.name)).toEqual(
      expect.arrayContaining(['CODE', 'DATA', '.rodata', '.data', '.bss']),
    );
    const strings = map.data.find((d) => d.section === '<merged strings>');
    expect(strings).toMatchObject({ addr: 1024, out: '.rodata', object: '<internal>' });
    // every data symbol of the final module sits where wasm-ld put it: wasm-opt does not move data
    const { image } = memoryImage(ours);
    const ti = map.data.find((d) => d.name.startsWith('_ZTS') && d.out === '.rodata')!;
    expect(cStringAt(image, 1024, ti.addr)?.toString('latin1')).toBe(ti.name.slice(4));
    expect(map.functions.length).toBeGreaterThan(ours.funcs.length);
  });
});

describe('loadOurSide', () => {
  it.skipIf(!real)('refuses a map directory whose scratch link is not the plain module', () => {
    expect(() => loadOurSide(PLAIN, TWIN_DIR)).toThrow(/does not describe/);
  });

  it.skipIf(!real)('names every function of the plain link', () => {
    const ours = loadOurSide(PLAIN, MAP_DIR);
    expect(ours.fns.size).toBe(ours.module.funcs.length);
    expect(ours.fnByKey.get('sqlite3ErrorMsg')).toBeDefined();
    expect(ours.dataEnd + ours.stackSize).toBe(Number(ours.module.globals[0].init.split(' ')[1]));
  });
});

describe('remapBody', () => {
  const load = () => {
    const ours = loadOurSide(PLAIN, MAP_DIR);
    const target = loadTargetSide(TARGET, SYMBOLS, `${URNA}/ledger`, ours.stackSize);
    return symbolMaps(ours, target);
  };

  it.skipIf(!real)(
    're-points bodies the census paired by address into the target bodies, ambiguous constants read either way',
    () => {
      const maps = load();
      const tf = (i: number) => maps.target.module.funcs[i - maps.target.module.importedFuncCount];
      for (const name of ['sqlite3ErrorMsg', 'BN_CTX_get']) {
        const o = maps.ours.fnByKey.get(name)!;
        const t = maps.target.symbols.fns.get(name)!;
        const r = remapBody(maps, o);
        expect(r.ok).toBe(true);
        if (r.ok) {
          // BN_CTX_get's 786688 is an integer that falls inside RHVoice's unicode table: only the target decides
          expect(matchesTarget(r.pieces, tf(t).bodyBytes).match).toBe(true);
          expect(r.rewritten.func ?? 0).toBeGreaterThan(0);
        }
      }
    },
  );

  it.skipIf(!real)('reads a constant as a table slot when the target holds a function pointer there', () => {
    const maps = load();
    const o = maps.ours.fnByKey.get('__cxa_allocate_exception')!;
    const t = maps.target.symbols.fns.get('__cxa_allocate_exception')!;
    const r = remapBody(maps, o);
    expect(r.ok).toBe(true);
    if (r.ok) {
      const m = matchesTarget(r.pieces, maps.target.module.funcs[t - maps.target.module.importedFuncCount].bodyBytes);
      expect(m.match).toBe(true);
      expect(m.readings?.some((x) => x.startsWith('table slot'))).toBe(true);
    }
  });

  it.skipIf(!real)('reports a call to an import the target lacks as unresolved, naming it', () => {
    const maps = load();
    const om = maps.ours.module;
    const imports = om.imports.filter((i) => i.kind === 'func');
    const missing = imports.findIndex((i) => !maps.target.importByName.has(maps.ours.importNames[i.name] ?? i.name));
    expect(missing).toBeGreaterThanOrEqual(0);
    const caller = om.funcs.find((f) =>
      disassemble(f).some((i) => i.opcode === 0x10 && (i.imms[0] as { value: number }).value === missing),
    )!;
    const r = remapBody(maps, caller.index);
    expect(r.ok).toBe(false);
    if (!r.ok) {
      expect(r.unresolved.some((u) => /^import \S+ is not imported by the target$/.test(u.reason))).toBe(true);
    }
  });

  it.skipIf(!real)('resolves a string literal by content and a source_location record by what it records', () => {
    const maps = load();
    const strings = maps.ours.map.data.find((d) => d.section === '<merged strings>')!;
    const r = resolveDataAddress(maps.ours, maps.ourImage, maps.target, strings.addr + 0x100);
    if (r.ok) {
      const ours = cStringAt(maps.ourImage, 1024, strings.addr + 0x100)!;
      expect(cStringAt(maps.target.image, 1024, r.addr)!.equals(ours)).toBe(true);
    } else {
      expect(r.reason).toMatch(/string/);
    }
    let resolved = 0;
    let numbered = 0;
    for (const loc of maps.ours.map.data.filter(isSourceLocationRecord)) {
      const l = resolveDataAddress(maps.ours, maps.ourImage, maps.target, loc.addr + 8);
      expect(dataObjectAt(maps.ours.map.data, loc.addr + 15)?.addr).toBe(loc.addr);
      if (l.ok) {
        expect(l.how).toBe('srcloc');
        expect(srclocKey(maps.target.image, 1024, l.addr - 8)).toBe(srclocKey(maps.ourImage, 1024, loc.addr));
        resolved++;
        numbered += loc.name === '.L.constant' ? 0 : 1;
      } else {
        // a reconstructed unit whose line, column or function spelling is not yet the target's
        expect(l.reason).toBe('source_location not in the target');
      }
    }
    expect(resolved).toBeGreaterThan(0);
    // the second and later records of one object are .L.constant.<n>
    expect(numbered).toBeGreaterThan(0);
  });
});

// target 700 is vota::CThreadVota::StartTick with api::CTickManager::StartTick inlined, target 5447 api::CWait's ctor:
// each references a source_location record of its own, and both call the same std::format helpers
const START_TICK = '_ZN4vota11CThreadVota9StartTickEh';
const CWAIT_CTOR = '_ZN3api5CWaitC2Em';
const VFORMAT_TO =
  '_ZNSt3__28__format12__vformat_toB8ne210108INS_26basic_format_parse_contextIcEENS_20basic_format_contextINS_20back_insert_iteratorINS0_15__output_bufferIcEEEEcEEEENT0_8iteratorEOT_OSA_';
const real700 = real && existsSync(`${URNA}/src/uenux2/src/app/vota/cthreadvota.cpp`);

describe('srclocAnchors', { timeout: 180_000 }, () => {
  it.skipIf(!real700)(
    'pairs the one target function and the one of ours that reference a source_location record',
    () => {
      const ours = loadOurSide(PLAIN, MAP_DIR);
      const target = loadTargetSide(TARGET, SYMBOLS, `${URNA}/ledger`, ours.stackSize);
      const { image } = memoryImage(ours.module);
      const anchors = srclocAnchors(
        ours,
        image,
        target,
        normalizeModule(target.module, { depth: 0 }),
        normalizeModule(ours.module, { depth: 0 }),
      );
      const anchored = new Map(anchors.map(([t, o]) => [o, t]));
      expect(anchored.get(ours.fnByKey.get(START_TICK)!)).toBe(700);
      expect(anchored.get(ours.fnByKey.get(CWAIT_CTOR)!)).toBe(5447);
      // a function is anchored to one function of the other side, never two
      expect(new Set(anchors.map(([t]) => t)).size).toBe(anchors.length);
      expect(anchored.size).toBe(anchors.length);
    },
  );

  it.skipIf(!real700)('names a callee its anchored callers vote for, listing them as its voters', () => {
    const syms = targetSymbolsOf(parseSymbolFile(readFileSync(SYMBOLS, 'utf8')));
    expect(syms.fns.get(START_TICK)).toBe(700);
    expect(syms.fnEvidence.get(START_TICK)?.evidence).toBe('srcloc-anchor');
    expect(syms.fns.get(VFORMAT_TO)).toBe(146);
    expect(soleFnVoters(syms, VFORMAT_TO)).toEqual([700, 5447]);
    // a name a seed pair voted for lists no voters
    expect(soleFnVoters(syms, '__cxa_allocate_exception')).toBeNull();
  });
});

// target 0x18336c is api::AudioCollector's vtable; our ASN1 OBJECT_IDENTIFIER destructors have the shape of its
// destructors, so aligning them votes OBJECT_IDENTIFIER's vtable onto it
const AUDIO_COLLECTOR_VTABLE = 0x18336c;
const OID_VTABLE = '_ZTVN4ASN117OBJECT_IDENTIFIERE';
const STRINGSTREAM = 'NSt3__218basic_stringstreamIcNS_11char_traitsIcEENS_9allocatorIcEEEE';

describe('refuteDataNames', () => {
  const load = () => {
    const ours = loadOurSide(PLAIN, MAP_DIR);
    return symbolMaps(ours, loadTargetSide(TARGET, SYMBOLS, `${URNA}/ledger`, ours.stackSize));
  };

  it.skipIf(!real)('drops an RTTI name whose target bytes describe another class', () => {
    const { ours, ourImage, target } = load();
    expect(rttiClassAt(target.image, target.imageBase, OID_VTABLE, AUDIO_COLLECTOR_VTABLE)).toBe(
      'N3api14AudioCollectorE',
    );
    const names = new Map([
      ['_ZTVN3api14AudioCollectorE', { addr: AUDIO_COLLECTOR_VTABLE, evidence: 'rtti' }],
      [OID_VTABLE, { addr: AUDIO_COLLECTOR_VTABLE, evidence: 'vote:2', voters: [10822, 10840] }],
    ]);
    const stats = refuteDataNames(ours, ourImage, target, new Map(), names);
    expect([...names.keys()]).toEqual(['_ZTVN3api14AudioCollectorE']);
    expect(stats.rttiRefuted).toBe(1);
  });

  it.skipIf(!real)('drops a voted name that shares its target address with another object of ours', () => {
    const { ours, ourImage, target } = load();
    const names = new Map([
      ['_ZTVN3api14AudioCollectorE', { addr: AUDIO_COLLECTOR_VTABLE, evidence: 'rtti' }],
      // stdout's one word points at __stdout_FILE, which has no target name: its bytes cannot refute it
      ['stdout', { addr: AUDIO_COLLECTOR_VTABLE, evidence: 'vote:1', voters: [394] }],
    ]);
    const stats = refuteDataNames(ours, ourImage, target, new Map(), names);
    expect([...names.keys()]).toEqual(['_ZTVN3api14AudioCollectorE']);
    expect(stats).toMatchObject({ voteBytesRefuted: 0, placeRefuted: 1 });
    // two names of ours with the same bytes may share a place: K256 and SHA256_K_ARRAY are the same table
    const same = new Map([
      ['K256', { addr: 0x164290, evidence: 'vote:8' }],
      ['SHA256_K_ARRAY', { addr: 0x164290, evidence: 'content' }],
    ]);
    refuteDataNames(ours, ourImage, target, new Map(), same);
    expect(same.size).toBe(2);
  });

  it.skipIf(!real)('keeps the generated symbol file free of the class the target does not have', () => {
    const syms = targetSymbolsOf(parseSymbolFile(readFileSync(SYMBOLS, 'utf8')));
    expect(syms.data.has(OID_VTABLE)).toBe(false);
    expect(syms.data.get('_ZTVN3api14AudioCollectorE')).toBe(AUDIO_COLLECTOR_VTABLE);
  });
});

describe('vtableTypeinfoOffset', () => {
  it.skipIf(!real)("places a vtable with virtual bases at its first offset word, ahead of the ledger's", () => {
    const ours = loadOurSide(PLAIN, MAP_DIR);
    const { image } = memoryImage(ours.module);
    const byKey = new Map(ours.map.data.map((d) => [d.key, d]));
    expect(vtableTypeinfoOffset(byKey, image, `_ZTV${STRINGSTREAM}`)).toBe(8);
    expect(vtableTypeinfoOffset(byKey, image, '_ZTVN3api14AudioCollectorE')).toBe(4);
    // ledger/vtables.json gives the primary table's offset-to-top word, 0x13c48c
    const syms = targetSymbolsOf(parseSymbolFile(readFileSync(SYMBOLS, 'utf8')));
    expect(syms.data.get(`_ZTV${STRINGSTREAM}`)).toBe(0x13c488);
  });
});

describe('compareObject', () => {
  it.skipIf(!real)('compares an object word by word, re-pointing our pointers, and says when it cannot tell', () => {
    const ours = loadOurSide(PLAIN, MAP_DIR);
    const maps = symbolMaps(ours, loadTargetSide(TARGET, SYMBOLS, `${URNA}/ledger`, ours.stackSize));
    const obj = (key: string) => maps.ours.map.data.find((d) => d.key === key)!;
    // an EVP_CIPHER: integers, then function pointers (table slots that also look like data addresses)
    expect(compareObject(maps, obj('aes_128_cbc'), maps.target.symbols.data.get('aes_128_cbc')!).verdict).toBe(
      'agrees',
    );
    // OID's vtable never agrees with AudioCollector's; whether a word proves it depends on where our link puts its
    // strings, since a table slot of ours can also be the address of one of them
    expect(compareObject(maps, obj(OID_VTABLE), AUDIO_COLLECTOR_VTABLE).verdict).not.toBe('agrees');
    // a .bss object is zeros on both sides
    expect(
      compareObject(maps, obj('__cxa_new_handler'), maps.target.symbols.data.get('__cxa_new_handler')!).verdict,
    ).toBe('unknown');
  });
});

describe('resolveRepointed', { timeout: 180_000 }, () => {
  const load = () => {
    const ours = loadOurSide(PLAIN, MAP_DIR);
    return symbolMaps(ours, loadTargetSide(TARGET, SYMBOLS, `${URNA}/ledger`, ours.stackSize));
  };
  const obj = (maps: ReturnType<typeof load>, key: string) => maps.ours.map.data.find((d) => d.key === key)!;

  it.skipIf(!real)('places a .data object the symbol file does not name by its bytes, its pointers re-pointed', () => {
    // CMAC's OSSL_PARAM table: string pointers, small integers, NULLs and -1 sizes
    const maps = load();
    const o = obj(maps, 'cmac_gettable_params.gettable_params');
    expect(maps.target.symbols.data.has(o.key)).toBe(false);
    const r = resolveRepointed(maps, o);
    expect(r).toMatchObject({ ok: true, how: 'content-repointed' });
    if (r.ok) {
      const p = objectPattern(maps, o);
      const at = r.addr - maps.target.imageBase;
      expect(p.words.every((w) => w.values.includes(maps.target.image.readUInt32LE(at + w.at)))).toBe(true);
      expect(p.fixed.every((b, k) => b === null || maps.target.image[at + k] === b)).toBe(true);
      expect(compareObject(maps, o, r.addr).verdict).not.toBe('differs');
      // an address inside the object lands at the same offset inside the target's
      const inside = resolveDataAddress(maps.ours, maps.ourImage, maps.target, o.addr + 20);
      expect(inside).toMatchObject({ ok: true, addr: r.addr + 20, how: 'content-repointed' });
    }
  });

  it.skipIf(!real)('offers every place of read-only content it finds more than once, for the body to pick', () => {
    // boost's bad_weekday typeinfo: the target holds three objects with these words
    const maps = load();
    const o = obj(maps, '_ZTIN5boost9gregorian11bad_weekdayE');
    expect(o.section.startsWith('.data.rel.ro')).toBe(true);
    const r = resolveDataAddress(maps.ours, maps.ourImage, maps.target, o.addr);
    expect(r.ok).toBe(false);
    expect(r.same?.length).toBeGreaterThan(1);
  });

  it.skipIf(!real)('does not offer the places of a writable object it finds more than once', () => {
    const maps = load();
    const o = obj(maps, '_ZN21ModuloInformacaoMidia15InformacaoMidia8fieldIdsE');
    expect(o.section.startsWith('.data.rel.ro')).toBe(false);
    expect(resolveRepointed(maps, o).same?.length).toBeGreaterThan(1);
    expect(resolveDataAddress(maps.ours, maps.ourImage, maps.target, o.addr).same).toBeUndefined();
  });
});

describe('resolveDataAddress on a short string', { timeout: 180_000 }, () => {
  it.skipIf(!real)('offers every place the target holds the same C string, past the limit for other content', () => {
    // our " \t\r\n" is once in the target, and its tail "\n" is in hundreds of places: the target's ffc_params
    // printer loads the "\n" that ends "UE DE {}\n\n"
    const ours = loadOurSide(PLAIN, MAP_DIR);
    const maps = symbolMaps(ours, loadTargetSide(TARGET, SYMBOLS, `${URNA}/ledger`, ours.stackSize));
    const ws = maps.ourImage.indexOf(Buffer.from('\0 \t\r\n\0', 'latin1'));
    expect(ws).toBeGreaterThan(0);
    const newline = 1024 + ws + 4;
    const used = maps.target.imageBase + maps.target.image.indexOf(Buffer.from('UE DE {}\n\n\0', 'latin1')) + 9;
    const r = resolveDataAddress(maps.ours, maps.ourImage, maps.target, newline);
    expect(r.ok).toBe(true);
    expect(r.same?.length).toBeGreaterThan(SAME_CONTENT_LIMIT);
    expect(r.same).toContain(used);
  });
});

describe('remapBody on a function the target holds in several table slots', { timeout: 180_000 }, () => {
  it.skipIf(!real)('offers each of its slots and lets the target body pick one', () => {
    // sqlite3_result_text64 (target 1311) holds a pointer to a function that the target's table holds in several
    // slots: binaryen merged identical bodies, and each slot that pointed at one of them now points at the survivor
    const ours = loadOurSide(PLAIN, MAP_DIR);
    const maps = symbolMaps(ours, loadTargetSide(TARGET, SYMBOLS, `${URNA}/ledger`, ours.stackSize));
    const r = remapBody(maps, ours.fnByKey.get('sqlite3_result_text64')!);
    expect(r.ok).toBe(true);
    if (r.ok) {
      const tm = maps.target.module;
      const m = matchesTarget(r.pieces, tm.funcs[1311 - tm.importedFuncCount].bodyBytes);
      expect(m.match).toBe(true);
      expect(m.readings?.some((x) => x.startsWith('table slot copy'))).toBe(true);
    }
  });
});

describe('remapBody on a packed string_view', { timeout: 180_000 }, () => {
  it.skipIf(!real700)("re-points an i64.const holding a string's length and address into the target's", () => {
    const ours = loadOurSide(PLAIN, MAP_DIR);
    const maps = symbolMaps(ours, loadTargetSide(TARGET, SYMBOLS, `${URNA}/ledger`, ours.stackSize));
    const o = ours.fnByKey.get(CWAIT_CTOR)!;
    const packed = disassemble(ours.module.funcs[o - ours.module.importedFuncCount]).find(
      (i) => i.opcode === 0x42 && (i.imms[0] as { value: bigint }).value > 1n << 32n,
    )!;
    const v = (packed.imms[0] as { value: bigint }).value;
    // "Quantidade excessiva de milissegundos {}": 40 bytes
    expect(v >> 32n).toBe(40n);
    expect(cStringAt(maps.ourImage, 1024, Number(v & 0xffffffffn))?.length).toBe(40);
    const r = remapBody(maps, o);
    expect(r.ok).toBe(true);
    if (r.ok) {
      const tf = maps.target.module.funcs[5447 - maps.target.module.importedFuncCount];
      const m = matchesTarget(r.pieces, tf.bodyBytes);
      expect(m.match).toBe(true);
      expect(m.readings?.filter((x) => x.endsWith('(string_view)')).length).toBe(2);
    }
  });
});

describe('matchesTarget', () => {
  it('takes whichever reading of an ambiguous constant the target body has', () => {
    const pieces = [
      Uint8Array.from([0x00, 0x41]),
      {
        alternatives: [Uint8Array.from([0x90, 0x64]), Uint8Array.from([0x05])],
        readings: ['table slot of f', 'literal'],
        at: 1,
      },
      Uint8Array.from([0x0b]),
    ];
    expect(matchesTarget(pieces, Uint8Array.from([0x00, 0x41, 0x05, 0x0b]))).toEqual({
      match: true,
      readings: ['literal'],
    });
    expect(matchesTarget(pieces, Uint8Array.from([0x00, 0x41, 0x90, 0x64, 0x0b]))).toEqual({
      match: true,
      readings: ['table slot of f'],
    });
    expect(matchesTarget(pieces, Uint8Array.from([0x00, 0x41, 0x06, 0x0b]))).toEqual({
      match: false,
      firstDifference: 2,
    });
  });
});

describe('symbol file', () => {
  it('keeps the hand-edit section verbatim and lets it win over a generated line', () => {
    const generated = [
      { name: 'foo', at: { kind: 'func' as const, index: 200 }, evidence: 'pair-addr' },
      { name: 'bar', at: { kind: 'data' as const, addr: 0x1234 }, size: 8, evidence: 'rtti' },
      { name: 'baz', at: { kind: 'data' as const, addr: 0x1240 }, size: 4, evidence: 'vote:3', voters: [137, 140] },
    ];
    const text = renderSymbolFile(['header'], generated, ['foo = func:201; // evidence:hand checked by eye']);
    expect(text).toContain(`${renderSymbolEntry(generated[2])}\n${HAND_EDIT_MARKER}\n`);
    const parsed = parseSymbolFile(text);
    expect(parsed.generated).toHaveLength(3);
    expect(parsed.handLines).toEqual(['foo = func:201; // evidence:hand checked by eye']);
    expect(targetSymbolsOf(parsed).fns.get('foo')).toBe(201);
    expect(targetSymbolsOf(parsed).data.get('bar')).toBe(0x1234);
    expect(parsed.generated[1]).toMatchObject({ size: 8, evidence: 'rtti' });
    expect(soleVoters(targetSymbolsOf(parsed), 'baz')).toEqual([137, 140]);
    expect(soleVoters(targetSymbolsOf(parsed), 'bar')).toBeNull();
  });

  it.skipIf(!real)(
    'names the i2d_/d2i_ wrappers that differ only by their ASN1_ITEM through their callers, not by position',
    () => {
      // each wrapper's body is the same but for the item's address; the item's sname says which one it is
      const target = parseModule(new Uint8Array(readFileSync(TARGET)));
      const { image } = memoryImage(target);
      const syms = targetSymbolsOf(parseSymbolFile(readFileSync(SYMBOLS, 'utf8')));
      let checked = 0;
      for (const [name, t] of syms.fns) {
        const m =
          /^(?:i2d|d2i)_(RSAPublicKey|RSAPrivateKey|X509_SIG|X509_ALGOR|ASN1_OCTET_STRING|ASN1_INTEGER|DHparams)$/.exec(
            name,
          );
        if (m === null) {
          continue;
        }
        const item = disassemble(target.funcs[t - target.importedFuncCount])
          .flatMap((i) => i.imms)
          .find((im) => im.kind === 's32' && im.value >= 1024);
        const sname = image.readUInt32LE((item as { value: number }).value + 24 - 1024);
        expect(cStringAt(image, 1024, sname)?.toString('latin1')).toBe(m[1]);
        checked++;
      }
      expect(checked).toBeGreaterThanOrEqual(6);
    },
  );
});
