import type { Lifted } from '@asmlift/wasm/lift';
import { funcAt } from '@asmlift/wasm/parse';
import { liftCompileFlags } from '@asmlift/wasm-benchmark/flags';
import type { BenchResults, DatasetRow } from '@asmlift/wasm-benchmark/schema';
import { useQueryState } from 'nuqs';
import { type ReactNode, useMemo } from 'react';

import { ratioText, referenceText } from '../../lib/bench';
import { localCommands } from '../../lib/commands';
import {
  type Target,
  calleesOf,
  defaultLiftFlags,
  displayName,
  foldText,
  instructions,
  isDefined,
  ledgerFacts,
  lift,
  signatureText,
  stringsOf,
  watText,
} from '../../lib/target';
import { Fact, Panel } from '../../shared/components/Panel';
import { Pill } from '../../shared/components/Pill';
import { DATASET_ROWS, SETUP_BY_KEY } from '../../shared/data/dataset';
import { pickRun, useRuns } from '../../shared/data/runs';
import { useCallers, useTarget } from '../../shared/data/target';
import { EXACT_TINT, outcomeTint } from '../../shared/theme';
import { PANES, fnParser, paneParser, runParser } from '../../shared/utils/url-state';
import { CppEditor, FoldView, WatView } from './CodePanes';
import { FunctionPicker } from './FunctionPicker';

const DEFAULT_ROW = DATASET_ROWS.find((r) => r.id === 'uenux2/2219') ?? DATASET_ROWS[0];

const PANE_LABEL: Record<(typeof PANES)[number], string> = { wat: 'WAT', fold: 'Folded readout' };

export function Playground() {
  const loaded = useTarget();
  const [fn, setFn] = useQueryState('fn', fnParser.withOptions({ history: 'push' }));
  const index = fn ?? DEFAULT_ROW.index;

  if (loaded.state === 'loading') {
    return (
      <div className="grid min-h-[50vh] place-items-center text-sm text-slate-500">
        loading target/vota_web_wasm.wasm (7.7 MB), its glue and the ledger…
      </div>
    );
  }
  if (loaded.state === 'error') {
    return <div className="rounded-lg border border-rose-900 bg-rose-950/40 p-4 font-mono text-sm text-rose-300">{loaded.error}</div>;
  }
  const t = loaded.value;
  return (
    <div className="flex flex-col gap-4">
      <FunctionPicker t={t} index={index} onPick={(i) => void setFn(i)} />
      {isDefined(t, index) ? (
        <FunctionView t={t} index={index} onPick={(i) => void setFn(i)} />
      ) : (
        <div className="rounded-lg border border-amber-900/60 bg-amber-950/30 p-4 text-sm text-amber-300">
          {index} is not a defined function: indices 0–{t.m.importedFuncCount - 1} are imports and have no body, and the
          module ends at {t.m.importedFuncCount + t.m.funcs.length - 1}.
        </div>
      )}
    </div>
  );
}

