// What the lifter needs besides the module to name what it emits: the glue's import and export
// names, the ledger's names, the component attribution and the mangled spellings of demangled
// names; and to choose each lift's compile flags, the component attribution and the evidence of the
// tree a function's code is in (its source_location records, the file of its stub). Under the urna
// layout (`<root>/target/x.wasm`) every input defaults to its place in the checkout around the module.
import { existsSync, readFileSync, readdirSync } from 'node:fs';
import { dirname, join, relative, resolve } from 'node:path';

import { glueNamesFor } from './glue-file';
import { type Attribution, ledgerNames, readAttribution, readLedgerTsv, readMangledPairs } from './ledger';
import { type TreeEvidence, liftNames } from './lift';
import { type Module, parseModule } from './parse';

export interface LiftInputs {
  m: Module;
  names: Map<number, string>;
  mangled: Map<string, string> | undefined;
  attribution: Map<number, Attribution> | null;
  /** what says which tree each function's code is in (`liftFlags`); null outside the urna layout */
  trees: TreeEvidence | null;
}

export interface LiftInputPaths {
  /** the ledger's `functions.tsv` */
  names?: string;
  /** `func-attribution.tsv` */
  attribution?: string;
  /** `<mangled.txt>:<demangled.txt>`, two line-aligned symbol lists of one build */
  mangled?: string;
}

const STUB_TREES = ['src/uenux2', 'src/ecourna', 'src/asn1'];
const STUB_RE = /^\/\/ f(\d+) \u00b7 /gm;
const SOURCE_RE = /\.(cpp|cxx|h|hpp)$/;

function sourceFiles(dir: string): string[] {
  const out: string[] = [];
  for (const e of readdirSync(dir, { withFileTypes: true })) {
    const p = join(dir, e.name);
    if (e.isDirectory()) {
      out.push(...sourceFiles(p));
    } else if (SOURCE_RE.test(e.name)) {
      out.push(p);
    }
  }
  return out;
}

/** The tree evidence of the urna checkout at `root`: every function's source_location record files
 *  (`ledger/source-locations.tsv`, the `funcs` column listing the functions that load a record) and the file
 *  of every stub under src/uenux2, src/ecourna and src/asn1 (urna src/README.md "Stubs"; Latin-1 sources,
 *  the stub's `·` is byte 0xb7). A stub in a tree's unplaced.cpp is left out: the stub generator puts a
 *  function there by its component, which says nothing the component does not. Null when neither input exists. */
export function readTreeEvidence(root: string): TreeEvidence | null {
  const locs = join(root, 'ledger', 'source-locations.tsv');
  const records = new Map<number, string[]>();
  if (existsSync(locs)) {
    for (const line of readFileSync(locs, 'latin1').split('\n').slice(1)) {
      const cols = line.split('\t');
      if (cols.length < 6) {
        continue;
      }
      for (const f of cols[5].split(',')) {
        if (f === '') {
          continue;
        }
        const list = records.get(Number(f));
        if (list === undefined) {
          records.set(Number(f), [cols[1]]);
        } else {
          list.push(cols[1]);
        }
      }
    }
  }
  const stubs = new Map<number, string>();
  for (const tree of STUB_TREES) {
    const dir = join(root, tree);
    if (!existsSync(dir)) {
      continue;
    }
    for (const file of sourceFiles(dir)) {
      if (file.endsWith('/unplaced.cpp')) {
        continue;
      }
      for (const m of readFileSync(file, 'latin1').matchAll(STUB_RE)) {
        stubs.set(Number(m[1]), relative(root, file));
      }
    }
  }
  return records.size === 0 && stubs.size === 0 ? null : { records, stubs };
}

/** The urna layout: `<root>/target/x.wasm` beside `<root>/ledger` and `<root>/tools/libs/cls2.json`. */
export function projectDefaults(wasmPath: string): { ledger?: string; owners?: string } {
  const root = resolve(dirname(wasmPath), '..');
  const ledger = join(root, 'ledger');
  const owners = join(root, 'tools', 'libs', 'cls2.json');
  return {
    ...(existsSync(ledger) ? { ledger } : {}),
    ...(existsSync(owners) ? { owners } : {}),
  };
}

export function liftInputs(
  wasmPath: string,
  paths: LiftInputPaths = {},
  log: (line: string) => void = (l) => process.stderr.write(`${l}\n`),
): LiftInputs {
  const root = resolve(dirname(wasmPath), '..');
  const defaults = projectDefaults(wasmPath);
  const namesPath = paths.names ?? (defaults.ledger && join(defaults.ledger, 'functions.tsv'));
  const attributionPath = paths.attribution ?? (defaults.ledger && join(defaults.ledger, 'func-attribution.tsv'));
  const plain = join(root, 'build', 'out', 'wasm', 'vota_web');
  const mangledSpec =
    paths.mangled ??
    (existsSync(join(plain, 'symbols.mangled.txt'))
      ? `${join(plain, 'symbols.mangled.txt')}:${join(plain, 'symbols.demangled.txt')}`
      : undefined);
  const m = parseModule(new Uint8Array(readFileSync(wasmPath)));
  const glue = glueNamesFor(wasmPath);
  const ledger = namesPath !== undefined && existsSync(namesPath) ? ledgerNames(readLedgerTsv(namesPath)) : undefined;
  const attribution =
    attributionPath !== undefined && existsSync(attributionPath) ? readAttribution(attributionPath) : null;
  const [mangledPath, demangledPath] = mangledSpec?.split(':') ?? [];
  const mangled =
    mangledPath !== undefined && demangledPath !== undefined ? readMangledPairs(mangledPath, demangledPath) : undefined;
  const names = liftNames(m, {
    imports: glue?.imports ?? null,
    exports: glue?.exports ?? null,
    ledger,
    ...(mangled ? { mangled } : {}),
  });
  const trees = readTreeEvidence(root);
  log(
    `names: ${names.size} (glue ${glue?.from ?? 'none'}, ledger ${ledger?.size ?? 0}), mangled pairs: ${mangled?.size ?? 0}, ` +
      `attribution: ${attribution?.size ?? 0}, tree evidence: ${trees?.records.size ?? 0} with records, ${trees?.stubs.size ?? 0} stubs`,
  );
  return { m, names, mangled, attribution, trees };
}
