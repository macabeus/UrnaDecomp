// Substitution on the real modules of this checkout: the target's split (build/out/splat/pieces), the plain
// skeleton link with its scratch map (tools/round/link-map.sh) and splat/symbol_addrs.txt.
import { existsSync, readFileSync } from 'node:fs';
import { describe, expect, it } from 'vitest';

import { disassemble } from '../src/disasm';
import { readManifest } from '../src/splat';
import { remapBody } from '../src/splat-remap';
import {
  MatchesFailed,
  type ScanRow,
  addMatches,
  attempt,
  buildSplat,
  cachedRemapper,
  contentPairs,
  parseMatchList,
  readPieceBodies,
  readingMeterOf,
  renderScan,
  selfVoted,
  withMatches,
} from '../src/splat-sub';
import { loadOurSide, loadTargetSide, parseSymbolFile, symbolMaps, targetSymbolsOf } from '../src/splat-symbols';
import { URNA_ROOT } from './helpers';

const URNA = process.env.URNA_SPLAT_ROOT ?? URNA_ROOT;
const TARGET = `${URNA}/target/vota_web_wasm.wasm`;
const PLAIN = `${URNA}/build/out/wasm/vota_web/vota_web_wasm.wasm`;
const MAP_DIR = `${URNA}/build/out/wasm-map/vota_web`;
const SYMBOLS = `${URNA}/splat/symbol_addrs.txt`;
const PIECES = `${URNA}/build/out/splat/pieces`;
const TARGET_SHA = '8579ef8d1a93819152cecdbf52906d2e4dd0f57999cb2e1e8c0d1161cd1007e2';
const real = existsSync(`${MAP_DIR}/names.tsv`) && existsSync(SYMBOLS) && existsSync(`${PIECES}/manifest.json`);

// target 9670 is the __get_deleter of shared_ptr<userdict::correction>(userdict::insertion*); the ledger pairs it
// with russian_info's, a function of the same shape that compares another type name
const USERDICT_DELETER =
  '_ZNKSt3__220__shared_ptr_pointerIPN7RHVoice8userdict9insertionENS_10shared_ptrINS2_10correctionEE27__shared_ptr_default_deleteIS6_S3_EENS_9allocatorIS3_EEE13__get_deleterERKSt9type_info';
const RUSSIAN_DELETER =
  '_ZNKSt3__220__shared_ptr_pointerIPN7RHVoice12russian_infoENS_10shared_ptrINS1_13language_infoEE27__shared_ptr_default_deleteIS5_S2_EENS_9allocatorIS2_EEE13__get_deleterERKSt9type_info';

let cache: { maps: ReturnType<typeof symbolMaps>; bodies: Map<number, Uint8Array> } | null = null;
function load() {
  if (cache === null) {
    const ours = loadOurSide(PLAIN, MAP_DIR);
    const target = loadTargetSide(TARGET, SYMBOLS, `${URNA}/ledger`, ours.stackSize);
    cache = { maps: symbolMaps(ours, target), bodies: readPieceBodies(PIECES) };
  }
  return cache;
}

function candidate(symbol: string) {
  const ours = load().maps.ours.fnByKey.get(symbol);
  expect(ours).toBeDefined();
  return { ours: ours!, symbol, source: 'symbol' as const };
}

