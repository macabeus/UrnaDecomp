// The shell: the family header, the four tabs, and the pages under them. Each page is its own chunk, mounts on
// its first visit and then stays mounted, so the Playground's editor keeps what was typed. State lives in the URL
// fragment (shared/utils/hash-adapter.ts), so any view of any function is a link.
import { useQueryState, useQueryStates } from 'nuqs';
import { Suspense, lazy, useCallback, useEffect, useState } from 'react';

import logoUrl from './assets/urnadecomp.png';
import { PageIntro } from './shared/components/PageIntro';
import { SETUPS } from './shared/data/dataset';
import { type View, fnParser, viewParser } from './shared/utils/url-state';

const Benchmark = lazy(() => import('./pages/benchmark/Benchmark'));
const Playground = lazy(() => import('./pages/playground/Playground').then((m) => ({ default: m.Playground })));
const Progress = lazy(() => import('./pages/progress/Progress'));
const UrnaEmu = lazy(() => import('./pages/urnaemu/UrnaEmu').then((m) => ({ default: m.UrnaEmu })));

const NAV: { id: View; label: string }[] = [
  { id: 'progress', label: 'Progress' },
  { id: 'playground', label: 'Playground' },
  { id: 'benchmark', label: 'Benchmark' },
  { id: 'urnaemu', label: 'UrnaEmu' },
];

const SUBTITLE: Record<View, string> = {
  progress: 'the decompilation of vota_web_wasm.wasm, by origin, component and function',
  playground: 'any function of vota_web_wasm.wasm: WAT, folded readout and lifted C++, computed in your browser',
  benchmark: 'the lifter over every compile setup of vota_web_wasm.wasm',
  urnaemu: 'the emulator that runs vota_web_wasm.wasm in your browser',
};

export function App() {
  const [view, setView] = useQueryState('view', viewParser.withOptions({ history: 'push' }));
  const [visited, setVisited] = useState<ReadonlySet<View>>(() => new Set([view]));
  const [, setHandoff] = useQueryStates({ view: viewParser, fn: fnParser });

  useEffect(() => {
    setVisited((s) => (s.has(view) ? s : new Set([...s, view])));
  }, [view]);

  const navigate = useCallback(
    (v: View) => {
      setVisited((s) => (s.has(v) ? s : new Set([...s, v])));
      void setView(v);
    },
    [setView],
  );

  const openInPlayground = useCallback(
    (index: number) => {
      void setHandoff({ view: 'playground', fn: index }, { history: 'push', scroll: true });
    },
    [setHandoff],
  );

  return (
    <div className="min-h-screen">
      <div className="mx-auto flex max-w-[1500px] flex-col px-4 py-6">
        <header className="mb-6 overflow-hidden rounded-2xl bg-gradient-to-r from-slate-900 via-slate-800 to-slate-900 shadow-xl">
          <div className="flex items-center justify-between gap-6 px-6 py-5 sm:px-8">
            <div className="flex min-w-0 items-center gap-4 sm:gap-5">
              <div className="relative flex-shrink-0">
                <div className="absolute inset-0 rounded-full bg-amber-400/25 blur-xl" />
                <img
                  src={logoUrl}
                  alt="UrnaDecomp logo"
                  className="relative h-14 w-14 object-contain drop-shadow-lg sm:h-16 sm:w-16"
                />
              </div>
              <div className="min-w-0">
                <h1 className="text-2xl font-bold tracking-tight sm:text-3xl">
                  <span className="bg-gradient-to-r from-teal-300 via-teal-200 to-amber-300 bg-clip-text text-transparent">
                    Urna
                  </span>
                  <span className="text-slate-400">Decomp</span>
                </h1>
                <p className="mt-0.5 truncate text-sm font-medium text-slate-400">{SUBTITLE[view]}</p>
              </div>
            </div>
            <span className="hidden text-right font-mono text-xs leading-relaxed text-slate-500 md:block">
              target/vota_web_wasm.wasm
              <br />
              built with asmlift-wasm
            </span>
          </div>
          <div className="h-1 bg-gradient-to-r from-teal-600 via-amber-400 to-blue-500" />
        </header>

        <nav className="mb-6 flex items-center gap-2">
          {NAV.map((n) => (
            <button
              key={n.id}
              type="button"
              onClick={() => navigate(n.id)}
              className={`rounded-xl px-4 py-2 text-sm font-medium transition-all ${
                view === n.id
                  ? 'border border-teal-500/30 bg-teal-500/20 text-teal-300 shadow-lg shadow-teal-500/10'
                  : 'border border-transparent text-slate-400 hover:bg-slate-800/50 hover:text-slate-200'
              }`}
            >
              {n.label}
            </button>
          ))}
        </nav>

        {visited.has('progress') && (
          <div className={view === 'progress' ? undefined : 'hidden'}>
            <Suspense fallback={<Loading what="the meter" />}>
              <Progress onOpenInPlayground={openInPlayground} />
            </Suspense>
          </div>
        )}
        {visited.has('playground') && (
          <div className={view === 'playground' ? undefined : 'hidden'}>
            <PlaygroundIntro />
            <Suspense fallback={<Loading what="the playground" />}>
              <Playground />
            </Suspense>
          </div>
        )}
        {visited.has('benchmark') && (
          <div className={view === 'benchmark' ? undefined : 'hidden'}>
            <BenchmarkIntro />
            <Suspense fallback={<Loading what="the benchmark" />}>
              <Benchmark onOpenInPlayground={openInPlayground} />
            </Suspense>
          </div>
        )}
        {visited.has('urnaemu') && (
          <div className={view === 'urnaemu' ? undefined : 'hidden'}>
            <Suspense fallback={<Loading what="the page" />}>
              <UrnaEmu />
            </Suspense>
          </div>
        )}
      </div>
    </div>
  );
}

