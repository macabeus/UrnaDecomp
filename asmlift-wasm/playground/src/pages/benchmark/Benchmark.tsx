import type { BenchResults, DatasetRow, RowResult, Totals } from '@asmlift/wasm-benchmark/schema';
import { useQueryState } from 'nuqs';
import { useMemo } from 'react';

import { type RunDiff, diffRuns, failures, ratioText, referenceText, setupSummaries } from '../../lib/bench';
import { Fact, Panel, Stat } from '../../shared/components/Panel';
import { Pill } from '../../shared/components/Pill';
import { DATASET_ROWS, SETUPS, SETUP_BY_KEY } from '../../shared/data/dataset';
import { pickRun, useRuns } from '../../shared/data/runs';
import { outcomeTint } from '../../shared/theme';
import { rowParser, runParser, setupParser, vsParser } from '../../shared/utils/url-state';
import { OutcomeChart } from './OutcomeChart';

const ROW_BY_ID = new Map(DATASET_ROWS.map((r) => [r.id, r]));

export default function Benchmark({ onOpenInPlayground }: { onOpenInPlayground: (index: number) => void }) {
  const runs = useRuns();
  const [runLabel, setRunLabel] = useQueryState('run', runParser);
  const [vsLabel, setVsLabel] = useQueryState('vs', vsParser);
  const [setupKey, setSetupKey] = useQueryState('setup', setupParser);
  const [rowId, setRowId] = useQueryState('row', rowParser);

  if (runs === null) {
    return <div className="grid min-h-[40vh] place-items-center text-sm text-slate-500">loading the results…</div>;
  }
  const run = pickRun(runs, runLabel);
  if (run === null) {
    return <p className="text-sm text-slate-400">No results file under benchmark/results/: run `pnpm bench run`.</p>;
  }
  const vs = vsLabel === null ? null : (runs.find((r) => r.label === vsLabel && r.label !== run.label) ?? null);
  return (
    <BenchmarkView
      runs={runs}
      run={run}
      vs={vs}
      setupKey={setupKey}
      rowId={rowId}
      onRun={(l) => void setRunLabel(l)}
      onVs={(l) => void setVsLabel(l)}
      onSetup={(k) => void setSetupKey(k)}
      onRow={(id) => void setRowId(id)}
      onOpenInPlayground={onOpenInPlayground}
    />
  );
}

interface ViewProps {
  runs: BenchResults[];
  run: BenchResults;
  /** the run `run` is compared against: before → after is vs → run */
  vs: BenchResults | null;
  setupKey: string | null;
  rowId: string | null;
  onRun: (label: string) => void;
  onVs: (label: string | null) => void;
  onSetup: (key: string | null) => void;
  onRow: (id: string | null) => void;
  onOpenInPlayground: (index: number) => void;
}

function BenchmarkView(p: ViewProps) {
  const { run, vs } = p;
  const setups = useMemo(() => setupSummaries(run, SETUPS), [run]);
  const diff = useMemo(() => (vs === null ? null : diffRuns(vs, run)), [vs, run]);
  const o = run.totals.overall;
  const before = vs?.totals.overall;
  return (
    <div className="flex flex-col gap-4">
      <RunPicker {...p} />

      <div className="grid grid-cols-2 gap-3 md:grid-cols-3 xl:grid-cols-6">
        <Stat label="rows" value={o.rows} hint={`${SETUPS.length} compile setups`} />
        <Stat label="compiled" value={o.compiled} hint={delta(before?.compiled, o.compiled) ?? 'em++ -c at the row flags'} />
        <Stat label="exact at -Oz" value={o.exact} hint={delta(before?.exact, o.exact) ?? `${pct(o.exact, o.rows)} of the rows`} />
        <Stat label="exact raw" value={o.exactRaw} hint={delta(before?.exactRaw, o.exactRaw) ?? 'the object, before wasm-opt'} />
        <Stat
          label="median -Oz ratio"
          value={ratioText(o.medianOptRatio)}
          hint={ratioDelta(before?.medianOptRatio, o.medianOptRatio) ?? 'differing rows over target rows'}
        />
        <Stat label="mean -Oz ratio" value={ratioText(o.meanOptRatio)} hint={ratioDelta(before?.meanOptRatio, o.meanOptRatio) ?? 'lower is closer'} />
      </div>

      <div className="grid gap-4 xl:grid-cols-[minmax(0,5fr)_minmax(0,7fr)]">
        <Panel title="Rows per setup by outcome" subtitle="click a bar to list that setup's rows">
          <OutcomeChart run={run} setups={setups} onSetup={(k) => p.onSetup(k)} />
        </Panel>
        <Panel title="Per setup" subtitle={vs ? `${vs.label} → ${run.label}` : 'click a setup to list its rows'}>
          <SetupTable setups={setups} vs={vs} selected={p.setupKey} onSetup={p.onSetup} />
        </Panel>
      </div>

      {diff !== null && vs !== null && <DiffPanel diff={diff} before={vs} after={run} onRow={p.onRow} />}

      <RowsPanel {...p} />
    </div>
  );
}