describe('attempt', { timeout: 180_000 }, () => {
  it.skipIf(!real)('re-points our function into exactly its target piece', () => {
    const { maps, bodies } = load();
    const a = attempt(maps, candidate('sqlite3ErrorMsg'), 162, bodies.get(162)!);
    expect(a.verdict).toBe('match');
    expect(a.verdict === 'match' && Buffer.from(a.bytes).equals(Buffer.from(bodies.get(162)!))).toBe(true);
  });

  it.skipIf(!real)('names the first differing instruction on both sides and what our operand is', () => {
    const { maps, bodies } = load();
    const a = attempt(maps, candidate(RUSSIAN_DELETER), 9670, bodies.get(9670)!);
    expect(a.verdict).toBe('differs');
    expect(a.verdict === 'differs' && a.detail).toMatch(
      /^\+0xb target \+0xa i32\.const 1103687 \| ours \+0xa i32\.const \d+ \[ours \d+ = _ZTS\S+\+0: address /,
    );
  });
});

describe('attempt on a voted name', { timeout: 180_000 }, () => {
  it.skipIf(!real)("does not match AudioCollector's destructors with ASN1 OBJECT_IDENTIFIER's", () => {
    // the two bodies are the same instructions; only the vtable address tells the classes apart, and the target's
    // vtable there is AudioCollector's
    const { maps, bodies } = load();
    for (const [index, symbol] of [
      [10822, '_ZN4ASN117OBJECT_IDENTIFIERD0Ev'],
      [10840, '_ZN4ASN117OBJECT_IDENTIFIERD2Ev'],
    ] as const) {
      const a = attempt(maps, candidate(symbol), index, bodies.get(index)!);
      expect(a.verdict).toBe('differs');
      expect(a.verdict !== 'match' && a.detail).toMatch(/_ZTVN4ASN117OBJECT_IDENTIFIERE: no target symbol/);
    }
  });
});

describe('selfVoted', { timeout: 180_000 }, () => {
  const chosenOf = (symbol: string, index: number) => {
    const { maps, bodies } = load();
    const a = attempt(maps, candidate(symbol), index, bodies.get(index)!);
    expect(a.verdict).toBe('match');
    return a.verdict === 'match' ? a.chosen : [];
  };

  it.skipIf(!real)('refuses a match whose address name comes only from its own vote and says nothing else', () => {
    // operator new's __cxa_new_handler is .bss: zeros on both sides, voted by target 137 alone
    const { maps } = load();
    const chosen = chosenOf('_Znwm', 137);
    const refused = selfVoted(maps, new Map([[137, { chosen }]]));
    expect(refused.get(137)).toMatchObject({ symbol: '__cxa_new_handler', bytes: 'unknown' });
  });

  it.skipIf(!real)('keeps it when another match re-points through the same name', () => {
    const { maps } = load();
    const chosen = chosenOf('_Znwm', 137);
    const other = { chosen: [{ reading: 'address symbol __cxa_new_handler', preferred: true, at: 0, oursAt: 0 }] };
    expect(
      selfVoted(
        maps,
        new Map([
          [137, { chosen }],
          [138, other],
        ]),
      ).size,
    ).toBe(0);
  });

  it.skipIf(!real)('refuses a match whose callee name comes only from its own call operand', () => {
    // target 146 (std::format's __vformat_to) is named by the calls of the anchored pairs 700 and 5447 alone
    const { maps } = load();
    if (!maps.ours.fnByKey.has('_ZN4vota11CThreadVota9StartTickEh')) {
      return;
    }
    const vformatTo = [...maps.target.symbols.fns].find(([, i]) => i === 146)![0];
    const fnEvidence = new Map(maps.target.symbols.fnEvidence);
    fnEvidence.set(vformatTo, { evidence: 'call', voters: [700] });
    const alone = { ...maps, target: { ...maps.target, symbols: { ...maps.target.symbols, fnEvidence } } };
    const startTick = maps.ours.fnByKey.get('_ZN4vota11CThreadVota9StartTickEh')!;
    const cwait = maps.ours.fnByKey.get('_ZN3api5CWaitC2Em')!;
    expect(selfVoted(alone, new Map([[700, { chosen: [], ours: startTick }]])).get(700)).toMatchObject({
      symbol: vformatTo,
      bytes: null,
    });
    // another match that calls the same function confirms the name
    expect(
      selfVoted(
        alone,
        new Map([
          [700, { chosen: [], ours: startTick }],
          [5447, { chosen: [], ours: cwait }],
        ]),
      ).size,
    ).toBe(0);
  });

  it.skipIf(!real)('counts a constant as a use of the function in its table slot only when the target reads a slot', () => {
    // two matches of one body of ours whose i32.const equals a named function's slot in our table, the name voted
    // by the first match alone: an integer constant in the other confirms nothing
    const { maps } = load();
    let use: { ours: number; at: number; symbol: string } | undefined;
    for (const f of maps.ours.module.funcs) {
      for (const i of disassemble(f)) {
        const im = i.imms[0];
        const g = i.opcode === 0x41 && im?.kind === 's32' ? maps.ours.elem.get(im.value) : undefined;
        const symbol = g === undefined ? undefined : maps.ours.fns.get(g)?.key;
        if (symbol !== undefined && maps.target.symbols.fns.has(symbol)) {
          use = { ours: f.index, at: i.offset, symbol };
          break;
        }
      }
      if (use !== undefined) {
        break;
      }
    }
    expect(use).toBeDefined();
    const { ours, at, symbol } = use!;
    const fnEvidence = new Map(maps.target.symbols.fnEvidence);
    fnEvidence.set(symbol, { evidence: 'table', voters: [1] });
    const alone = { ...maps, target: { ...maps.target, symbols: { ...maps.target.symbols, fnEvidence } } };
    const read = (reading: string) => ({ chosen: [{ reading, preferred: false, at: 0, oursAt: at }], ours });
    const slot = read(`table slot of ${symbol}`);
    expect(
      selfVoted(
        alone,
        new Map([
          [1, slot],
          [2, read('literal')],
        ]),
      ).get(1),
    ).toMatchObject({ symbol, bytes: null });
    expect(
      selfVoted(
        alone,
        new Map([
          [1, slot],
          [2, slot],
        ]),
      ).size,
    ).toBe(0);
  });

  it.skipIf(!real)("keeps it when our object's bytes are the target's there", () => {
    // __strerror_l's errmsgidx is voted by target 545 alone, and its 308 bytes are the same on both sides
    const { maps } = load();
    expect(maps.target.symbols.dataEvidence.get('errmsgidx')).toEqual({ evidence: 'vote:1', voters: [545] });
    expect(selfVoted(maps, new Map([[545, { chosen: chosenOf('__strerror_l', 545) }]])).size).toBe(0);
  });
});

describe('readingMeterOf', { timeout: 180_000 }, () => {
  it.skipIf(!real)('sorts matches by the strongest choice the target made among our readings', () => {
    const manifest = readManifest(PIECES);
    const size = (i: number) => manifest.functions.find((f) => f.index === i)!.size;
    const c = (reading: string, preferred: boolean) => ({ reading, preferred, at: 0, oursAt: 0 });
    const meter = readingMeterOf(
      manifest,
      new Map([
        [162, [c('address symbol x', true), c('literal', true)]],
        [829, [c('literal', false), c('table slot of f', false), c('address symbol y', true)]],
        [1189, [c('address same-content 0x10', true), c('literal', false)]],
      ]),
    );
    expect(meter).toEqual({
      preferred: { matches: 1, bytes: size(162) },
      other: { matches: 1, bytes: size(829), operands: 3, byKind: { literal: 2, 'table slot': 1 } },
      copy: { matches: 1, bytes: size(1189), operands: 1 },
    });
  });
});

describe('remapBody', { timeout: 180_000 }, () => {
  it.skipIf(!real)('keeps a memarg offset of 1024 or more as a field offset when the target does', () => {
    // BF_encrypt reads the S-boxes at offset 1024.. of its BF_KEY; our .rodata starts at 1024 too
    const { maps, bodies } = load();
    expect(attempt(maps, candidate('BF_encrypt'), 829, bodies.get(829)!).verdict).toBe('match');
  });

  it.skipIf(!real)('offers every target copy of a string the target holds more than once', () => {
    const { maps, bodies } = load();
    const r = remapBody(maps, maps.ours.fnByKey.get('sqlite3_exec')!);
    expect(
      r.ok && r.pieces.some((p) => !(p instanceof Uint8Array) && p.readings.some((x) => x.includes('same-content'))),
    ).toBe(true);
    expect(attempt(maps, candidate('sqlite3_exec'), 1189, bodies.get(1189)!).verdict).toBe('match');
  });
});

describe('contentPairs', { timeout: 180_000 }, () => {
  it.skipIf(!real)('pairs an unmatched target body with the one unused function of ours that re-points into it', () => {
    const { maps, bodies } = load();
    const used = maps.ours.fnByKey.get('sqlite3ErrorMsg')!;
    const row = (index: number, verdict: ScanRow['verdict'], ours: number | null): ScanRow => ({
      index,
      symbol: null,
      ours,
      source: null,
      verdict,
      detail: '',
      size: bodies.get(index)!.length,
      ledgerKind: 'none',
      group: '-',
      tried: 0,
    });
    const pairs = contentPairs(maps, [row(9670, 'differs', null), row(162, 'match', used)], bodies);
    expect(pairs.get(9670)?.candidate).toMatchObject({ symbol: USERDICT_DELETER, source: 'content' });
    expect(pairs.has(162)).toBe(false);
  });
});

describe('withMatches', { timeout: 180_000 }, () => {
  it.skipIf(!real)('names a matched function by its match and drops every other name the symbol file gives it', () => {
    const { maps } = load();
    expect(maps.target.symbols.fns.get('sqlite3ErrorMsg')).toBe(162);
    const matched = withMatches(maps, [
      { index: 9670, symbol: USERDICT_DELETER },
      { index: 162, symbol: 'BF_encrypt' },
    ]);
    expect(matched.target.symbols.fns.get(USERDICT_DELETER)).toBe(9670);
    expect(matched.target.symbols.fns.get('BF_encrypt')).toBe(162);
    expect(matched.target.symbols.fns.has('sqlite3ErrorMsg')).toBe(false);
    expect(maps.target.symbols.fns.get('sqlite3ErrorMsg')).toBe(162);
  });
});

describe('cachedRemapper', { timeout: 180_000 }, () => {
  it.skipIf(!real)('re-points again exactly the functions whose referenced functions resolve differently', () => {
    const { maps } = load();
    const first = cachedRemapper(maps);
    const caller = maps.ours.fnByKey.get('sqlite3ErrorMsg')!;
    const unrelated = maps.ours.fnByKey.get('BF_encrypt')!;
    first.remap(caller);
    first.remap(unrelated);
    // match one of sqlite3ErrorMsg's callees to another target function: only the caller is re-pointed again
    const callee = maps.ours.module.funcs[caller - maps.ours.module.importedFuncCount];
    const calls = disassemble(callee).filter((i) => i.opcode === 0x10);
    const f = (calls[0].imms[0] as { value: number }).value;
    const second = cachedRemapper(withMatches(maps, [{ index: 136, symbol: maps.ours.fns.get(f)!.key }]), first);
    expect(second.remap(unrelated)).toBe(first.results.get(unrelated));
    expect(second.remap(caller)).not.toBe(first.results.get(caller));
  });
});

describe('parseMatchList', () => {
  it('reads index and symbol per line, with comments', () => {
    const { matches } = parseMatchList('# matched\n162 sqlite3ErrorMsg  # by symbol\n\n169 BN_CTX_get\n');
    expect(matches).toEqual([
      { index: 162, symbol: 'sqlite3ErrorMsg', line: 2 },
      { index: 169, symbol: 'BN_CTX_get', line: 4 },
    ]);
  });

  it('refuses a function matched twice and a line it cannot read', () => {
    expect(() => parseMatchList('162 a\n162 b\n')).toThrow(/line 2: function 162 is already matched on line 1/);
    expect(() => parseMatchList('sqlite3ErrorMsg 162\n')).toThrow(/line 1: expected/);
  });
});

describe('addMatches', () => {
  it('appends the scan matches not listed yet and keeps every existing line', () => {
    const rows = [
      { index: 162, symbol: 'sqlite3ErrorMsg', verdict: 'match' },
      { index: 169, symbol: 'BN_CTX_get', verdict: 'match' },
      { index: 170, symbol: 'x', verdict: 'differs' },
    ] as ScanRow[];
    const r = addMatches('# hand note\n162 sqlite3ErrorMsg # kept\n', rows);
    expect(r).toEqual({
      text: '# hand note\n162 sqlite3ErrorMsg # kept\n169 BN_CTX_get\n',
      added: 1,
      renamed: [],
      missing: [],
      contradicted: [],
    });
  });

  it('renames in place a line whose symbol left our link to the twin the scan matches, and keeps any other', () => {
    const rows = [
      { index: 246, symbol: 'NumViasD0', verdict: 'match' },
      { index: 247, symbol: 'kept', verdict: 'match' },
      { index: 248, symbol: 'other', verdict: 'differs' },
      { index: 249, symbol: 'fresh', verdict: 'match' },
    ] as ScanRow[];
    const ours = new Set(['NumViasD0', 'kept', 'other', 'fresh']);
    const r = addMatches('246 FotoD0  # twin\n247 kept\n248 gone\n', rows, (s) => ours.has(s));
    expect(r.text).toBe('246 NumViasD0  # twin\n247 kept\n248 gone\n249 fresh\n');
    expect(r.renamed).toEqual([{ index: 246, line: 1, from: 'FotoD0', to: 'NumViasD0' }]);
    // 248's symbol is missing too, but the scan matches 248 with nothing: the build is left to refuse it
    expect(r.missing).toEqual([{ index: 248, symbol: 'gone', line: 3 }]);
    expect(r.added).toBe(1);
    // without the link's symbols, nothing is renamed
    expect(addMatches('246 FotoD0\n', rows).text).toBe('246 FotoD0\n247 kept\n249 fresh\n');
  });

  it('reports a listed line whose function the symbol file names after another function of our link', () => {
    const rows = [{ index: 11611, symbol: 'weekdayRethrow', verdict: 'match' }] as ScanRow[];
    const ours = new Set(['weekdayRethrow', 'monthRethrow']);
    const named = (i: number): string[] => (i === 11611 ? ['monthRethrow'] : []);
    const r = addMatches('11611 weekdayRethrow\n', rows, (x) => ours.has(x), named);
    expect(r.text).toBe('11611 weekdayRethrow\n');
    expect(r.contradicted).toEqual([{ match: { index: 11611, symbol: 'weekdayRethrow', line: 1 }, named: ['monthRethrow'] }]);
    // a name our link lacks, or the listed one among the names, is no contradiction
    expect(addMatches('11611 weekdayRethrow\n', rows, (x) => x === 'weekdayRethrow', named).contradicted).toEqual([]);
    expect(addMatches('11611 monthRethrow\n', rows, (x) => ours.has(x), named).contradicted).toEqual([]);
  });

  it.skipIf(!existsSync(SYMBOLS))(
    "names 11611 wrapexcept<bad_month>::rethrow by the slot of the target's vtable that holds it",
    () => {
      const syms = targetSymbolsOf(parseSymbolFile(readFileSync(SYMBOLS, 'utf8')));
      const name = '_ZNK5boost10wrapexceptINS_9gregorian9bad_monthEE7rethrowEv';
      expect(syms.fns.get(name)).toBe(11611);
      expect(syms.fnEvidence.get(name)?.evidence).toBe('vtable');
      expect(syms.fns.get('_ZNK5boost10wrapexceptINS_9gregorian11bad_weekdayEE7rethrowEv')).toBeUndefined();
      // CMixerST's second table has two slots more in our 7-Zip than in the target's: its slots pair with none, so
      // the target's SelectMainCoder thunk (8361) is not named after our AddCoder thunk two slots up
      expect(syms.fns.get('_ZThn4_N12NCoderMixer28CMixerST8AddCoderERK13CCreatedCoder')).toBeUndefined();
    },
  );

  it.skipIf(!real)(
    "renames 246 back to the survivor of the twin merge that took FotoD0Ev's name (facts/splat.md §S6)",
    { timeout: 180_000 },
    () => {
      // a stale match list: 246 under FotoD0Ev, the symbol that left our link when unit 20 brought in a twin of its
      // body linked ahead of it
      const { maps } = load();
      const FOTO = '_ZN21ModuloTiposEleitorais4FotoD0Ev';
      const SURVIVOR = '_ZN21ModuloEstadoGeralDefs26NumViasImpressasRelatoriosD0Ev';
      const inOurLink = (s: string): boolean => (maps.ours.fnByKey.get(s) ?? -1) >= maps.ours.module.importedFuncCount;
      expect(inOurLink(FOTO)).toBe(false);
      const list = readFileSync(`${URNA}/splat/matched.txt`, 'utf8');
      expect(list).toContain(`\n246 ${SURVIVOR}\n`);
      const stale = list.replace(`\n246 ${SURVIVOR}\n`, `\n246 ${FOTO}\n`);
      const { matches } = parseMatchList(stale);
      const { bodies } = load();
      const r = attempt(withMatches(maps, matches), candidate(SURVIVOR), 246, bodies.get(246)!);
      expect(r.verdict).toBe('match');
      const row = { index: 246, symbol: SURVIVOR, verdict: 'match' } as ScanRow;
      const out = addMatches(stale, [row], inOurLink);
      expect(out.renamed).toEqual([
        { index: 246, line: matches.find((m) => m.index === 246)!.line, from: FOTO, to: SURVIVOR },
      ]);
      expect(out.missing).toEqual([]);
      expect(out.text).toBe(list);
    },
  );
});

describe('renderScan', () => {
  it('writes one row per target function with the object that defines our candidate', () => {
    const rows = [
      { index: 136, symbol: 'emscripten_builtin_free', ours: 139, source: 'symbol', verdict: 'match', detail: '' },
      { index: 140, symbol: 'ERR_new', ours: 141, source: 'symbol', verdict: 'differs', detail: '+0x1\ttarget' },
      { index: 150, symbol: null, ours: null, source: null, verdict: 'no-candidate', detail: '' },
    ].map((r) => ({ ...r, size: 9, ledgerKind: 'none', group: 'unknown', tried: r.ours === null ? 0 : 1 })) as ScanRow[];
    const objects = new Map([
      [139, '/emsdk/upstream/emscripten/cache/sysroot/lib/wasm32-emscripten/libdlmalloc.a(dlmalloc.o)'],
      [141, 'openssl/O3/crypto/err/libcrypto-lib-err_blocks.o'],
    ]);
    expect(renderScan(rows, (ours) => objects.get(ours)).split('\n')).toEqual([
      'index\tsymbol\tours\tsource\tverdict\tdetail\tsize\tledger_kind\tgroup\ttried\tobject',
      '136\temscripten_builtin_free\t139\tsymbol\tmatch\t-\t9\tnone\tunknown\t1\tsysroot:libdlmalloc.a(dlmalloc.o)',
      '140\tERR_new\t141\tsymbol\tdiffers\t+0x1 target\t9\tnone\tunknown\t1\topenssl/O3/crypto/err/libcrypto-lib-err_blocks.o',
      '150\t-\t-\t-\tno-candidate\t-\t9\tnone\tunknown\t0\t-',
      '',
    ]);
  });
});

describe('buildSplat', { timeout: 180_000 }, () => {
  it.skipIf(!real)('assembles the target from the pieces with our bytes for every match', () => {
    const { maps } = load();
    const { matches } = parseMatchList(`162 sqlite3ErrorMsg\n829 BF_encrypt\n9670 ${USERDICT_DELETER}\n`);
    const r = buildSplat(maps, PIECES, matches, TARGET_SHA);
    expect(r.assembled.sha256).toBe(TARGET_SHA);
    expect(r.assembled.unchanged.sort()).toEqual([162, 829, 9670]);
    expect(r.matched).toBe(3);
    expect(r.readings.preferred.matches + r.readings.other.matches + r.readings.copy.matches).toBe(3);
    const m = readManifest(PIECES);
    expect(r.matchedBytes).toBe(
      m.functions.filter((f) => [162, 829, 9670].includes(f.index)).reduce((n, f) => n + f.size, 0),
    );
    expect(r.functions).toBe(14_343);
  });

  it.skipIf(!real)('refuses to build while a match does not verify, naming the function and the instruction', () => {
    const { maps } = load();
    const { matches } = parseMatchList(`162 sqlite3ErrorMsg\n9670 ${RUSSIAN_DELETER}\n200 no_such_symbol\n`);
    let failure: unknown;
    try {
      buildSplat(maps, PIECES, matches, TARGET_SHA);
    } catch (e) {
      failure = e;
    }
    expect(failure).toBeInstanceOf(MatchesFailed);
    const f = failure as MatchesFailed;
    expect(f.failures.map((x) => x.match.index)).toEqual([9670, 200]);
    expect(f.message).toMatch(/line 2: function 9670 .* DIFFERS at \+0xb target \+0xa i32\.const 1103687/);
    expect(f.message).toMatch(/line 3: function 200 .*: our link has no function no_such_symbol/);
  });

  it.skipIf(!real)('refuses a match whose only evidence for an address is its own vote, naming the operand', () => {
    const { maps } = load();
    const { matches } = parseMatchList('162 sqlite3ErrorMsg\n137 _Znwm\n');
    expect(() => buildSplat(maps, PIECES, matches, TARGET_SHA)).toThrow(
      /line 2: function 137 .*matched to _Znwm .*UNCONFIRMED: self-voted \+0x1c i32\.const 1936860: __cxa_new_handler = 0x1d8ddc \(evidence:vote:1 by:137\)/,
    );
  });
});