function PlaygroundIntro() {
  return (
    <PageIntro
      title="What the Playground is"
      lead={
        <>
          Read any function of <span className="font-mono">vota_web_wasm.wasm</span> the way a decompiler does, computed
          in your browser from the real module. Each view is a step from the target's bytes towards C++. The lifted C++ is a
          starting point to rewrite, never the source: a function counts as matched (the Progress page) only once real C++ in
          its source file builds to exactly the target's bytes.
        </>
      }
      parts={[
        ['WAT', "the function's WebAssembly body as text, one instruction per line, with every callee named"],
        ['Folded readout', 'the operand stack replayed into expressions: a reading aid that shows what each block computes'],
        ['Lifted C++', 'asmlift translates the body into compilable C++, with the names the ledger knows; edit, copy or download it'],
        ['Facts and commands', 'where the function sits (component, owner, size, callers, callees, strings) and the commands to compile and score your rewrite locally'],
      ]}
    />
  );
}

function BenchmarkIntro() {
  return (
    <PageIntro
      title="What the Benchmark is"
      lead={
        <>
          How good asmlift's lifter is, measured on a frozen set of target functions, about a dozen from each of the{' '}
          {SETUPS.length} compile setups the module was built with: TSE's code, its libraries and emscripten's system
          libraries. It measures the tool, not the decompilation: an exact row is not a matched function.
        </>
      }
      parts={[
        ['Row', "one target function, lifted to C++ and compiled with em++ at its own compile setup's flags"],
        ['Raw and -Oz score', "the object's instructions against the target's, as the share that differ, before and after the target link's wasm-opt passes; lower is closer"],
        ['Exact', 'the -Oz score is 0: structurally equal after normalisation, with the function linked alone; never byte-identical'],
        ['Runs', 'each results file under benchmark/results/ is a run; compare two to see what a lifter change moved'],
      ]}
    />
  );
}

function Loading({ what }: { what: string }) {
  return <div className="grid min-h-[60vh] place-items-center text-sm text-slate-500">loading {what}…</div>;
}