function RunPicker({ runs, run, vs, onRun, onVs }: ViewProps) {
  return (
    <div className="flex flex-wrap items-end gap-3 text-sm">
      <label className="flex flex-col gap-1">
        <span className="text-xs uppercase tracking-wide text-slate-500">Run ({runs.length} under benchmark/results/)</span>
        <select
          value={run.label}
          onChange={(e) => onRun(e.target.value)}
          className="rounded-md border border-slate-700 bg-slate-900 px-2 py-1.5 text-xs"
          aria-label="run"
        >
          {runs.map((r) => (
            <option key={r.label} value={r.label}>
              {r.label} · {r.createdAt.slice(0, 16).replace('T', ' ')}
            </option>
          ))}
        </select>
      </label>
      {runs.length > 1 && (
        <label className="flex flex-col gap-1">
          <span className="text-xs uppercase tracking-wide text-slate-500">Compare with</span>
          <select
            value={vs?.label ?? ''}
            onChange={(e) => onVs(e.target.value === '' ? null : e.target.value)}
            className="rounded-md border border-slate-700 bg-slate-900 px-2 py-1.5 text-xs"
            aria-label="compare with"
          >
            <option value="">nothing</option>
            {runs
              .filter((r) => r.label !== run.label)
              .map((r) => (
                <option key={r.label} value={r.label}>
                  {r.label}
                </option>
              ))}
          </select>
        </label>
      )}
      <p className="ml-auto max-w-xl text-right text-xs leading-relaxed text-slate-500">
        commit <span className="font-mono text-slate-400">{run.commit ?? 'unknown'}</span> · image{' '}
        <span className="font-mono text-slate-400">{run.image}</span> · {run.dataset.rows} rows ·{' '}
        {(run.durationMs.total / 1000).toFixed(1)} s. Exact here is a structure match of the function linked alone, never a
        byte match.
      </p>
    </div>
  );
}

function SetupTable({
  setups,
  vs,
  selected,
  onSetup,
}: {
  setups: ReturnType<typeof setupSummaries>;
  vs: BenchResults | null;
  selected: string | null;
  onSetup: (key: string | null) => void;
}) {
  const cell = 'px-2 py-1 text-right font-mono';
  const exactCell = (t: Totals, b: Totals | undefined) => (
    <>
      {t.exact}
      {b !== undefined && b.exact !== t.exact && <DeltaMark d={t.exact - b.exact} />}
    </>
  );
  return (
    <div className="scroll-slim overflow-x-auto">
      <table className="w-full text-xs">
        <thead className="text-slate-400">
          <tr className="border-b border-slate-700">
            <th className="px-2 py-1 text-left font-medium">setup</th>
            <th className="px-2 py-1 text-right font-medium">rows</th>
            <th className="px-2 py-1 text-right font-medium">compiled</th>
            <th className="px-2 py-1 text-right font-medium">exact raw</th>
            <th className="px-2 py-1 text-right font-medium">exact -Oz</th>
            <th className="px-2 py-1 text-right font-medium">failed</th>
            <th className="px-2 py-1 text-right font-medium">median raw</th>
            <th className="px-2 py-1 text-right font-medium">median -Oz</th>
            <th className="px-2 py-1 text-right font-medium">mean -Oz</th>
          </tr>
        </thead>
        <tbody>
          {setups.map((s) => {
            const b = vs?.totals.bySetup[s.key];
            const t = s.totals;
            return (
              <tr
                key={s.key}
                onClick={() => onSetup(selected === s.key ? null : s.key)}
                className={`cursor-pointer border-b border-slate-800 ${selected === s.key ? 'bg-teal-500/10' : 'hover:bg-slate-800/60'}`}
                title={SETUP_BY_KEY.get(s.key)?.title}
              >
                <td className="px-2 py-1 font-mono text-slate-200">{s.key}</td>
                <td className={cell}>{t.rows}</td>
                <td className={cell}>{t.compiled}</td>
                <td className={cell}>{t.exactRaw}</td>
                <td className={cell}>{exactCell(t, b)}</td>
                <td className={cell}>{failures(t)}</td>
                <td className={cell}>{ratioText(t.medianRatio)}</td>
                <td className={cell}>{ratioText(t.medianOptRatio)}</td>
                <td className={cell}>
                  {ratioText(t.meanOptRatio)}
                  {b?.meanOptRatio != null && t.meanOptRatio != null && b.meanOptRatio !== t.meanOptRatio && (
                    <span className="ml-1 text-slate-400">({signed(t.meanOptRatio - b.meanOptRatio, 3)})</span>
                  )}
                </td>
              </tr>
            );
          })}
        </tbody>
      </table>
    </div>
  );
}

