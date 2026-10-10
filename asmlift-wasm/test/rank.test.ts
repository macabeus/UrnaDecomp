// The compile-and-rank adapter end to end, through the project's pinned image (emsdk 6.0.0 at -Oz,
// the default template): three candidates of one function, linked into a module each, scored
// against a linked "target" built from one of them. Skips when the image is not present
// (`docker pull emscripten/emsdk:6.0.0`).
import { spawnSync } from 'node:child_process';
import { mkdtempSync, readFileSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';
import { describe, expect, it } from 'vitest';

import { exportNamesFromGlue, exportResolver, importNamesFromGlue, importResolver } from '../src/imports';
import { findFunction } from '../src/object';
import { parseModule } from '../src/parse';
import { DEFAULT_IMAGE, candidatesFrom, defaultTemplate, rankWasm } from '../src/rank';

const hasImage = spawnSync('docker', ['image', 'inspect', DEFAULT_IMAGE], { encoding: 'utf8' }).status === 0;

// Docker reaches the compile directory through a bind mount of /tmp; the runner creates it under
// os.tmpdir(), which on macOS is a per-user /var/folders path the daemon cannot see.
process.env.TMPDIR = '/tmp';

const TEMPLATE = defaultTemplate();

const A = `
#include <string.h>
extern "C" int helper(int);
static const char table[] = "abcdefghijklmnop";
extern "C" int foo(int x, const char* s) {
  int n = (int)strlen(s);
  if (n > 3) return helper(x + table[n & 15]);
  return x * 7 + n;
}
`;
const B = A.replace('x * 7', 'x * 9');
const C = A.replace('n > 3', 'n >= 3').replace('x * 7', 'x * 9');

describe('rankWasm', () => {
  it.skipIf(!hasImage)(
    'ranks the candidate that reproduces the target first, at score 0 and byte-exact',
    async () => {
      const dir = mkdtempSync('/tmp/wasm-rank-');
      writeFileSync(join(dir, 'a.cpp'), A);
      const built = spawnSync(
        'sh',
        [
          '-ec',
          TEMPLATE.replace('{{flags}}', '')
            .replace('{{inputPath}}', join(dir, 'a.cpp'))
            .replaceAll('{{outputPath}}', join(dir, 'target'))
            .replace('{{symbol}}', 'foo'),
        ],
        { encoding: 'utf8' },
      );
      expect(built.status, built.stderr).toBe(0);
      const target = parseModule(new Uint8Array(readFileSync(join(dir, 'target'))));
      // -Oz minifies the export: `foo` is `b` or so, and only the glue beside the module says which
      const glue = readFileSync(join(dir, 'target.js'), 'utf8');
      const exportNames = exportNamesFromGlue(glue);
      expect(Object.values(exportNames)).toContain('foo');
      expect(target.exports.find((e) => e.name === 'foo')).toBeUndefined();
      // and the import `helper` is minified too: without the glue's names the target's row reads
      // `import:<letter>` while the candidate's reads `import:helper`, one row off a perfect match
      const importNames = importNamesFromGlue(glue);
      expect(importNames).not.toBeNull();
      expect(Object.values(importNames!)).toContain('helper');
      const index = findFunction(
        target,
        { kind: 'export', name: 'foo' },
        { exportName: exportResolver(exportNames) },
      ).index;

      const { ranked, compiled } = await rankWasm(
        { module: target, index },
        candidatesFrom([
          { name: 'b', source: B },
          { name: 'a', source: A },
          { name: 'c', source: C },
          { name: 'broken', source: 'int foo(int x) { return x +; }' },
        ]),
        { extract: { kind: 'export', name: 'foo' }, ext: 'cpp', importName: importResolver(importNames) },
      );
      expect(ranked.winner.variations).toEqual(['a']);
      expect(ranked.winner.score.score).toBe(0);
      expect(ranked.winner.score.byteExact).toBe(true);
      expect(ranked.candidates.map((c) => c.variations[0])).toEqual(['a', 'b', 'c']);
      expect(ranked.candidates[1].score.score).toBe(1);
      expect(ranked.candidates[1].score.breakdown.operandOnly).toBe(1);
      expect(ranked.candidates[2].score.score).toBeGreaterThan(1);
      expect(ranked.dropped).toHaveLength(1);
      expect(ranked.dropped[0].error).toMatch(/rejected|error/);
      expect(compiled.get(A)?.relocatable).toBe(false);
    },
    180_000,
  );
});
