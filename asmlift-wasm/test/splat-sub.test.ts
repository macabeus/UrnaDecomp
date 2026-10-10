// Substitution on the real modules of this checkout: the target's split (build/out/splat/pieces), the plain
// skeleton link with its scratch map (tools/round/link-map.sh) and splat/symbol_addrs.txt.
import { existsSync } from 'node:fs';
import { describe, expect, it } from 'vitest';

import { disassemble } from '../src/disasm';
import { readManifest } from '../src/splat';
import { remapBody } from '../src/splat-remap';
import {
  ClaimsFailed,
  type ScanRow,
  addClaims,
  attempt,
  buildSplat,
  cachedRemapper,
  contentPairs,
  parseClaims,
  readPieceBodies,
  readingMeterOf,
  renderScan,
  selfVoted,
  withClaims,
} from '../src/splat-sub';
import { loadOurSide, loadTargetSide, symbolMaps } from '../src/splat-symbols';
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
    const other = { chosen: [{ reading: 'address symbol __cxa_new_handler', preferred: true, at: 0 }] };
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

  it.skipIf(!real)("keeps it when our object's bytes are the target's there", () => {
    // __strerror_l's errmsgidx is voted by target 545 alone, and its 308 bytes are the same on both sides
    const { maps } = load();
    expect(maps.target.symbols.dataEvidence.get('errmsgidx')).toEqual({ evidence: 'vote:1', voters: [545] });
    expect(selfVoted(maps, new Map([[545, { chosen: chosenOf('__strerror_l', 545) }]])).size).toBe(0);
  });
});

describe('readingMeterOf', { timeout: 180_000 }, () => {
  it.skipIf(!real)('sorts claims by the strongest choice the target made among our readings', () => {
    const manifest = readManifest(PIECES);
    const size = (i: number) => manifest.functions.find((f) => f.index === i)!.size;
    const c = (reading: string, preferred: boolean) => ({ reading, preferred, at: 0 });
    const meter = readingMeterOf(
      manifest,
      new Map([
        [162, [c('address symbol x', true), c('literal', true)]],
        [829, [c('literal', false), c('table slot of f', false), c('address symbol y', true)]],
        [1189, [c('address same-content 0x10', true), c('literal', false)]],
      ]),
    );
    expect(meter).toEqual({
      preferred: { claims: 1, bytes: size(162) },
      other: { claims: 1, bytes: size(829), operands: 3, byKind: { literal: 2, 'table slot': 1 } },
      copy: { claims: 1, bytes: size(1189), operands: 1 },
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

describe('withClaims', { timeout: 180_000 }, () => {
  it.skipIf(!real)('names a claimed function by its claim and drops every other name the symbol file gives it', () => {
    const { maps } = load();
    expect(maps.target.symbols.fns.get('sqlite3ErrorMsg')).toBe(162);
    const claimed = withClaims(maps, [
      { index: 9670, symbol: USERDICT_DELETER },
      { index: 162, symbol: 'BF_encrypt' },
    ]);
    expect(claimed.target.symbols.fns.get(USERDICT_DELETER)).toBe(9670);
    expect(claimed.target.symbols.fns.get('BF_encrypt')).toBe(162);
    expect(claimed.target.symbols.fns.has('sqlite3ErrorMsg')).toBe(false);
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
    // claim one of sqlite3ErrorMsg's callees for another target function: only the caller is re-pointed again
    const callee = maps.ours.module.funcs[caller - maps.ours.module.importedFuncCount];
    const calls = disassemble(callee).filter((i) => i.opcode === 0x10);
    const f = (calls[0].imms[0] as { value: number }).value;
    const second = cachedRemapper(withClaims(maps, [{ index: 136, symbol: maps.ours.fns.get(f)!.key }]), first);
    expect(second.remap(unrelated)).toBe(first.results.get(unrelated));
    expect(second.remap(caller)).not.toBe(first.results.get(caller));
  });
});

describe('parseClaims', () => {
  it('reads index and symbol per line, with comments', () => {
    const { claims } = parseClaims('# matched\n162 sqlite3ErrorMsg  # by symbol\n\n169 BN_CTX_get\n');
    expect(claims).toEqual([
      { index: 162, symbol: 'sqlite3ErrorMsg', line: 2 },
      { index: 169, symbol: 'BN_CTX_get', line: 4 },
    ]);
  });

  it('refuses a function claimed twice and a line it cannot read', () => {
    expect(() => parseClaims('162 a\n162 b\n')).toThrow(/line 2: function 162 is already claimed on line 1/);
    expect(() => parseClaims('sqlite3ErrorMsg 162\n')).toThrow(/line 1: expected/);
  });
});

describe('addClaims', () => {
  it('appends the scan matches not claimed yet and keeps every existing line', () => {
    const rows = [
      { index: 162, symbol: 'sqlite3ErrorMsg', verdict: 'match' },
      { index: 169, symbol: 'BN_CTX_get', verdict: 'match' },
      { index: 170, symbol: 'x', verdict: 'differs' },
    ] as ScanRow[];
    const r = addClaims('# hand note\n162 sqlite3ErrorMsg # kept\n', rows);
    expect(r).toEqual({ text: '# hand note\n162 sqlite3ErrorMsg # kept\n169 BN_CTX_get\n', added: 1 });
  });
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
  it.skipIf(!real)('assembles the target from the pieces with our bytes for every claim', () => {
    const { maps } = load();
    const { claims } = parseClaims(`162 sqlite3ErrorMsg\n829 BF_encrypt\n9670 ${USERDICT_DELETER}\n`);
    const r = buildSplat(maps, PIECES, claims, TARGET_SHA);
    expect(r.assembled.sha256).toBe(TARGET_SHA);
    expect(r.assembled.unchanged.sort()).toEqual([162, 829, 9670]);
    expect(r.claimed).toBe(3);
    expect(r.readings.preferred.claims + r.readings.other.claims + r.readings.copy.claims).toBe(3);
    const m = readManifest(PIECES);
    expect(r.claimedBytes).toBe(
      m.functions.filter((f) => [162, 829, 9670].includes(f.index)).reduce((n, f) => n + f.size, 0),
    );
    expect(r.functions).toBe(14_343);
  });

  it.skipIf(!real)('refuses to build while a claim does not verify, naming the function and the instruction', () => {
    const { maps } = load();
    const { claims } = parseClaims(`162 sqlite3ErrorMsg\n9670 ${RUSSIAN_DELETER}\n200 no_such_symbol\n`);
    let failure: unknown;
    try {
      buildSplat(maps, PIECES, claims, TARGET_SHA);
    } catch (e) {
      failure = e;
    }
    expect(failure).toBeInstanceOf(ClaimsFailed);
    const f = failure as ClaimsFailed;
    expect(f.failures.map((x) => x.claim.index)).toEqual([9670, 200]);
    expect(f.message).toMatch(/line 2: function 9670 .* DIFFERS at \+0xb target \+0xa i32\.const 1103687/);
    expect(f.message).toMatch(/line 3: function 200 .*: our link has no function no_such_symbol/);
  });

  it.skipIf(!real)('refuses a claim whose only evidence for an address is its own vote, naming the operand', () => {
    const { maps } = load();
    const { claims } = parseClaims('162 sqlite3ErrorMsg\n137 _Znwm\n');
    expect(() => buildSplat(maps, PIECES, claims, TARGET_SHA)).toThrow(
      /line 2: function 137 .*claimed by _Znwm .*UNCONFIRMED: self-voted \+0x1c i32\.const 1936860: __cxa_new_handler = 0x1d8ddc \(evidence:vote:1 by:137\)/,
    );
  });
});