function DiffPanel({ diff, before, after, onRow }: { diff: RunDiff; before: BenchResults; after: BenchResults; onRow: (id: string) => void }) {
  const cell = (r: RowResult | undefined) =>
    r === undefined ? 'absent' : r.opt === null ? r.outcome : `${r.opt.score} (${ratioText(r.opt.ratio)})`;
  return (
    <Panel
      title={`What moved: ${before.label} → ${after.label}`}
      subtitle={`${diff.better} rows scored better at -Oz, ${diff.worse} worse, ${diff.moved.length - diff.better - diff.worse} changed otherwise or are in one run only${
        diff.sameDataset ? '' : '; the two runs used different datasets'
      }`}
    >
      {diff.moved.length === 0 ? (
        <p className="text-xs text-slate-500">no row changed its outcome or its -Oz score</p>
      ) : (
        <table className="w-full text-xs">
          <thead className="text-slate-400">
            <tr className="border-b border-slate-700">
              <th className="px-2 py-1 text-left font-medium">row</th>
              <th className="px-2 py-1 text-right font-medium">{before.label}</th>
              <th className="px-2 py-1 text-right font-medium">{after.label}</th>
              <th className="px-2 py-1 text-left font-medium" />
            </tr>
          </thead>
          <tbody>
            {diff.moved.map((m) => (
              <tr key={m.id} onClick={() => onRow(m.id)} className="cursor-pointer border-b border-slate-800 hover:bg-slate-800/60">
                <td className="px-2 py-1 font-mono text-slate-200">{m.id}</td>
                <td className="px-2 py-1 text-right font-mono">{cell(m.before)}</td>
                <td className="px-2 py-1 text-right font-mono">{cell(m.after)}</td>
                <td className="px-2 py-1 text-slate-400">{m.direction}</td>
              </tr>
            ))}
          </tbody>
        </table>
      )}
    </Panel>
  );
}

function RowsPanel({ run, vs, setupKey, rowId, onSetup, onRow, onOpenInPlayground }: ViewProps) {
  const rows = run.rows.filter((r) => setupKey === null || r.setup === setupKey);
  const before = new Map(vs?.rows.map((r) => [r.id, r]) ?? []);
  const open = rowId === null ? null : (run.rows.find((r) => r.id === rowId) ?? null);
  return (
    <div className="grid gap-4 xl:grid-cols-[minmax(0,7fr)_minmax(0,5fr)]">
      <Panel
        title={setupKey === null ? `All rows (${rows.length})` : `Rows of ${setupKey} (${rows.length})`}
        subtitle={setupKey === null ? 'click a row for its details' : SETUP_BY_KEY.get(setupKey)?.title}
        aside={
          setupKey !== null && (
            <button type="button" onClick={() => onSetup(null)} className="rounded-md px-2 py-0.5 text-xs text-teal-300 hover:bg-slate-800">
              show every setup
            </button>
          )
        }
      >
        <div className="scroll-slim max-h-[560px] overflow-auto">
          <table className="w-full text-xs">
            <thead className="sticky top-0 bg-slate-900 text-slate-400">
              <tr className="border-b border-slate-700">
                <th className="px-2 py-1 text-left font-medium">row</th>
                <th className="px-2 py-1 text-left font-medium">name</th>
                <th className="px-2 py-1 text-right font-medium">size</th>
                <th className="px-2 py-1 text-left font-medium">outcome</th>
                <th className="px-2 py-1 text-right font-medium">raw</th>
                <th className="px-2 py-1 text-right font-medium">-Oz</th>
                <th className="px-2 py-1 text-right font-medium">-Oz ratio</th>
              </tr>
            </thead>
            <tbody>
              {rows.map((r) => {
                const b = before.get(r.id);
                const name = ROW_BY_ID.get(r.id)?.name ?? null;
                return (
                  <tr
                    key={r.id}
                    onClick={() => onRow(r.id === rowId ? null : r.id)}
                    className={`cursor-pointer border-b border-slate-800 ${r.id === rowId ? 'bg-teal-500/10' : 'hover:bg-slate-800/60'}`}
                  >
                    <td className="whitespace-nowrap px-2 py-1 font-mono text-slate-200">{r.id}</td>
                    <td className="max-w-[320px] truncate px-2 py-1 font-mono text-slate-400" title={name ?? undefined}>
                      {name ?? <span className="text-slate-600">no ledger name</span>}
                    </td>
                    <td className="px-2 py-1 text-right font-mono">{r.size}</td>
                    <td className="px-2 py-1">
                      <Pill tint={outcomeTint(r.outcome)} size="xs">
                        {r.outcome}
                      </Pill>
                    </td>
                    <td className="px-2 py-1 text-right font-mono">{r.raw?.score ?? '–'}</td>
                    <td className="px-2 py-1 text-right font-mono">
                      {r.opt?.score ?? '–'}
                      {b?.opt && r.opt && b.opt.score !== r.opt.score && <DeltaMark d={r.opt.score - b.opt.score} lowerIsBetter />}
                    </td>
                    <td className="px-2 py-1 text-right font-mono">{ratioText(r.opt?.ratio)}</td>
                  </tr>
                );
              })}
            </tbody>
          </table>
        </div>
      </Panel>
      {open === null ? (
        <Panel title="Row details">
          <p className="text-xs text-slate-500">Pick a row to see its flags, reference source, failure and the lifter's stand-ins.</p>
        </Panel>
      ) : (
        <RowDetail result={open} row={ROW_BY_ID.get(open.id) ?? null} onOpenInPlayground={onOpenInPlayground} />
      )}
    </div>
  );
}

