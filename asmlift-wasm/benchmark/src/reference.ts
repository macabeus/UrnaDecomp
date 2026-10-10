// Where the source of a row's function lives: the reconstructed unit an app object compiles, a
// library's upstream file in third_party/, or a system library's source in the pinned image, named by
// the archive member's debug info.
import { existsSync, readFileSync, readdirSync } from 'node:fs';
import { basename, join } from 'node:path';

import { URNA_ROOT } from './paths';
import type { Definition, Definitions } from './provenance';
import type { Reference } from './schema';

const SQLITE_C = 'third_party/sqlite/sqlite3.c';
/** OpenSSL's out-of-tree build directory: its objects sit in the source tree's layout */
const OPENSSL_BUILD = 'build/out/openssl/O3';
const SEVENZIP_SOURCES = 'tools/libs/7zip/sources.txt';
const SEVENZIP_TREE = 'third_party/7zip';
const RHVOICE_TREE = 'third_party/RHVoice';
const SONIC_C = `${RHVOICE_TREE}/external/libs/sonic/sonic.c`;

/** A repo-relative file's text, or null when it is absent. */
export type ReadRepoFile = (path: string) => string | null;

/** The checkout's files as they are now. */
export const readCheckout: ReadRepoFile = (path) => {
  const abs = join(URNA_ROOT, path);
  return existsSync(abs) ? readFileSync(abs, 'latin1') : null;
};

let opensslObjects: Map<string, string> | null = null;
/** libcrypto member name → its object's directory in the build directory, which is its source's in third_party/openssl. */
function opensslObjectDirs(): Map<string, string> {
  if (opensslObjects !== null) {
    return opensslObjects;
  }
  opensslObjects = new Map();
  const walk = (dir: string): void => {
    const abs = join(URNA_ROOT, dir);
    if (!existsSync(abs)) {
      return;
    }
    for (const e of readdirSync(abs, { withFileTypes: true })) {
      if (e.isDirectory()) {
        walk(join(dir, e.name));
      } else if (/^libcrypto-lib-.*\.o$/.test(e.name) && !opensslObjects!.has(e.name)) {
        opensslObjects!.set(e.name, dir.slice(OPENSSL_BUILD.length + 1));
      }
    }
  };
  walk(OPENSSL_BUILD);
  return opensslObjects;
}

/** The repo source of `def`'s object: `source` when the build named it, else by the library's layout. */
export function referenceFor(
  d: Definitions,
  def: Definition,
  setup: string,
  source: string | null,
  read: ReadRepoFile,
): Reference | null {
  if (def.object.startsWith('emsdk:')) {
    const path = d.memberSources[`${def.object}:${def.member}`];
    return path === undefined ? null : { path: `emsdk:${path}` };
  }
  if (source !== null) {
    return { path: source };
  }
  const name = basename(def.member ?? def.object);
  switch (setup) {
    case 'sqlite':
      return { path: SQLITE_C };
    case 'openssl': {
      const dir = opensslObjectDirs().get(name);
      if (dir === undefined) {
        return null;
      }
      return { path: `third_party/openssl/${dir}/${name.replace(/^libcrypto-lib-/, '').replace(/\.o$/, '.c')}` };
    }
    case '7zip': {
      const listed = (read(SEVENZIP_SOURCES) ?? '')
        .split('\n')
        .map((l) => l.split(/\s+/)[0])
        .find((p) => p !== '' && !p.startsWith('#') && `${p.replace(/\//g, '_')}.o` === name);
      return listed === undefined ? null : { path: `${SEVENZIP_TREE}/${listed}` };
    }
    case 'rhvoice': {
      const file = name.replace(/\.o$/, '');
      if (file === 'sonic.c') {
        return { path: SONIC_C };
      }
      const dir = file.startsWith('HTS_') ? 'src/hts_engine' : 'src/core';
      return { path: `${RHVOICE_TREE}/${dir}/${file}` };
    }
    default:
      return null;
  }
}

function escape(s: string): string {
  return s.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}

/** The identifier a definition of `name` spells: `Class::method` for a C++ member, the bare name
 *  otherwise; template arguments and the parameter list dropped. */
function definitionKey(name: string): string | null {
  let depth = 0;
  let head = '';
  for (const c of name) {
    if (c === '<') {
      depth++;
    } else if (c === '>') {
      depth--;
    } else if (c === '(' && depth === 0) {
      break;
    } else if (depth === 0) {
      head += c;
    }
  }
  const parts = head.trim().split(/\s+/).pop()!.split('::');
  const key = parts.slice(-2).join('::');
  return /^[A-Za-z_~][\w:~]*$/.test(key) ? key : null;
}

/** A reference's text, as `read` gives it; null for a file in the image or one that is not here. */
function sourceText(ref: Reference, read: ReadRepoFile): string | null {
  return ref.path.startsWith('emsdk:') ? null : read(ref.path);
}

/** The 1-based line of `name`'s definition in the reference: the one line that starts a definition
 *  of it (not a declaration ending in `;`), or undefined. Lines are numbered as the compiler numbers them:
 *  a `#line N` gives the line after it number N, so in a TSE source the line is the one of TSE's file
 *  that the gaps keep (src/README.md "Gaps"). */
export function definitionLine(ref: Reference, name: string, read: ReadRepoFile): number | undefined {
  const key = definitionKey(name);
  const text = key === null ? null : sourceText(ref, read);
  if (key === null || text === null) {
    return undefined;
  }
  const re = new RegExp(`^(?:[^\\s#/*].*?[^\\w:])?${escape(key)}\\s*\\(`);
  const hits: number[] = [];
  let n = 1;
  for (const line of text.split('\n')) {
    const directive = /^#line (\d+)\s*$/.exec(line);
    if (directive !== null) {
      n = Number(directive[1]);
      continue;
    }
    if (re.test(line) && !/;\s*$/.test(line)) {
      hits.push(n);
    }
    n += 1;
  }
  return hits.length === 1 ? hits[0] : undefined;
}
