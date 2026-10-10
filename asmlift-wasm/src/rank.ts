// The compile-and-rank adapter: candidate sources in, a core `rankBy` ranking out. Each candidate
// is compiled by a shell template through @match-kit/compiler (`{{inputPath}}`, `{{outputPath}}`,
// `{{flags}}`), the function is pulled out of what the template wrote (object.ts decides between a
// linked module and a relocatable object by the `linking` section), and scored against the target
// function with score.ts. `rankBy` is synchronous, so every compile runs first and the score
// function it is handed only looks the outcome up.
import { type Candidate, type RankedResult, rankBy } from '@asmlift/core/rank';
import { type Outcome, createRunner } from '@match-kit/compiler';
import { spawnSync } from 'node:child_process';
import { existsSync, readFileSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';

import { exportNamesFromGlue, exportResolver, importNamesFromGlue, importResolver } from './imports';
import { type NormalizeOptions, normalizeModule, signature } from './normalize';
import { type Extract, extractFunction, isRelocatableObject } from './object';
import { type Module, funcAt, parseModule } from './parse';
import { type Score, score } from './score';

/** The project's pinned link: emsdk 6.0.0 at -Oz -fexceptions with the target's link settings (urna
 *  build/README.md). `{{image}}` is filled by `defaultTemplate`. The candidate is linked alone as a
 *  module (`--no-entry`, undefined symbols allowed) with `{{symbol}}` exported; emcc writes the glue to
 *  `{{outputPath}}.js` (left there: its `wasmImports` table names the minified imports) and the module
 *  to `{{outputPath}}.wasm`, renamed to the extension-less path the runner expects. */
export const DEFAULT_IMAGE = 'emscripten/emsdk:6.0.0';
export const DEFAULT_LINK_FLAGS = '-Oz -fexceptions -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB';
export function defaultTemplate(image = DEFAULT_IMAGE, linkFlags = DEFAULT_LINK_FLAGS): string {
  return (
    `docker run --rm -v /tmp:/tmp -w /tmp ${image} em++ ${linkFlags} {{flags}} {{inputPath}} -o {{outputPath}}.js ` +
    `--no-entry -sERROR_ON_UNDEFINED_SYMBOLS=0 -sEXPORTED_FUNCTIONS=_{{symbol}} && mv {{outputPath}}.wasm {{outputPath}}`
  );
}

export interface RankOptions extends NormalizeOptions {
  /** the compile command; `{{inputPath}}`, `{{outputPath}}` required, `{{flags}}` optional. Default `defaultTemplate()` */
  template?: string;
  flags?: readonly string[];
  cwd?: string;
  /** the candidate source's extension (`cpp` default) */
  ext?: string;
  /** how to find the function in what the template wrote */
  extract: Extract;
  /** compiles in flight at once (default 4) */
  concurrency?: number;
}

export type WasmScore = Score;

export interface Compiled {
  /** the module the template wrote, when the compile succeeded */
  module?: Module;
  score?: WasmScore;
  error?: string;
  /** whether the function came from a relocatable object (reloc symbols, no callee fingerprints) */
  relocatable?: boolean;
}

export interface TargetFunction {
  module: Module;
  index: number;
}

/** A plain-source list becomes core candidates: the first is the default (`preference 0`). */
export function candidatesFrom(sources: { name: string; source: string }[]): Candidate[] {
  return sources.map((s) => ({ variations: [s.name], source: s.source, preference: 0 }));
}

/** Compile every candidate and score the extracted function against the target. */
export async function compileAndScore(
  target: TargetFunction,
  candidates: readonly Candidate[],
  opts: RankOptions,
): Promise<Map<string, Compiled>> {
  const targetFn = funcAt(target.module, target.index);
  if (targetFn === null) {
    throw new Error(`target index ${target.index} is not a defined function`);
  }
  const depth = opts.depth ?? 2;
  const normOpts: NormalizeOptions = { depth, ...(opts.importName ? { importName: opts.importName } : {}) };
  const targetNorm = normalizeModule(target.module, normOpts)[target.index - target.module.importedFuncCount];
  // an object's callees are reloc symbols, which no target fingerprint equals: compare the target at depth 0 too
  const targetNormDepth0 = depth === 0 ? targetNorm : null;
  const lazyDepth0 = (): string[] =>
    (
      targetNormDepth0 ??
      normalizeModule(target.module, { ...normOpts, depth: 0 })[target.index - target.module.importedFuncCount]
    ).rows;
  let depth0Rows: string[] | null = null;

  // `{{symbol}}` in the template is the function's export/symbol name (an index selector has none)
  const symbol = opts.extract.kind === 'index' ? null : opts.extract.name;
  // the template's `{{flags}}` must be answered, with no flags when the caller has none
  const runner = createRunner(opts.template ?? defaultTemplate(), {
    ...(opts.cwd ? { cwd: opts.cwd } : {}),
    flags: opts.flags ?? [],
  });
  const results = new Map<string, Compiled>();
  const sources = [...new Set(candidates.map((c) => c.source))];
  const limit = opts.concurrency ?? 4;
  let next = 0;
  const worker = async (): Promise<void> => {
    while (next < sources.length) {
      const source = sources[next++];
      let outcome: Outcome | null = null;
      try {
        outcome = await runner.compile(source, { ext: opts.ext ?? 'cpp', ...(symbol === null ? {} : { symbol }) });
        if (outcome.kind !== 'ok') {
          results.set(source, { error: `${outcome.kind}: ${'output' in outcome ? outcome.output : ''}`.trim() });
          continue;
        }
        const bytes = new Uint8Array(readFileSync(outcome.object));
        const module = parseModule(bytes);
        const relocatable = isRelocatableObject(module);
        // a linked candidate's imports and exports are minified like the target's; its glue, when the
        // template left it beside the module, carries their real names (without one, the bare fields stand)
        const glue = `${outcome.object}.js`;
        const js = existsSync(glue) ? readFileSync(glue, 'utf8') : null;
        const extracted = extractFunction(module, opts.extract, {
          ...normOpts,
          importName: importResolver(js === null ? null : importNamesFromGlue(js)),
          exportName: exportResolver(js === null ? null : exportNamesFromGlue(js)),
        });
        let targetRows = targetNorm.rows;
        if (relocatable) {
          depth0Rows ??= lazyDepth0();
          targetRows = depth0Rows;
        }
        const s = score(
          { rows: targetRows, fn: targetFn, signature: signature(targetFn) },
          { rows: extracted.rows, fn: extracted.fn, signature: extracted.signature },
        );
        results.set(source, { module, score: s, relocatable });
      } catch (e) {
        results.set(source, { error: e instanceof Error ? e.message : String(e) });
      } finally {
        outcome?.[Symbol.dispose]();
      }
    }
  };
  try {
    await Promise.all(Array.from({ length: Math.min(limit, sources.length) }, worker));
  } finally {
    await runner.dispose();
  }
  return results;
}

export interface WasmRanking {
  ranked: RankedResult<WasmScore>;
  compiled: Map<string, Compiled>;
}

/** Compile, score and rank with core's `rankBy`. A candidate whose compile or extraction failed is
 *  thrown to `rankBy`, which records it under `dropped`. */
export async function rankWasm(
  target: TargetFunction,
  candidates: readonly Candidate[],
  opts: RankOptions,
): Promise<WasmRanking> {
  const compiled = await compileAndScore(target, candidates, opts);
  const ranked = rankBy([...candidates], `fn${target.index}`, (source) => {
    const r = compiled.get(source);
    if (r === undefined) {
      throw new Error('internal: candidate was not compiled');
    }
    if (r.score === undefined) {
      throw new Error(r.error ?? 'compile failed');
    }
    return r.score;
  });
  return { ranked, compiled };
}

/** Compile each lifted file of `dir` in one container, at the flags its `// em++ …` header names,
 *  writing `<file>.o` and `<file>.err`; returns the files that failed. */
export function compileLifted(dir: string, files: string[], image = DEFAULT_IMAGE): string[] {
  const list = join(dir, '.lift-files');
  writeFileSync(list, `${files.join('\n')}\n`);
  const script =
    'xargs -P 8 -I{} sh -c \'f="{}"; flags=$(sed -n "s|^// em++ ||p" "$f" | head -1); ' +
    'em++ -c ${flags:--O2 -fPIC -std=c++20 -DNDEBUG} -w "$f" -o "$f.o" 2> "$f.err" || echo "$f"\' < .lift-files';
  const r = spawnSync('docker', ['run', '--rm', '-v', `${dir}:${dir}`, '-w', dir, image, 'sh', '-c', script], {
    encoding: 'utf8',
    maxBuffer: 1 << 26,
  });
  if (r.status !== 0 && r.stdout === '') {
    throw new Error(`docker could not run ${image}: ${r.stderr || r.error?.message}`);
  }
  return r.stdout.split('\n').filter((l) => l !== '');
}

/** The target link's post-link optimiser: emcc -Oz's two wasm-opt runs, the second without its
 *  import/export minification so the function stays findable by name. */
const POST_LINK_FEATURES =
  '--mvp-features --enable-bulk-memory --enable-bulk-memory-opt --enable-call-indirect-overlong ' +
  '--enable-multivalue --enable-mutable-globals --enable-nontrapping-float-to-int --enable-reference-types ' +
  '--enable-sign-ext';
const POST_LINK_OPT =
  `wasm-opt --strip-target-features --post-emscripten -Oz --low-memory-unused --zero-filled-memory ` +
  `--pass-arg=directize-initial-contents-immutable --no-stack-ir "$f.ld.wasm" -o "$f.opt.wasm" ${POST_LINK_FEATURES} && ` +
  `wasm-opt --optimize-level=2 --shrink-level=2 --optimize-stack-ir "$f.opt.wasm" -o "$f.opt.wasm" ${POST_LINK_FEATURES}`;

/** Link each compiled `<file>.o` of `dir` alone with wasm-ld, exporting its symbol, and run the
 *  target link's wasm-opt passes over it, writing `<file>.opt.wasm` (and `<file>.opt.err`); one
 *  container for all. Returns the files that failed. */
export function optimizeLifted(
  dir: string,
  items: readonly { file: string; symbol: string }[],
  image = DEFAULT_IMAGE,
): string[] {
  const list = join(dir, '.opt-files');
  writeFileSync(list, `${items.map((i) => `${i.file}\t${i.symbol}`).join('\n')}\n`);
  const script =
    'export PATH=/emsdk/upstream/bin:$PATH; ' +
    'while IFS="$(printf \'\\t\')" read -r f sym; do ' +
    '{ wasm-ld "$f.o" -o "$f.ld.wasm" --no-entry --export="$sym" --import-memory --allow-undefined && ' +
    `${POST_LINK_OPT}; } 2> "$f.opt.err" || echo "$f"; ` +
    'done < .opt-files';
  const r = spawnSync('docker', ['run', '--rm', '-v', `${dir}:${dir}`, '-w', dir, image, 'sh', '-c', script], {
    encoding: 'utf8',
    maxBuffer: 1 << 26,
  });
  if (r.status !== 0 && r.stdout === '') {
    throw new Error(`docker could not run ${image}: ${r.stderr || r.error?.message}`);
  }
  return r.stdout.split('\n').filter((l) => l !== '');
}
