// The glue and import-name files beside a module on disk; the parsing itself is in imports.ts,
// which reads no file so that a browser can bundle it.
import { existsSync, readFileSync } from 'node:fs';

import { type ExportNames, type ImportNames, exportNamesFromGlue, importNamesFromGlue } from './imports';

export interface GlueNames {
  imports: ImportNames;
  exports: ExportNames;
  from: string;
}

/** Both tables from the glue beside a module (x.wasm → x.js), or null when there is none. */
export function glueNamesFor(wasmPath: string): GlueNames | null {
  const glue = siblingGluePath(wasmPath);
  if (glue === null) {
    return null;
  }
  const js = readFileSync(glue, 'utf8');
  const imports = importNamesFromGlue(js);
  return imports === null ? null : { imports, exports: exportNamesFromGlue(js), from: glue };
}

export function readImportNames(path: string): ImportNames {
  const parsed: unknown = JSON.parse(readFileSync(path, 'utf8'));
  if (parsed === null || typeof parsed !== 'object' || Array.isArray(parsed)) {
    throw new Error(`${path}: an import-names file is a JSON object { field: name }`);
  }
  return parsed as ImportNames;
}

/** The glue emcc wrote beside a linked module: `x.wasm` → `x.js`, `x` → `x.js`. */
export function siblingGluePath(wasmPath: string): string | null {
  const js = wasmPath.endsWith('.wasm') ? `${wasmPath.slice(0, -5)}.js` : `${wasmPath}.js`;
  return existsSync(js) ? js : null;
}

/** The names for a module: an explicit file, else the sibling glue's table, else none. */
export function importNamesFor(wasmPath: string, explicit?: string): { names: ImportNames; from: string } | null {
  if (explicit !== undefined) {
    return { names: readImportNames(explicit), from: explicit };
  }
  const glue = siblingGluePath(wasmPath);
  if (glue === null) {
    return null;
  }
  const names = importNamesFromGlue(readFileSync(glue, 'utf8'));
  return names === null ? null : { names, from: glue };
}
