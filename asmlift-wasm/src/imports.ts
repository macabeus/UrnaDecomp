// Import and export names. At -O2 and above emcc minifies import fields (`a.o`, `a.Za`) and export
// names (`Eb`), and the only place the real names survive is the glue: the `var wasmImports={o:___cxa_begin_catch,…}`
// table and the `_votaInit=Module["_votaInit"]=wasmExports["Eb"]` assignments. A rebuild
// compares to the target only when both sides spell an import the same way, so the canonical row
// symbol is the REAL name without its module (`import:__cxa_throw`): the module is a function of the
// name (`fd_write` lives in wasi_snapshot_preview1, everything else in env) and carries nothing.
//
// The JS-side name in the table is the C symbol with the glue's leading-underscore convention:
// `___cxa_throw` is the wasm import `__cxa_throw`, `_fd_write` is `fd_write`, while EM_JS and
// `invoke_*` names (`js_init`, `invoke_vii`) carry no prefix. Stripping exactly one leading `_`
// recovers the field: verified on a 6.0.0 probe linked with -sASSERTIONS=1 (which keeps the fields
// unminified), where every one of its 42 table entries obeys the rule. A shorthand property
// (`invoke_ii` with no colon) names both sides at once.
export type ImportNames = Record<string, string>;
/** `{ <minified export>: <real name> }` */
export type ExportNames = Record<string, string>;

function unprefixed(jsName: string): string {
  return jsName.startsWith('_') ? jsName.slice(1) : jsName;
}

/** `{ <field>: <real name> }` from a glue's `var wasmImports={…}` table; null when the glue has none. */
export function importNamesFromGlue(js: string): ImportNames | null {
  const m = /var wasmImports\s*=\s*\{([^}]*)\}/.exec(js);
  if (m === null) {
    return null;
  }
  const out: ImportNames = {};
  for (const entry of m[1].split(',')) {
    const e = entry.trim();
    if (e === '') {
      continue;
    }
    const colon = e.indexOf(':');
    const field = colon < 0 ? e : e.slice(0, colon).trim();
    const jsName = colon < 0 ? e : e.slice(colon + 1).trim();
    out[field] = unprefixed(jsName);
  }
  return out;
}

/** `{ <export>: <real name> }` from the glue's `<js name>=…=wasmExports["<export>"]` assignments; the
 *  one bare `wasmExports["X"]()` call is `__wasm_call_ctors`. Empty when the glue has no exports. */
export function exportNamesFromGlue(js: string): ExportNames {
  const out: ExportNames = {};
  const assign = /([A-Za-z_$][\w$]*)=(?:Module\["[\w$]+"\]=|wasm(?:Memory|Table)=)?wasmExports\["([\w$]+)"\]/g;
  for (let m = assign.exec(js); m !== null; m = assign.exec(js)) {
    const [, jsName, field] = m;
    out[field] = jsName === '__indirect_function_table' || jsName === 'memory' ? jsName : unprefixed(jsName);
  }
  const ctors = /wasmExports\["([\w$]+)"\]\(\)/.exec(js);
  if (ctors !== null && out[ctors[1]] === undefined) {
    out[ctors[1]] = '__wasm_call_ctors';
  }
  return out;
}

/** `findFunction`'s export lookup: the real name's minified export, when the glue knows it. */
export function exportResolver(names: ExportNames | null): (realName: string) => string | undefined {
  return (realName) => {
    if (names === null) {
      return undefined;
    }
    for (const [field, real] of Object.entries(names)) {
      if (real === realName) {
        return field;
      }
    }
    return undefined;
  };
}

/** `normalizeModule`'s `importName`: the map's real name when it has the field, else the field. */
export function importResolver(names: ImportNames | null): (module: string, field: string, funcIndex: number) => string {
  return (_module, field) => names?.[field] ?? field;
}
