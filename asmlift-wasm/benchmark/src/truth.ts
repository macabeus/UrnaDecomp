// The build's own account of what it compiles and how: object lists from src/Makefile's variables,
// each app object's compile command from a `make -n -B` dry run, the flags a library build recorded
// beside its objects, and emscripten's system-library flags computed by emscripten itself in the
// pinned image. The setup registry is checked against these; the dataset's provenance reads them.
import { spawnSync } from 'node:child_process';
import { existsSync, readFileSync } from 'node:fs';
import { isAbsolute, join, relative } from 'node:path';

import { codegenFlags, splitFlags } from './flags';
import { BENCH_DIR, URNA_ROOT } from './paths';
import type { Lang, Setup } from './schema';

function run(cmd: string, args: string[], opts: { cwd?: string } = {}): string {
  const r = spawnSync(cmd, args, { encoding: 'utf8', maxBuffer: 1 << 28, ...opts });
  if (r.status !== 0) {
    throw new Error(`${cmd} ${args.slice(0, 6).join(' ')}… failed (${r.status}): ${(r.stderr || r.error?.message || '').slice(0, 2000)}`);
  }
  return r.stdout;
}

/** `path` relative to the urna root. */
export function repoPath(path: string): string {
  return isAbsolute(path) ? relative(URNA_ROOT, path) : path.replace(/^\.\//, '');
}

/** The words of each `make -C src print-<var>`, as repo-relative paths. */
export function makePrint(vars: readonly string[]): Map<string, string[]> {
  const out = run('make', ['-s', '-C', join(URNA_ROOT, 'src'), ...vars.map((v) => `print-${v}`)]);
  const lines = out.split('\n');
  return new Map(vars.map((v, k) => [v, (lines[k] ?? '').split(/\s+/).filter((w) => w !== '').map(repoPath)]));
}

export interface ObjectCommand {
  object: string;
  compiler: 'emcc' | 'em++';
  lang: Lang;
  /** code-generation flags, in command order */
  flags: string[];
  /** repo-relative source path */
  source: string;
}

/** What `make -n -B` would run to build each object: the compiler, its code-generation flags and the
 *  source. Objects without a compile command (an archive, a library build's output) are absent. */
export function makeCommands(objects: readonly string[]): Map<string, ObjectCommand> {
  const wanted = new Set(objects.map(repoPath));
  const out = run('make', ['-n', '-B', '-C', join(URNA_ROOT, 'src'), ...objects.map((o) => join(URNA_ROOT, repoPath(o)))]);
  const found = new Map<string, ObjectCommand>();
  for (const line of out.split('\n')) {
    const at = line.lastIndexOf('docker-build.sh ');
    if (at < 0) {
      continue;
    }
    const tokens = splitFlags(line.slice(at + 'docker-build.sh '.length).split(' && ')[0]);
    const compiler = tokens[0];
    if (compiler !== 'emcc' && compiler !== 'em++') {
      continue;
    }
    const o = tokens.indexOf('-o');
    const c = tokens.indexOf('-c');
    if (o < 0 || c < 0) {
      continue;
    }
    const object = repoPath(tokens[o + 1]);
    if (!wanted.has(object)) {
      continue;
    }
    const source = tokens[c + 1].replace(/^\/work\//, '');
    found.set(object, {
      object,
      compiler,
      lang: compiler === 'emcc' || /\.c$/.test(source) ? 'c' : 'c++',
      flags: codegenFlags(tokens.slice(1)),
      source,
    });
  }
  return found;
}

/** The flags a library build recorded, or null when that build has not run in this checkout. */
export function artefactFlags(truth: Extract<Setup['truth'], { kind: 'artefact' }>): string[] | null {
  const path = join(URNA_ROOT, truth.file);
  if (!existsSync(path)) {
    return null;
  }
  const text = readFileSync(path, 'utf8');
  switch (truth.format) {
    case 'nul':
      return codegenFlags(text.split('\0').filter((t) => t !== ''));
    case 'lines':
      return codegenFlags(text.split('\n').filter((t) => t !== ''));
    case 'make-var': {
      const vars = new Map(
        text
          .split('\n')
          .map((l) => /^([A-Z_]+)=(.*)$/.exec(l))
          .filter((m) => m !== null)
          .map((m) => [m[1], m[2]]),
      );
      return codegenFlags((truth.fileVars ?? []).flatMap((v) => splitFlags(vars.get(v) ?? '')));
    }
  }
}

export function hasImage(image: string): boolean {
  return spawnSync('docker', ['image', 'inspect', image], { encoding: 'utf8' }).status === 0;
}

/** Each named system library's flags as emscripten computes them for the target's link (legacy JS
 *  exceptions, no assertions): `tools/system-lib-flags.py` run in the image. */
export function emscriptenFlags(image: string, libraries: readonly string[]): Map<string, string[]> {
  const tools = join(BENCH_DIR, 'tools');
  const out = run('docker', [
    'run',
    '--rm',
    '-v',
    `${tools}:/bench-tools:ro`,
    image,
    'python3',
    '/bench-tools/system-lib-flags.py',
    ...libraries,
  ]);
  const parsed = JSON.parse(out) as Record<string, string[]>;
  return new Map(Object.entries(parsed).map(([k, v]) => [k, codegenFlags(v)]));
}

/** Run `script` with `sh -c` in the image, the urna root mounted at /work. */
export function inImage(image: string, script: string): string {
  return run('docker', ['run', '--rm', '-v', `${URNA_ROOT}:/work`, '-w', '/work', image, 'sh', '-c', script]);
}