function FunctionView({ t, index, onPick }: { t: Target; index: number; onPick: (i: number) => void }) {
  const [pane, setPane] = useQueryState('pane', paneParser);
  const [runLabel, setRunLabel] = useQueryState('run', runParser);
  const runs = useRuns();
  const callers = useCallers(t);
  const fn = funcAt(t.m, index)!;
  const row = DATASET_ROWS.find((r) => r.index === index) ?? null;
  const flags = row === null ? defaultLiftFlags(t, index) : liftCompileFlags(row.flags);

  const view = useMemo(() => {
    const instrs = instructions(t, index);
    let lifted: Lifted | { error: string };
    try {
      lifted = lift(t, index, flags);
    } catch (e) {
      lifted = { error: e instanceof Error ? e.message : String(e) };
    }
    return {
      instrs,
      wat: watText(t, index, instrs),
      fold: foldText(t, index, instrs),
      lifted,
      callees: calleesOf(t, instrs),
      strings: stringsOf(t, instrs),
    };
  }, [t, index, flags]);

  const facts = ledgerFacts(t, index);
  const name = displayName(t, index);
  const run = runs === null ? null : pickRun(runs, runLabel);
  const symbol = 'error' in view.lifted ? name : view.lifted.symbol;

  return (
    <>
      <div className="flex flex-wrap items-baseline gap-x-3 gap-y-1">
        <h2 className="font-mono text-lg font-semibold text-slate-100">
          <span className="text-slate-500">#{index}</span> {name}
        </h2>
        <span className="text-xs text-slate-400">{fn.size} bytes</span>
        {row && (
          <Pill tint={EXACT_TINT} size="xs">
            benchmark row {row.id}
          </Pill>
        )}
        {facts.component !== '-' && <Pill size="xs">component {facts.component}</Pill>}
        {facts.owner !== '-' && <Pill size="xs">owner {facts.owner}</Pill>}
      </div>

      <main className="grid gap-4 lg:grid-cols-2">
        <section className="flex h-[600px] flex-col gap-1.5">
          <div className="flex items-center gap-1">
            {PANES.map((p) => (
              <button
                key={p}
                type="button"
                onClick={() => void setPane(p)}
                className={`rounded-md px-2.5 py-1 text-xs font-medium ${pane === p ? 'bg-slate-700 text-white' : 'text-slate-400 hover:bg-slate-800'}`}
              >
                {PANE_LABEL[p]}
              </button>
            ))}
            <span className="ml-auto text-[11px] text-slate-500">
              {pane === 'wat'
                ? `${view.instrs.length} instructions; the gutter is the offset from the body start`
                : 'the operand stack replayed into expressions: a reading aid, not a decompiler'}
            </span>
          </div>
          <div className="min-h-0 flex-1 overflow-hidden rounded-lg border border-slate-800">
            {pane === 'wat' ? <WatView value={view.wat} /> : <FoldView value={view.fold} />}
          </div>
        </section>
        <section className="flex h-[600px] flex-col gap-1.5">
          <div className="flex items-baseline justify-between gap-2">
            <h3 className="shrink-0 text-sm font-semibold text-slate-300">Lifted C++</h3>
            <span className="truncate font-mono text-[11px] text-slate-500">em++ {flags}</span>
          </div>
          {'error' in view.lifted ? (
            <div className="rounded-lg border border-rose-900 bg-rose-950/40 p-4 font-mono text-sm text-rose-300">
              the lifter threw: {view.lifted.error}
            </div>
          ) : (
            <CppEditor key={`${index} ${flags}`} initial={view.lifted.source} fileName={`${index}.cpp`} />
          )}
        </section>
      </main>

      <div className="grid gap-4 lg:grid-cols-3">
        <Panel title="Facts" subtitle="from the module, its glue and the ledger">
          <dl className="grid grid-cols-[auto_1fr] gap-x-4 gap-y-1.5">
            <Fact term="index">{index}</Fact>
            <Fact term="name">
              <span className="font-mono text-xs">{name}</span>
            </Fact>
            {symbol !== name && (
              <Fact term="symbol">
                <span className="font-mono text-xs">{symbol}</span>
              </Fact>
            )}
            {t.exports.has(index) && <Fact term="exported as">{t.exports.get(index)!.join(', ')}</Fact>}
            <Fact term="size">
              {fn.size} bytes, {view.instrs.length} instructions, body at 0x{fn.bodyOffset.toString(16)}
            </Fact>
            <Fact term="signature">
              <span className="font-mono text-xs">{signatureText(fn)}</span>
            </Fact>
            <Fact term="locals">{localsSummary(fn.locals)}</Fact>
            <Fact term="component">
              {facts.component}
              {facts.evidence !== '-' && <span className="text-slate-500"> (by {facts.evidence})</span>}
            </Fact>
            <Fact term="owner">{facts.owner}</Fact>
            <Fact term="census pair">
              {facts.best === null ? (
                'none'
              ) : (
                <>
                  {facts.best.kind} with {facts.best.label}:{facts.best.index}
                  {facts.best.score > 0 && `, ${facts.best.score} ${facts.best.score === 1 ? 'row differs' : 'rows differ'}`}
                </>
              )}
              {facts.shapeName && (
                <div className="font-mono text-xs text-slate-500">same shape as {facts.shapeName}</div>
              )}
            </Fact>
            <Fact term="lift flags">
              <span className="font-mono text-xs">{flags}</span>
              <div className="text-xs text-slate-500">
                {row ? `the ${row.setup} setup's, as the benchmark compiles it` : `component ${facts.component}'s, as pnpm wasm lift writes them`}
              </div>
            </Fact>
          </dl>
        </Panel>
        <BenchmarkRow row={row} runs={runs} run={run} onRun={(l) => void setRunLabel(l)} index={index} />
        <Commands
          index={index}
          symbol={symbol}
          name={name}
          component={facts.component}
          flags={flags}
          setup={row?.setup ?? null}
        />
      </div>

      <div className="grid gap-4 lg:grid-cols-3">
        <Panel
          title={callers === null ? 'Callers' : `Callers (${callers.get(index)?.length ?? 0})`}
          subtitle="functions with a direct call to it"
        >
          {callers === null ? (
            <p className="text-xs text-slate-500">scanning every function body…</p>
          ) : (
            <FunctionList t={t} items={(callers.get(index) ?? []).map((i) => ({ index: i }))} onPick={onPick} empty="no direct caller: reached through the table, an export or not at all" />
          )}
        </Panel>
        <Panel title={`Callees (${view.callees.length})`} subtitle="direct calls, with the number of call sites">
          <FunctionList
            t={t}
            items={view.callees.map((c) => ({ index: c.index, note: `×${c.sites}`, import: c.import }))}
            onPick={onPick}
            empty="a leaf: no direct call"
          />
        </Panel>
        <Panel title={`Strings (${view.strings.length})`} subtitle="ledger strings an i32.const ≥ 1024 points into; a flag or size constant can land in one by chance">
          {view.strings.length === 0 ? (
            <p className="text-xs text-slate-500">no string reference</p>
          ) : (
            <ul className="scroll-slim max-h-72 space-y-1 overflow-auto pr-1">
              {view.strings.map((s) => (
                <li key={s.addr} className="font-mono text-xs">
                  <span className="text-slate-500">{s.offset.toString(16).padStart(6, '0')} </span>
                  <span className="text-slate-400">
                    {s.addr}
                    {s.into > 0 && ` (+${s.into})`}{' '}
                  </span>
                  <span className="break-all text-amber-200/90">{JSON.stringify(s.s.text)}</span>
                </li>
              ))}
            </ul>
          )}
        </Panel>
      </div>
    </>
  );
}

