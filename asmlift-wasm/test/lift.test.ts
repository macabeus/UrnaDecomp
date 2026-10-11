// The lifter on the real target: every function lifts, the representative shapes read the way
// they should, and the outputs compile with em++ in the pinned image (skipped without the image).
// The tests that spell a callee by its mangled symbol read the symbol lists `make -C src link` writes beside the
// plain link (tools/round/link-symbols.sh); they are skipped where those are absent, as in a fresh clone.
import { spawnSync } from 'node:child_process';
import { existsSync, mkdtempSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';
import { describe, expect, it } from 'vitest';

import { glueNamesFor } from '../src/glue-file';
import { ledgerNames, readAttribution, readLedgerTsv, readMangledPairs } from '../src/ledger';
import {
  COMPONENT_FLAGS,
  type LiftOptions,
  TREE_FLAGS,
  type TreeEvidence,
  cName,
  liftFlags,
  liftFunction,
  liftNames,
  recordTree,
} from '../src/lift';
import { liftInputs, readTreeEvidence } from '../src/lift-inputs';
import { DEFAULT_IMAGE, compileLifted } from '../src/rank';
import { TARGET, target, URNA_ROOT } from './helpers';

const LEDGER = `${URNA_ROOT}/ledger/functions.tsv`;
const PLAIN = `${URNA_ROOT}/build/out/wasm/vota_web`;

const hasImage = spawnSync('docker', ['image', 'inspect', DEFAULT_IMAGE], { encoding: 'utf8' }).status === 0;
const hasMangled = existsSync(join(PLAIN, 'symbols.mangled.txt'));

let cachedOpts: LiftOptions | null = null;
function opts(): LiftOptions {
  if (cachedOpts === null) {
    const m = target();
    const glue = glueNamesFor(TARGET);
    const ledger = existsSync(LEDGER) ? ledgerNames(readLedgerTsv(LEDGER)) : undefined;
    const mangled = hasMangled
      ? readMangledPairs(join(PLAIN, 'symbols.mangled.txt'), join(PLAIN, 'symbols.demangled.txt'))
      : undefined;
    cachedOpts = {
      names: liftNames(m, {
        imports: glue?.imports ?? null,
        exports: glue?.exports ?? null,
        ledger,
        ...(mangled ? { mangled } : {}),
      }),
      ...(mangled ? { mangled } : {}),
    };
  }
  return cachedOpts;
}

function lift(index: number): string {
  return liftFunction(target(), index, opts()).source;
}

function indexNamed(name: string): number {
  for (const [index, n] of opts().names ?? []) {
    if (n === name && index >= target().importedFuncCount) {
      return index;
    }
  }
  throw new Error(`no function named ${name}`);
}

describe('liftFunction', () => {
  it('lifts every defined function of the target into a definition of its own symbol with a body', () => {
    const m = target();
    const threw: string[] = [];
    const stubs: number[] = [];
    for (const f of m.funcs) {
      try {
        const lifted = liftFunction(m, f.index, opts());
        const def = lifted.source.indexOf(` ${lifted.name}(`, lifted.source.indexOf('\nextern "C"'));
        const body = def < 0 ? '' : lifted.source.slice(lifted.source.indexOf(') {\n', def));
        // a body of more than its closing `end` (one byte) spells a statement or a stand-in
        if (def < 0 || (f.size - f.codeStart > 1 && !body.includes(';') && !body.includes('/* wasm: '))) {
          stubs.push(f.index);
        }
      } catch (e) {
        threw.push(`${f.index}: ${String(e)}`);
      }
    }
    expect(threw).toEqual([]);
    expect(stubs).toEqual([]);
    expect(m.funcs.length).toBe(14343);
  }, 120_000);

  it('spells a leaf with loops as do/while over typed loads, under its ledger name', () => {
    const s = lift(156);
    expect(s).toContain('extern "C" int32_t strlen(int32_t p0) {');
    expect(s).toContain('} while (*(uint8_ma*)l1);');
    expect(s).toContain('l3 = *(int32_ma*)l2;');
    expect(s).not.toContain('for (;;)');
  });

  it.skipIf(!hasMangled)(
    'turns a loop whose first statement exits it into a while, named by the mangled symbol',
    () => {
      const s = lift(7817);
      expect(s).toContain('extern "C" int32_t _ZNKSt3__25ctypeIwE11do_scan_notEmPKwS3_(');
      expect(s).toContain('  while (p2 != p3) {');
      expect(s).toContain('if ((uint32_t)p0 > 127u) break;');
    },
  );

  it('recovers the shadow-stack frame into local objects and drops the stack-pointer writes', () => {
    const s = lift(10058);
    expect(s).toContain('uint8_t frame_48[16];');
    expect(s).toContain('*(volatile int32_ma*)frame_48 = p1;');
    expect(s).toContain('(int32_t)frame_48)');
    expect(s).not.toMatch(/__stack_pointer|__builtin_frame_address|\bg0\b/);
  });

  it('splits a frame into one object per address taken as a value, declared top first past a jump', () => {
    expect(lift(11632)).toContain(
      [
        '  goto frame_objects;',
        '  alignas(8) uint8_t frame_48[16];',
        '  alignas(4) uint8_t frame_32[16];',
        '  alignas(4) uint8_t frame_16[16];',
        '  alignas(8) uint8_t frame_0[16];',
        '  frame_objects:;',
      ].join('\n'),
    );
  });

  it.skipIf(!hasMangled)(
    'lets an object read or written before its address is taken escape at entry once one below it has',
    () => {
      const s = lift(11432);
      expect(s).toContain('__asm__ volatile("" :: "r"(frame_464)); /* wasm: may be part of frame_432 */');
      const store = s.indexOf('*(volatile int8_ma*)(frame_464 + 4) = 0;');
      expect(s).toContain('if (*(uint8_ma*)(frame_464 + 4) == 1) {');
      expect(store).toBeGreaterThan(0);
      expect(store).toBeLessThan(s.indexOf('_ZNSt3__26localeD1Ev((int32_t)frame_464)'));
    },
  );

  it('outlines a callee wasm-opt inlined with its own frame into a static noinline function', () => {
    const s = lift(9127);
    expect(s).toContain(
      'static __attribute__((noinline)) void f9127_inlined0(int32_t p0, int32_t p1, int32_t p2, int32_t p3, int32_t p4, int32_t p5) {',
    );
    expect(s).toContain('  alignas(4) uint8_t frame_164[12];');
    expect(s).toContain('  f9127_inlined0(p0, (int32_t)frame_0, p2, l3, l5, l6);');
    expect(s).toContain('extern "C" void f9127(int32_t, int32_t, int32_t);');
    expect(s.indexOf('f9127_inlined0(int32_t')).toBeLessThan(s.indexOf('extern "C" void f9127(int32_t p0'));
  });

  it('passes an outlined callee the value the caller left on the stack before its frame', () => {
    const s = lift(270);
    expect(s).toContain('static __attribute__((noinline)) void f270_inlined0(int32_t p0) {');
    expect(s).toContain('  l2 = f3598(p0);');
    expect(s).toContain('    l3 = f137(132);\n    f270_inlined0(l3);');
  });

  it('keeps a frame nothing reads with an empty asm that takes its address', () => {
    expect(lift(10251)).toContain(
      [
        '  alignas(4) uint8_t frame[480];',
        '  __asm__ volatile("" :: "r"(frame)); /* wasm: stack the target reserves and nothing here reads */',
      ].join('\n'),
    );
  });

  it('evaluates both arms of a select that reads memory before choosing', () => {
    const s = lift(3477);
    expect(s).toContain('t0 = *(int32_ma*)frame_4;');
    expect(s).toContain('f194(p0, l3 ? t0 : t1, l3 ? t2 : t3);');
  });

  it('casts a computed table slot to a function pointer for call_indirect', () => {
    expect(lift(10058)).toContain('p2 = ((int32_t(*)(int32_t))*(int32_ma*)((char*)*(int32_ma*)p1 + 8))(p1);');
  });

  it.skipIf(!hasMangled)('declares a callee under its mangled name with the demangled one beside it', () => {
    expect(lift(10058)).toContain(
      'extern "C" int32_t _ZN7RHVoice4item8iteratormmEv(int32_t); // RHVoice::item::iterator::operator--()',
    );
  });

  it('keeps an invoke_* thunk call, naming the callee a constant table slot holds', () => {
    const s = lift(5528);
    expect(s).toContain('extern "C" void invoke_vi(int32_t, int32_t);');
    expect(s).toContain('invoke_vi(t0, p0);');
    expect(lift(indexNamed('votaTick'))).toMatch(
      /invoke_vi\(\d+ \/\* &\w+ \*\/, \(int32_t\)\(\(uint32_t\)l2 \+ 36u\)\);/,
    );
  });

  it('spells br_table as a switch of branches', () => {
    const s = lift(6283);
    expect(s).toContain('switch (*(int32_ma*)((char*)p0 + 8)) {');
    expect(s).toMatch(/case 0: goto L\d+;/);
    expect(s).toContain('default: return;');
  });

  it('wraps integer arithmetic as wasm does, where a signed overflow check would fold away', () => {
    const s = lift(9666);
    expect(s).toContain('p0 = (int32_t)(0u - (uint32_t)p0);');
    expect(s).toContain('if (t0 != (p0 < 0)) goto L2;');
    expect(s).toContain('l2 = (int32_t)((uint32_t)r0 + ((uint32_t)p1 + 36u));');
  });

  it('masks a variable shift count the way wasm does', () => {
    const s = lift(3240);
    expect(s).toContain(' >> (l4 & 31))');
    expect(s).not.toMatch(/>> l4\b/);
  });

  it('reads and writes memory through may_alias types unless the flags turn strict aliasing off', () => {
    const strict = liftFunction(target(), 1549, { ...opts(), flags: COMPONENT_FLAGS.RHVoice }).source;
    expect(strict).toContain('typedef int32_t int32_ma __attribute__((may_alias));');
    expect(strict).toMatch(/\*\(int64_(ma|a\d)\*\)/);
    // ecourna and uenux2 compile with -fno-strict-aliasing
    for (const flags of [COMPONENT_FLAGS.ecourna, COMPONENT_FLAGS.vota]) {
      const relaxed = liftFunction(target(), 1549, { ...opts(), flags }).source;
      expect(relaxed).not.toContain('may_alias');
      expect(relaxed).toContain('*(int32_t*)');
    }
  });

  it('keeps loads after the call the wasm makes first', () => {
    const s = lift(782);
    const call = s.indexOf('t0 = __cxa_allocate_exception(40);');
    expect(call).toBeGreaterThan(0);
    expect(s.indexOf('l4 = *(int32_ma*)p1;')).toBeGreaterThan(call);
  });

  it('spills a store address that reads memory before a value that calls', () => {
    expect(lift(8905)).toContain(
      [
        '  t0 = (int32_t)((uint32_t)l3 + (uint32_t)*(int32_ma*)(int32_t)((uint32_t)*(int32_ma*)l3 - 12u));',
        '  t1 = strlen(p2);',
        '  *(volatile int32_ma*)((char*)t0 + 12) = (int32_t)(((uint32_t)t1 + (uint32_t)*(int32_ma*)((char*)p0 + 8)) + 2u);',
      ].join('\n'),
    );
  });

  it('spells a dropped load as a volatile read', () => {
    expect(lift(3477)).toContain('(void)*(volatile int32_ma*)(frame_4 + 8);');
  });

  it('recognises a variable-length stack allocation through a saved stack pointer or the frame base', () => {
    const getcwd = lift(3984);
    expect(getcwd).toContain('l4 = (int32_t)__builtin_alloca(p0 ? 16 : 4096);');
    expect(getcwd).not.toContain('__stack_pointer');
    expect(lift(2526)).toContain('l7 = (int32_t)__builtin_alloca((int32_t)((uint32_t)p1 + 15u) & (-16));');
  });

  it('aligns a frame object no more than an under-aligned access at its start allows', () => {
    const s = lift(11632);
    expect(s).toContain('*(int64_a4*)frame_32;');
    expect(s).toContain('alignas(4) uint8_t frame_32[16];');
  });

  it('spells a base plus a scaled index as pointer arithmetic, base first', () => {
    expect(lift(6477)).toContain('l1 = *(int32_ma*)((char*)l4 + (l2 << 4) + 4);');
  });

  it('keeps a scaled sum held in a local as pointer arithmetic, which LLVM leaves base first', () => {
    expect(lift(4809)).toContain('l3 = (int32_t)((char*)p0 + (l6 << 2));');
  });

  it('declares the frame of a callee left inlined above the function own, at its wasm offsets', () => {
    const s = lift(9866);
    expect(s.indexOf('alignas(4) uint8_t frame1[32];')).toBeGreaterThan(0);
    expect(s.indexOf('alignas(4) uint8_t frame1[32];')).toBeLessThan(s.indexOf('alignas(4) uint8_t frame[64];'));
  });

  it('keeps the bytes a constant-length fill covers in one frame object', () => {
    const s = lift(5270);
    expect(s).toContain('alignas(4) uint8_t frame_20[136];');
    expect(s).toContain('__builtin_memset((void*)(int32_t)frame_20, 0, 136);');
  });

  it("keeps an arm's last call in the arm with an asm SimplifyCFG cannot sink into the join", () => {
    const s = lift(3020);
    expect(s).toContain('#define KEEP_IN_ARM() __asm__ volatile("")');
    expect(s).toContain(
      ['((void(*)(int32_t))*(int32_ma*)1798424)(l1);', '      KEEP_IN_ARM();', '      goto L0;'].join('\n'),
    );
    expect(s).toContain(['((void(*)(int32_t))*(int32_ma*)1798372)(l1);', '    KEEP_IN_ARM();', '  }'].join('\n'));
  });

  it('calls memmove without the test of its length that LLVM lowers it behind', () => {
    const s = lift(750);
    expect(s).toContain('\n  __builtin_memmove((void*)l4, (void*)l2, l5);\n');
    expect(s).not.toContain('if (l5) {');
  });

  it.skipIf(!hasMangled)('declares a parameter a call site sign-extends from 8 bits as int8_t', () => {
    expect(lift(673)).toContain(
      'extern "C" void _ZNSt3__212basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9push_backEc(int32_t, int8_t);',
    );
  });

  it('hides the constant address of an under-aligned access from alignment inference', () => {
    const s = lift(9715);
    expect(s).toContain('*(volatile int32_a1*)(frame_8 + 3) = *(int32_a1*)opaque_addr(50102);');
    expect(s).toContain('static inline int32_t opaque_addr(int32_t a) {');
  });

  it('writes every store as volatile, in the wasm order and width', () => {
    expect(lift(8164)).toContain(
      ['  *(volatile int32_ma*)((char*)p0 + 20) = 1164512;', '  *(volatile int32_ma*)((char*)p0 + 16) = 1164480;'].join(
        '\n',
      ),
    );
  });

  it('renames a callee whose name a generated local would shadow', () => {
    const names = new Map(opts().names);
    const callee = 143;
    names.set(callee, 't0');
    const s = liftFunction(target(), 782, { ...opts(), names }).source;
    expect(s).toContain('extern "C" int32_t t0_(int32_t) __asm__("t0");');
    expect(s).toContain('t0 = t0_(40);');
  });

  it('gives main an asm label, since C++ forbids an extern "C" main with these parameter types', () => {
    expect(lift(indexNamed('main'))).toContain('extern "C" int32_t main_(int32_t p0, int32_t p1) __asm__("main");');
  });

  it.skipIf(!hasImage)(
    'produces sources em++ compiles at the vota flags',
    () => {
      const m = target();
      const dir = mkdtempSync('/tmp/wasm-lift-');
      const indices = [
        156,
        7817,
        10058,
        11432,
        10251,
        9127,
        270,
        5892,
        5528,
        6283,
        3020,
        750,
        673,
        9715,
        5270,
        9866,
        indexNamed('votaTick'),
        indexNamed('main'),
      ];
      for (let k = 0; k < m.funcs.length; k += 500) {
        indices.push(m.funcs[k].index);
      }
      const files = indices.map((index) => {
        const file = `${index}.cpp`;
        writeFileSync(
          join(dir, file),
          liftFunction(m, index, { ...opts(), flags: '-O2 -fPIC -std=c++20 -DNDEBUG -fno-strict-aliasing' }).source,
        );
        return file;
      });
      expect(compileLifted(dir, files)).toEqual([]);
    },
    300_000,
  );
});

describe('liftFlags', () => {
  const trees = readTreeEvidence(URNA_ROOT)!;
  const component = (index: number) => readAttribution(`${URNA_ROOT}/ledger/func-attribution.tsv`).get(index)?.component;

  it("gives a uenux2 function in group ecourna uenux2's flags, by its source_location record", () => {
    expect(component(1260)).toBe('ecourna');
    const f = liftFlags(1260, 'ecourna', trees);
    expect(f).toMatchObject({ flags: TREE_FLAGS.uenux2, by: 'record', of: 'uenux2' });
    expect(f.flags).toBe('-O2 -fPIC -std=c++20 -DNDEBUG -fno-strict-aliasing');
    expect(f.why).toBe("uenux2's flags: a std::source_location record names /home/rubio/tse/uenux2/src/api/util/cdirreader.cpp");
    const head = liftFunction(target(), 1260, { ...opts(), flags: f.flags, flagsWhy: f.why }).source.split('\n');
    expect(head.slice(1, 3)).toEqual([`// em++ ${f.flags}`, `// ${f.why}`]);
  });

  it("keeps ecourna's flags for ecourna's own code, by its record or, with none, by its component", () => {
    expect(liftFlags(336, component(336), trees)).toMatchObject({ flags: COMPONENT_FLAGS.ecourna, by: 'record', of: 'ecourna' });
    expect(liftFlags(170, component(170), trees)).toMatchObject({ flags: COMPONENT_FLAGS.ecourna, by: 'component', of: 'ecourna' });
  });

  it("keeps a library function's component flags", () => {
    expect(component(241)).toBe('sqlite');
    expect(liftFlags(241, 'sqlite', trees)).toEqual({
      flags: '-O3 -fPIC',
      by: 'component',
      of: 'sqlite',
      why: "component sqlite's flags: no record or stub names its tree",
    });
    expect(liftFlags(172, component(172), trees)).toMatchObject({ flags: TREE_FLAGS.uenux2, by: 'component', of: 'libc++' });
  });

  it('takes the tree of the stub when no record decides, and records that name two trees decide nothing', () => {
    const conan = '/home/rubio/.conan2/p/b/libecea1da310e5107';
    const ev: TreeEvidence = {
      records: new Map([
        [1, [`${conan}/b/src/ecourna/api/io/cfile.cpp`, '/home/rubio/tse/uenux2/src/api/io/cinistrings.cpp']],
        [2, [`${conan}/p/include/ecourna/api/asn/iconversorasn.hpp`]],
      ]),
      stubs: new Map([[1, 'src/asn1/BEREncoder.cxx']]),
    };
    expect(liftFlags(1, 'ecourna', ev)).toMatchObject({ flags: TREE_FLAGS.asn1, by: 'stub', of: 'asn1' });
    expect(liftFlags(2, 'vota', ev)).toMatchObject({ flags: COMPONENT_FLAGS.vota, by: 'component' });
    expect(liftFlags(1, 'ecourna', { ...ev, stubs: new Map() }).why).toBe(
      "component ecourna's flags: its records name ecourna and uenux2",
    );
    expect(recordTree(`${conan}/p/include/ecourna/api/pattern/cbasetype.hpp`)).toBeNull();
    expect(recordTree(`${conan}/b/src/ecourna/api/pattern/cbasetype.hpp`)).toBe('ecourna');
    expect(recordTree('/home/rubio/tse/uenux2/mock/app/comum/cappinfobuilder.cpp')).toBe('uenux2');
  });

  it('reads the stubs of src/ but not those of a tree unplaced.cpp, and liftInputs hands the evidence to the CLI', () => {
    expect(trees.stubs.size).toBeGreaterThan(0);
    expect([...trees.stubs.values()].some((f) => f.endsWith('unplaced.cpp'))).toBe(false);
    const inputs = liftInputs(TARGET, {}, () => {});
    expect(liftFlags(1260, inputs.attribution?.get(1260)?.component, inputs.trees).by).toBe('record');
  });
});

describe('liftNames', () => {
  it('gives no symbol to two functions, keeping an export over a ledger guess', () => {
    const names = opts().names!;
    const bySymbol = new Map<string, number[]>();
    for (const [index, name] of names) {
      const c = cName(name, opts().mangled);
      const sym = c.label ?? c.ident;
      bySymbol.set(sym, [...(bySymbol.get(sym) ?? []), index]);
    }
    expect([...bySymbol].filter(([, v]) => v.length > 1)).toEqual([]);
    expect(bySymbol.get('__wasm_call_ctors')).toHaveLength(1);
  });
});

describe('cName', () => {
  it('maps a demangled name to its mangled symbol, and to a readable identifier without one', () => {
    const mangled = new Map([['std::__2::locale::~locale()', '_ZNSt3__26localeD2Ev']]);
    expect(cName('std::__2::locale::~locale()', mangled)).toEqual({
      ident: '_ZNSt3__26localeD2Ev',
      pretty: 'std::__2::locale::~locale()',
    });
    expect(cName('std::__2::locale::~locale()').ident).toBe('std____2__locale__dtor_locale');
    expect(cName('void ns::f<int>(int, std::vector<int>)').ident).toBe('ns__f');
  });

  it('labels a symbol that is not an identifier', () => {
    expect(cName('byn$mgfn-shared$x.1')).toEqual({ ident: 'byn$mgfn_shared$x_1_', label: 'byn$mgfn-shared$x.1' });
    expect(cName('main')).toEqual({ ident: 'main_', label: 'main' });
  });
});
