// Import and export names recovered from the glue, on the real target's glue.
import { readFileSync } from 'node:fs';
import { describe, expect, it } from 'vitest';

import { exportNamesFromGlue, exportResolver, importNamesFromGlue, importResolver } from '../src/imports';
import { normalizeModule } from '../src/normalize';
import { findFunction } from '../src/object';
import { TARGET, target } from './helpers';

const glue = readFileSync(TARGET.replace(/\.wasm$/, '.js'), 'utf8');

describe('importNamesFromGlue', () => {
  const names = importNamesFromGlue(glue)!;

  it('names every imported function of the target', () => {
    const m = target();
    expect(Object.keys(names)).toHaveLength(m.importedFuncCount);
    for (const imp of m.imports) {
      if (imp.kind === 'func') {
        expect(names[imp.name], imp.name).toBeDefined();
      }
    }
  });

  it('strips exactly one leading underscore and keeps EM_JS and invoke names', () => {
    expect(names.o).toBe('__cxa_begin_catch');
    expect(names.b).toBe('__cxa_throw');
    expect(names.K).toBe('fd_write');
    expect(names.la).toBe('_abort_js');
    expect(names.Ia).toBe('js_init');
    expect(names.Xa).toBe('invoke_d');
  });

  it('reads a shorthand property as its own name', () => {
    expect(importNamesFromGlue('var wasmImports={invoke_ii,a:_fd_write}')).toEqual({
      invoke_ii: 'invoke_ii',
      a: 'fd_write',
    });
    expect(importNamesFromGlue('no table here')).toBeNull();
  });

  it('puts the real name in the rows', () => {
    const m = target();
    const rows = normalizeModule(m, { depth: 0, importName: importResolver(names) }).flatMap((n) => n.rows);
    const imports = rows.filter((r) => r.startsWith('call import:'));
    expect(imports.length).toBeGreaterThan(1000);
    expect(imports).toContain('call import:__cxa_throw');
    expect(imports.some((r) => /^call import:[a-zA-Z]{1,2}$/.test(r))).toBe(false);
  }, 30_000);
});

describe('exportNamesFromGlue', () => {
  const names = exportNamesFromGlue(glue);

  it('maps every minified export of the target to its name, ctors included', () => {
    const m = target();
    for (const e of m.exports) {
      if (e.kind === 'func' || e.kind === 'table') {
        expect(names[e.name], e.name).toBeDefined();
      }
    }
    expect(names.Eb).toBe('votaInit');
    expect(names.Kb).toBe('main');
    expect(names.Nb).toBe('__cxa_decrement_exception_refcount');
    expect(names.Db).toBe('__wasm_call_ctors');
    expect(names.Fb).toBe('__indirect_function_table');
  });

  it('lets findFunction take the real name', () => {
    const m = target();
    const fn = findFunction(m, { kind: 'export', name: '__wasm_call_ctors' }, { exportName: exportResolver(names) });
    expect(fn.index).toBe(14478);
    expect(() => findFunction(m, { kind: 'export', name: 'votaInit' })).toThrow(/no exported function/);
  });
});