function localsSummary(locals: readonly string[]): string {
  if (locals.length === 0) {
    return 'none';
  }
  const counts = new Map<string, number>();
  for (const l of locals) {
    counts.set(l, (counts.get(l) ?? 0) + 1);
  }
  return `${locals.length}: ${[...counts].map(([type, n]) => `${n} × ${type}`).join(', ')}`;
}

function FunctionList({
  t,
  items,
  onPick,
  empty,
}: {
  t: Target;
  items: { index: number; note?: string; import?: boolean }[];
  onPick: (i: number) => void;
  empty: string;
}) {
  if (items.length === 0) {
    return <p className="text-xs text-slate-500">{empty}</p>;
  }
  return (
    <ul className="scroll-slim max-h-72 space-y-0.5 overflow-auto pr-1">
      {items.map((it) => (
        <li key={it.index} className="flex items-baseline gap-2 font-mono text-xs">
          <span className="w-12 shrink-0 text-right text-slate-500">{it.index}</span>
          {it.import ? (
            <span className="truncate text-slate-400" title={displayName(t, it.index)}>
              import {displayName(t, it.index)}
            </span>
          ) : (
            <button
              type="button"
              onClick={() => onPick(it.index)}
              className="truncate text-left text-teal-300 hover:underline"
              title={displayName(t, it.index)}
            >
              {displayName(t, it.index)}
            </button>
          )}
          {it.note && <span className="ml-auto shrink-0 text-slate-500">{it.note}</span>}
        </li>
      ))}
    </ul>
  );
}

