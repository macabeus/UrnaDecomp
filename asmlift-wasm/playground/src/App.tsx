// The shell: the family header, the two tabs, and the pages under them. The Playground stays mounted
// so its editor keeps what was typed; the Benchmark (ECharts) loads on its first visit. State lives in
// the URL fragment (shared/utils/hash-adapter.ts), so any view of any function is a link.
import { useQueryState, useQueryStates } from 'nuqs';
import { Suspense, lazy, useCallback, useEffect, useState } from 'react';

import logoUrl from './assets/logo.png';
import { Playground } from './pages/playground/Playground';
import { type View, fnParser, viewParser } from './shared/utils/url-state';

const Benchmark = lazy(() => import('./pages/benchmark/Benchmark'));

const NAV: { id: View; label: string }[] = [
  { id: 'playground', label: 'Playground' },
  { id: 'benchmark', label: 'Benchmark' },
];

const SUBTITLE: Record<View, string> = {
  playground: 'any function of vota_web_wasm.wasm: WAT, folded readout and lifted C++, computed in your browser',
  benchmark: 'the lifter over every compile setup of vota_web_wasm.wasm',
};

export function App() {
  const [view, setView] = useQueryState('view', viewParser.withOptions({ history: 'push' }));
  const [benchVisited, setBenchVisited] = useState(view === 'benchmark');
  const [, setHandoff] = useQueryStates({ view: viewParser, fn: fnParser });

  useEffect(() => {
    if (view === 'benchmark') {
      setBenchVisited(true);
    }
  }, [view]);

  const navigate = useCallback(
    (v: View) => {
      if (v === 'benchmark') {
        setBenchVisited(true);
      }
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
                  alt="asmlift logo"
                  className="relative h-14 w-14 object-contain drop-shadow-lg sm:h-16 sm:w-16"
                />
              </div>
              <div className="min-w-0">
                <h1 className="text-2xl font-bold tracking-tight sm:text-3xl">
                  <span className="bg-gradient-to-r from-teal-300 via-teal-200 to-amber-300 bg-clip-text text-transparent">
                    asmlift
                  </span>{' '}
                  <span className="text-slate-400">wasm</span>
                </h1>
                <p className="mt-0.5 truncate text-sm font-medium text-slate-400">{SUBTITLE[view]}</p>
              </div>
            </div>
            <span className="hidden font-mono text-xs text-slate-500 md:block">urna · target/vota_web_wasm.wasm</span>
          </div>
          <div className="h-1 bg-gradient-to-r from-teal-500 via-teal-400 to-amber-400" />
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

        <div className={view === 'playground' ? undefined : 'hidden'}>
          <Playground />
        </div>
        {benchVisited && (
          <div className={view === 'benchmark' ? undefined : 'hidden'}>
            <Suspense fallback={<Loading />}>
              <Benchmark onOpenInPlayground={openInPlayground} />
            </Suspense>
          </div>
        )}
      </div>
    </div>
  );
}

function Loading() {
  return <div className="grid min-h-[60vh] place-items-center text-sm text-slate-500">loading the benchmark…</div>;
}
