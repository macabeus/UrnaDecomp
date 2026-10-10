import { readFileSync } from 'node:fs';
import { describe, expect, it } from 'vitest';

import { glueNamesFor, parseModule } from '@asmlift/wasm';

import { featuresOf } from '../src/features';
import { TARGET } from '../src/paths';

const m = parseModule(new Uint8Array(readFileSync(TARGET)));
const imports = glueNamesFor(TARGET)?.imports ?? null;

describe('featuresOf', () => {
  it('reads a leaf with a frame off a posix mutex lock', () => {
    // api::CPosixMutex::Lock() (uenux2)
    expect(featuresOf(m, 10252, imports)).toEqual(['leaf', 'frame']);
  });

  it('sees musl pow_small branching on a table, in floating point and 64-bit integers', () => {
    expect(featuresOf(m, 1172, imports)).toEqual(expect.arrayContaining(['switch', 'i64', 'float']));
  });

  it('names an invoke_* call and a throw from the real import names', () => {
    const f = featuresOf(m, 10307, imports);
    expect(f).toContain('invoke');
    expect(f).toContain('throw');
    expect(f).not.toContain('leaf');
  });

  it('marks __memcpy a leaf that copies with bulk memory', () => {
    // __memcpy (libc_optz)
    expect(featuresOf(m, 504, imports)).toEqual(['leaf', 'bulk-memory']);
  });
});