function BenchmarkRow({
  row,
  runs,
  run,
  onRun,
  index,
}: {
  row: DatasetRow | null;
  runs: BenchResults[] | null;
  run: BenchResults | null;
  onRun: (label: string) => void;
  index: number;
}) {
  if (row === null) {
    return (
      <Panel title="Benchmark" subtitle="not one of the benchmark's rows">
        <p className="text-sm text-slate-400">
          The benchmark has {DATASET_ROWS.length} rows; <code className="font-mono text-xs">pnpm bench add {index}</code> adds this
          function when a compile setup covers it, and says why not otherwise.
        </p>
      </Panel>
    );
  }
  const setup = SETUP_BY_KEY.get(row.setup);
  const result = run?.rows.find((r) => r.id === row.id) ?? null;
  const reference = referenceText(row);
  return (
    <Panel
      title={`Benchmark row ${row.id}`}
      subtitle={setup?.title}
      aside={
        runs !== null && runs.length > 1 ? (
          <select
            value={run?.label ?? ''}
            onChange={(e) => onRun(e.target.value)}
            className="rounded-md border border-slate-700 bg-slate-900 px-1.5 py-0.5 text-xs"
            aria-label="benchmark run"
          >
            {runs.map((r) => (
              <option key={r.label} value={r.label}>
                {r.label}
              </option>
            ))}
          </select>
        ) : undefined
      }
    >
      <dl className="grid grid-cols-[auto_1fr] gap-x-4 gap-y-1.5">
        {result === null ? (
          <Fact term="result">{run === null ? 'loading runs…' : `not in run ${run.label}`}</Fact>
        ) : (
          <>
            <Fact term={`run ${run!.label}`}>
              <Pill tint={outcomeTint(result.outcome)} size="xs">
                {result.outcome}
              </Pill>
              {result.exact && <span className="ml-2 text-xs text-slate-400">-Oz score 0</span>}
            </Fact>
            <Fact term="raw score">
              {result.raw === null ? '–' : `${result.raw.score} of ${result.targetRows} rows (${ratioText(result.raw.ratio)})`}
            </Fact>
            <Fact term="-Oz score">
              {result.opt === null ? '–' : `${result.opt.score} of ${result.targetRows} rows (${ratioText(result.opt.ratio)})`}
            </Fact>
            {result.failure && (
              <Fact term="failure">
                {result.failure.class}
                <div className="font-mono text-xs text-slate-400">{result.failure.line}</div>
              </Fact>
            )}
            {result.standIns.length > 0 && (
              <Fact term="stand-ins">
                <div className="flex flex-wrap gap-1">
                  {result.standIns.map((s) => (
                    <Pill key={s} size="xs">
                      {s}
                    </Pill>
                  ))}
                </div>
              </Fact>
            )}
          </>
        )}
        <Fact term="setup">
          {row.setup} ({row.lang}){row.instantiatedBy && `, instantiated by ${row.instantiatedBy}`}
        </Fact>
        <Fact term="build flags">
          <span className="font-mono text-xs">{row.flags}</span>
        </Fact>
        <Fact term="features">
          <div className="flex flex-wrap gap-1">
            {row.features.map((f) => (
              <Pill key={f} size="xs" mono>
                {f}
              </Pill>
            ))}
          </div>
        </Fact>
        <Fact term="evidence">
          {row.evidence === 'symbol' ? (
            <>
              symbol <span className="font-mono text-xs">{row.symbol}</span> defined by{' '}
              <span className="font-mono text-xs">{row.object}</span>
            </>
          ) : (
            "the ledger's component attribution"
          )}
        </Fact>
        <Fact term="reference">{reference === null ? 'none yet' : <span className="font-mono text-xs break-all">{reference}</span>}</Fact>
      </dl>
    </Panel>
  );
}

function Commands(props: {
  index: number;
  symbol: string;
  name: string;
  component: string;
  flags: string;
  setup: string | null;
}) {
  const { index } = props;
  const blocks = localCommands(props);
  return (
    <Panel
      title="Compile and score it locally"
      subtitle={`em++ does not run in a browser. Download the C++ as ${index}.cpp into /tmp/lift/, then:`}
    >
      <div className="flex flex-col gap-3">
        {blocks.map((b) => (
          <div key={b.title}>
            <p className="mb-1 text-xs text-slate-400">
              {b.title} <span className="text-slate-600">· in {b.cwd === '.' ? 'the urna checkout' : `urna/${b.cwd}`}</span>
            </p>
            <Shell lines={b.lines} />
          </div>
        ))}
      </div>
    </Panel>
  );
}

function Shell({ lines }: { lines: string[] }): ReactNode {
  return (
    <pre className="whitespace-pre-wrap break-all rounded-md border border-slate-800 bg-slate-900/80 px-2.5 py-1.5 font-mono text-[11px] leading-relaxed text-slate-200">
      {lines.join('\n')}
    </pre>
  );
}
