import { describe, expect, it } from 'vitest';

import { codegenFlags, effectiveFlags, liftCompileFlags, splitFlags } from '../src/flags';

describe('splitFlags', () => {
  it('splits words and keeps quoted text whole', () => {
    expect(splitFlags(`-O3 '-DPACKAGE="RHVoice"' -DX="a b"  -c`)).toEqual(['-O3', '-DPACKAGE="RHVoice"', '-DX=a b', '-c']);
  });
});

describe('codegenFlags', () => {
  it('drops include paths, path maps, warnings, debug info, the output and the input', () => {
    expect(
      codegenFlags(splitFlags('-O2 -Isrc -ffile-prefix-map=/a=/b -Wall -Wno-x -g -sSTRICT -c x.cpp -o x.o -fPIC -DNDEBUG')),
    ).toEqual(['-O2', '-fPIC', '-DNDEBUG']);
  });
});

describe('effectiveFlags', () => {
  it('spells three ways of turning PIC off the same', () => {
    const base = '-O3 -fexceptions -std=c++20 -DNDEBUG';
    expect(effectiveFlags(`${base} -fPIC -fno-PIC`)).toBe(effectiveFlags(`${base} -fno-PIC -fno-pic`));
    expect(effectiveFlags(`${base} -fno-pic`)).toBe(effectiveFlags(base));
  });

  it('takes the last -O and the last exceptions switch', () => {
    expect(effectiveFlags('-Oz -fexceptions -O3')).toBe('-O3 -fexceptions');
    expect(effectiveFlags('-Os -fno-exceptions -fexceptions -fignore-exceptions')).toBe('-Os -fexceptions -fignore-exceptions');
  });

  it('does not equate flag sets that differ in a define', () => {
    expect(effectiveFlags('-O3 -DA')).not.toBe(effectiveFlags('-O3 -DB'));
  });
});

describe('liftCompileFlags', () => {
  it('drops defines and a C -std but keeps a C++ -std', () => {
    expect(liftCompileFlags('-O2 -std=c99 -D_XOPEN_SOURCE=700 -Os -fno-builtin')).toBe('-O2 -Os -fno-builtin');
    expect(liftCompileFlags(`-O3 -std=c++14 '-DPACKAGE="RHVoice"' -DNDEBUG`)).toBe('-O3 -std=c++14');
  });
});
