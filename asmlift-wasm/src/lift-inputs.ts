// What the lifter needs besides the module to name what it emits: the glue's import and export
// names, the ledger's names, the component attribution and the mangled spellings of demangled
// names. Under the urna layout (`<root>/target/x.wasm`) every input defaults to its place in the
// checkout around the module.
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';

import { glueNamesFor } from './glue-file';
import { type Attribution, ledgerNames, readAttribution, readLedgerTsv, readMangledPairs } from './ledger';
import { liftNames } from './lift';
import { type Module, parseModule } from './parse';

export interface LiftInputs {
  m: Module;
  names: Map<number, string>;
  mangled: Map<string, string> | undefined;
  attribution: Map<number, Attribution> | null;
}

export interface LiftInputPaths {
  /** the ledger's `functions.tsv` */
  names?: string;
  /** `func-attribution.tsv` */
  attribution?: string;
  /** `<mangled.txt>:<demangled.txt>`, two line-aligned symbol lists of one build */
  mangled?: string;
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
  log(
    `names: ${names.size} (glue ${glue?.from ?? 'none'}, ledger ${ledger?.size ?? 0}), mangled pairs: ${mangled?.size ?? 0}, ` +
      `attribution: ${attribution?.size ?? 0}`,
  );
  return { m, names, mangled, attribution };
}
