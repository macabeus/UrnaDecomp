// The registry against the real build: src/Makefile's dry run, the library builds' recorded flags and
// emscripten's own system-library flags in the pinned image (skipped without it).
import { existsSync } from 'node:fs';
import { join } from 'node:path';
import { describe, expect, it } from 'vitest';

import { URNA_ROOT } from '../src/paths';
import type { Setup } from '../src/schema';
import { classifyObjects, loadSetups } from '../src/setups';
import { IMAGE, hasImage } from './helpers';

function edited(key: string, edit: (s: Setup) => Setup): Setup[] {
  return loadSetups().map((s) => (s.key === key ? edit(structuredClone(s)) : s));
}

describe('loadSetups', () => {
  it('reads a registry whose setups all name the pinned image', () => {
    const setups = loadSetups();
    expect(setups.length).toBeGreaterThan(10);
    expect(new Set(setups.map((s) => s.image))).toEqual(new Set(['urna-emsdk:6.0.0']));
  });
});

describe('classifyObjects', () => {
  it('finds every object of every Makefile setup compiling with its flags', () => {
    const { checks, objects } = classifyObjects(loadSetups(), IMAGE);
    const make = loadSetups().filter((s) => s.truth.kind === 'make');
    for (const s of make) {
      const c = checks.find((x) => x.key === s.key)!;
      expect(c.status, `${s.key}: ${c.detail.join('; ')}`).toBe('ok');
      expect(c.objects).toBeGreaterThan(0);
    }
    expect(objects.find((o) => o.object === 'build/out/app/asn1/BERDecoderPartial.o')?.setup).toBe('asn1-eh');
    expect(objects.find((o) => o.object === 'build/out/app/ecourna/api_security_base64.cpp.o')?.setup).toBe('ecourna');
    expect(objects.find((o) => o.object === 'build/out/app/ecourna/api_io_cfile.cpp.o')?.setup).toBe('ecourna-nopic');
  });

  it('reports drift when a setup no longer has the flags its objects compile with', () => {
    const { checks } = classifyObjects(
      edited('ecourna-nopic', (s) => ({ ...s, flags: { 'c++': s.flags!['c++']!.replace('-O3', '-O2') } })),
      IMAGE,
    );
    const c = checks.find((x) => x.key === 'ecourna-nopic')!;
    expect(c.status).toBe('drift');
    expect(c.detail.some((d) => d.includes('api_io_cfile.cpp.o') && d.includes('no setup has'))).toBe(true);
  });

  it("counts the stand-ins compiling with the stand-ins' flags, keeps them stand-ins, and reports drift", () => {
    const { checks, objects, standIns } = classifyObjects(loadSetups(), IMAGE);
    const c = checks.find((x) => x.key === 'app-stand-ins')!;
    expect(c.status, c.detail.join('; ')).toBe('ok');
    expect(c.objects).toBeGreaterThan(0);
    expect(standIns).toContain('build/out/app/roots_boost.o');
    expect(objects.some((o) => o.setup === 'app-stand-ins')).toBe(false);
    const edit = classifyObjects(
      edited('app-stand-ins', (s) => ({ ...s, flags: { 'c++': `${s.flags!['c++']} -fno-strict-aliasing` } })),
      IMAGE,
    );
    expect(edit.checks.find((x) => x.key === 'app-stand-ins')!.status).toBe('drift');
  });

  it.skipIf(!existsSync(join(URNA_ROOT, 'build/out/sqlite/O3pic2/cflags.txt')))(
    'reports drift when a library build recorded other flags',
    () => {
      const { checks } = classifyObjects(
        edited('sqlite', (s) => ({ ...s, flags: { c: s.flags!.c!.replace('-O3', '-O2') } })),
        IMAGE,
      );
      expect(checks.find((x) => x.key === 'sqlite')!.status).toBe('drift');
      expect(checks.find((x) => x.key === 'openssl')!.status).not.toBe('drift');
    },
  );

  it.skipIf(!hasImage)('agrees with emscripten on every system library and catches a changed one', () => {
    const ok = classifyObjects(loadSetups(), IMAGE).checks.filter((c) => c.status !== 'ok');
    expect(ok.map((c) => `${c.key}: ${c.detail.join('; ')}`)).toEqual([]);
    const { checks } = classifyObjects(
      edited('libc', (s) => ({ ...s, flags: { c: s.flags!.c!.replace('-Os', '-Oz') } })),
      IMAGE,
    );
    expect(checks.find((x) => x.key === 'libc')!.status).toBe('drift');
  });
});