function RowDetail({
  result,
  row,
  onOpenInPlayground,
}: {
  result: RowResult;
  row: DatasetRow | null;
  onOpenInPlayground: (index: number) => void;
}) {
  const reference = row === null ? null : referenceText(row);
  return (
    <Panel
      title={`Row ${result.id}`}
      subtitle={row?.name ?? undefined}
      aside={
        <button
          type="button"
          onClick={() => onOpenInPlayground(result.index)}
          className="shrink-0 rounded-md border border-teal-700 bg-teal-900/40 px-3 py-1 text-xs font-medium text-teal-300 hover:bg-teal-900/70"
        >
          Open #{result.index} in the playground
        </button>
      }
    >
      <dl className="grid grid-cols-[auto_1fr] gap-x-4 gap-y-1.5">
        <Fact term="outcome">
          <Pill tint={outcomeTint(result.outcome)} size="xs">
            {result.outcome}
          </Pill>
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
        <Fact term="size">
          {result.size} bytes ({result.band})
        </Fact>
        <Fact term="features">
          <div className="flex flex-wrap gap-1">
            {result.features.map((f) => (
              <Pill key={f} size="xs" mono>
                {f}
              </Pill>
            ))}
          </div>
        </Fact>
        <Fact term="stand-ins">
          {result.standIns.length === 0 ? (
            'none'
          ) : (
            <div className="flex flex-wrap gap-1">
              {result.standIns.map((s) => (
                <Pill key={s} size="xs">
                  {s}
                </Pill>
              ))}
            </div>
          )}
        </Fact>
        {row && (
          <>
            <Fact term="setup">
              {row.setup} ({row.lang}){row.instantiatedBy && `, instantiated by ${row.instantiatedBy}`}
            </Fact>
            <Fact term="build flags">
              <span className="font-mono text-xs">{row.flags}</span>
            </Fact>
            <Fact term="ledger">
              {row.ledger.kind}
              {row.ledger.score !== null && row.ledger.score > 0 && `, ${row.ledger.score} ${row.ledger.score === 1 ? 'row differs' : 'rows differ'}`}
              {row.component !== 'unknown' && ` · component ${row.component}`}
            </Fact>
            <Fact term="object">
              <span className="break-all font-mono text-xs">{row.object ?? 'none (decided by component)'}</span>
            </Fact>
            <Fact term="reference">
              {reference === null ? 'none yet' : <span className="break-all font-mono text-xs">{reference}</span>}
            </Fact>
          </>
        )}
      </dl>
    </Panel>
  );
}

function DeltaMark({ d, lowerIsBetter = false }: { d: number; lowerIsBetter?: boolean }) {
  const good = lowerIsBetter ? d < 0 : d > 0;
  return <span className={`ml-1 ${good ? 'text-teal-300' : 'text-amber-300'}`}>({signed(d, 0)})</span>;
}

function signed(d: number, digits: number): string {
  return `${d > 0 ? '+' : ''}${d.toFixed(digits)}`;
}

function pct(n: number, of: number): string {
  return of === 0 ? '–' : `${Math.round((100 * n) / of)}%`;
}

function delta(before: number | undefined, after: number): string | null {
  return before === undefined ? null : `${signed(after - before, 0)} against the compared run`;
}

function ratioDelta(before: number | null | undefined, after: number | null): string | null {
  return before === undefined || before === null || after === null ? null : `${signed(after - before, 3)} against the compared run`;
}
