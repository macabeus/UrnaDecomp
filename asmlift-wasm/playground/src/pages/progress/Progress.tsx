// The decompilation meter: the official number (splat/matched.txt's matches, as the README gives it), a waffle
// that drills from origins to components to functions, and the matches over time. Every square of the waffles is
// read from splat/scan.tsv; the drill-down, the metric and the order live in the URL fragment.
import { useQueryState } from 'nuqs';
import { type ReactNode, useMemo, useState } from 'react';

import {
  type ComponentRow,
  type Fn,
  type Meter,
  type Metric,
  ORIGINS,
  type OriginId,
  type Tally,
  componentsOf,
  matched,
  meter,
  num,
  pct,
  share,
  total,
} from '../../lib/progress';
import { type ProgressData, VERDICTS } from '../../lib/progress-data';
import { Panel } from '../../shared/components/Panel';
import { useProgress } from '../../shared/data/progress';
import { SERIES_COLORS, SERIES_TOTAL, VERDICT_COLORS, VERDICT_LABELS, VERDICT_TINTS } from '../../shared/theme';
import { SORTS, type Sort, componentParser, metricParser, originParser, sortParser } from '../../shared/utils/url-state';
import { type FnGroup, FunctionWaffle } from './FunctionWaffle';
import { HistoryChart, type Series } from './HistoryChart';
import { Waffle } from './Waffle';

const ORIGIN_COLOR: Record<OriginId, string> = { tse: SERIES_COLORS[0], upstream: SERIES_COLORS[1], unattributed: SERIES_COLORS[2] };

export default function Progress({ onOpenInPlayground }: { onOpenInPlayground: (index: number) => void }) {
  const loaded = useProgress();
  if (loaded.state === 'loading') {
    return <div className="grid min-h-[50vh] place-items-center text-sm text-slate-500">loading the meter…</div>;
  }
  if (loaded.state === 'error') {
    return <div className="rounded-lg border border-rose-900 bg-rose-950/40 p-4 font-mono text-sm text-rose-300">{loaded.error}</div>;
  }
  return <ProgressView data={loaded.value} onOpenInPlayground={onOpenInPlayground} />;
}

function ProgressView({ data, onOpenInPlayground }: { data: ProgressData; onOpenInPlayground: (index: number) => void }) {
  const m = useMemo(() => meter(data), [data]);
  const [origin, setOrigin] = useQueryState('origin', originParser.withOptions({ history: 'push' }));
  const [componentId, setComponent] = useQueryState('component', componentParser.withOptions({ history: 'push' }));
  const [metric, setMetric] = useQueryState('by', metricParser);
  const [sort, setSort] = useQueryState('sort', sortParser);
  const [selected, setSelected] = useState<Fn | null>(null);

  const component = componentId === null ? null : (m.byComponent.get(componentId) ?? null);
  const level: OriginId | null = component?.origin ?? origin;

  const groups = useMemo(() => groupsFor(m, level, component, sort), [m, level, component, sort]);
  const series = useMemo(() => seriesFor(m, data, level, component, metric), [m, data, level, component, metric]);

  const go = (o: OriginId | null, c: string | null) => {
    void setOrigin(o);
    void setComponent(c);
  };

  return (
    <div className="flex flex-col gap-5">
      <Hero m={m} data={data} />

      <div className="flex flex-wrap items-center justify-between gap-3">
        <Breadcrumb level={level} component={component} onGo={go} />
        <div className="flex items-center gap-3">
          <Legend />
          <Toggle
            value={metric}
            options={[
              ['functions', 'Functions'],
              ['bytes', 'Code bytes'],
            ]}
            onChange={(v) => void setMetric(v)}
          />
        </div>
      </div>

      {component === null ? (
        <CardGrid m={m} metric={metric} origin={level} onGo={go} />
      ) : (
        <ComponentHeader row={component} metric={metric} />
      )}

      <div className="grid gap-5 xl:grid-cols-[minmax(0,1fr)_340px]">
        <Panel
          title={component ? `Every function of ${component.id}` : level ? `Every function of ${label(level)}` : 'Every function in the module'}
          subtitle="one square per target function, coloured by its verdict"
          aside={
            <Toggle
              value={sort}
              options={SORTS.map((s) => [s, s === 'verdict' ? 'By verdict' : s === 'index' ? 'By index' : 'By size'] as [Sort, string])}
              onChange={(v) => void setSort(v)}
            />
          }
        >
          <FunctionWaffle
            groups={groups}
            cell={component ? 13 : level ? 8 : 5}
            gap={component ? 3 : level ? 2 : 1}
            selected={selected?.index ?? null}
            onSelect={setSelected}
          />
        </Panel>
        <FunctionCard fn={selected} data={data} onOpenInPlayground={onOpenInPlayground} onGo={go} />
      </div>

      <Panel
        title="Matches over time"
        subtitle={`the share of ${metric === 'functions' ? 'functions' : 'code bytes'} matched at every commit of main that changed splat/matched.txt`}
      >
        <HistoryChart data={data} series={series} metric={metric} />
        <Timeline data={data} m={m} />
      </Panel>
    </div>
  );
}

const label = (o: OriginId): string => ORIGINS.find((x) => x.id === o)!.label;

function Hero({ m, data }: { m: Meter; data: ProgressData }) {
  const tse = m.byOrigin.get('tse')!;
  return (
    <section className="relative overflow-hidden rounded-2xl border border-slate-700/50 bg-slate-800/40 p-6 shadow-xl sm:p-8">
      <div className="pointer-events-none absolute -right-24 -top-24 h-72 w-72 rounded-full bg-teal-500/10 blur-3xl" />
      <div className="pointer-events-none absolute -bottom-24 left-1/3 h-60 w-60 rounded-full bg-amber-400/10 blur-3xl" />
      <div className="pointer-events-none absolute -left-20 top-1/4 h-56 w-56 rounded-full bg-blue-500/15 blur-3xl" />
      <div className="relative grid gap-8 lg:grid-cols-[minmax(0,1.3fr)_minmax(0,1fr)]">
        <div>
          <p className="text-xs font-semibold uppercase tracking-[0.2em] text-teal-300/80">Decompilation progress</p>
          <div className="mt-3 flex flex-wrap items-baseline gap-x-4 gap-y-1">
            <span className="bg-gradient-to-r from-teal-200 via-teal-300 to-amber-200 bg-clip-text font-mono text-6xl font-bold tracking-tight text-transparent sm:text-7xl">
              {pct(share(tse, 'functions'))}
            </span>
            <span className="text-lg text-slate-300">of TSE's own code matched</span>
          </div>
          <p className="mt-3 max-w-2xl text-sm leading-relaxed text-slate-400">
            Recovering TSE's code is the project's goal. <span className="font-mono text-slate-200">{num(matched(tse, 'functions'))}</span>{' '}
            of its <span className="font-mono text-slate-200">{num(total(tse, 'functions'))}</span> functions in{' '}
            <span className="font-mono">vota_web_wasm.wasm</span> are built byte for byte by our link (
            <span className="font-mono text-slate-200">{pct(share(tse, 'bytes'))}</span> of its code bytes).
          </p>
          <VerdictBar tally={tse} metric="functions" className="mt-5" />
        </div>
        <div className="grid grid-cols-2 gap-3 self-start">
          <BigStat
            label="The whole module"
            value={pct(share(m.all, 'functions'))}
            hint={`${num(matched(m.all, 'functions'))} / ${num(total(m.all, 'functions'))} functions`}
            accent
          />
          <BigStat
            label="Its code bytes"
            value={pct(share(m.all, 'bytes'))}
            hint={`${num(matched(m.all, 'bytes'))} / ${num(total(m.all, 'bytes'))} B`}
          />
          <BigStat
            label="Upstream libraries"
            value={pct(share(m.byOrigin.get('upstream')!, 'functions'))}
            hint="rebuilt from their pinned sources"
          />
          <BigStat
            label="As of"
            value={<span className="text-xl">{data.commit ?? 'working tree'}</span>}
            hint={data.date ? `${data.date.slice(0, 16).replace('T', ' ')} UTC` : undefined}
          />
        </div>
      </div>
    </section>
  );
}

function BigStat({ label, value, hint, accent = false }: { label: string; value: ReactNode; hint?: string; accent?: boolean }) {
  return (
    <div className={`rounded-xl border p-4 ${accent ? 'border-amber-400/30 bg-amber-400/5' : 'border-slate-700/50 bg-slate-900/40'}`}>
      <div className="text-[11px] uppercase tracking-wide text-slate-400">{label}</div>
      <div className={`mt-1 font-mono text-3xl font-bold ${accent ? 'text-amber-300' : 'text-slate-100'}`}>{value}</div>
      {hint && <div className="mt-1 text-xs text-slate-500">{hint}</div>}
    </div>
  );
}

function VerdictBar({ tally, metric, className = '' }: { tally: Tally; metric: Metric; className?: string }) {
  const vs = metric === 'functions' ? tally.fns : tally.bytes;
  return (
    <div className={className}>
      <div className="flex h-3 gap-[2px] overflow-hidden rounded-full">
        {vs.map((v, i) =>
          v === 0 ? null : (
            <div
              key={VERDICTS[i]}
              title={`${VERDICT_LABELS[i]}: ${num(v)}`}
              className="min-w-[3px] first:rounded-l-full last:rounded-r-full"
              style={{ flexGrow: v, flexBasis: 0, backgroundColor: VERDICT_COLORS[i], transition: 'flex-grow 500ms ease' }}
            />
          ),
        )}
      </div>
      <div className="mt-2 flex flex-wrap gap-x-4 gap-y-1 text-xs text-slate-400">
        {vs.map((v, i) => (
          <span key={VERDICTS[i]} className="flex items-center gap-1.5">
            <span className="h-2 w-2 rounded-sm" style={{ backgroundColor: VERDICT_COLORS[i] }} />
            {VERDICT_LABELS[i]} <span className="font-mono text-slate-300">{num(v)}</span>
          </span>
        ))}
      </div>
    </div>
  );
}

function Legend() {
  return (
    <div className="hidden items-center gap-3 text-xs text-slate-400 md:flex">
      {VERDICT_LABELS.map((l, i) => (
        <span key={l} className="flex items-center gap-1.5">
          <span className="h-2.5 w-2.5 rounded-[3px]" style={{ backgroundColor: VERDICT_COLORS[i] }} />
          {l}
        </span>
      ))}
    </div>
  );
}

function Toggle<T extends string>({ value, options, onChange }: { value: T; options: [T, string][]; onChange: (v: T) => void }) {
  return (
    <div className="flex rounded-lg border border-slate-700/60 bg-slate-900/50 p-0.5 text-xs">
      {options.map(([v, l]) => (
        <button
          key={v}
          type="button"
          onClick={() => onChange(v)}
          className={`rounded-md px-2.5 py-1 transition-colors ${
            v === value ? 'bg-teal-500/20 text-teal-200' : 'text-slate-400 hover:text-slate-200'
          }`}
        >
          {l}
        </button>
      ))}
    </div>
  );
}

function Breadcrumb({
  level,
  component,
  onGo,
}: {
  level: OriginId | null;
  component: ComponentRow | null;
  onGo: (o: OriginId | null, c: string | null) => void;
}) {
  const crumbs: [string, () => void][] = [['All origins', () => onGo(null, null)]];
  if (level !== null) {
    crumbs.push([label(level), () => onGo(level, null)]);
  }
  if (component !== null) {
    crumbs.push([component.id, () => onGo(component.origin, component.id)]);
  }
  return (
    <nav className="flex items-center gap-1.5 text-sm" aria-label="drill-down">
      {crumbs.map(([l, go], i) => (
        <span key={l} className="flex items-center gap-1.5">
          {i > 0 && <span className="text-slate-600">›</span>}
          <button
            type="button"
            onClick={go}
            className={i === crumbs.length - 1 ? 'font-semibold text-slate-100' : 'text-slate-400 hover:text-teal-300'}
          >
            {l}
          </button>
        </span>
      ))}
    </nav>
  );
}

function CardGrid({
  m,
  metric,
  origin,
  onGo,
}: {
  m: Meter;
  metric: Metric;
  origin: OriginId | null;
  onGo: (o: OriginId | null, c: string | null) => void;
}) {
  if (origin === null) {
    return (
      <div className="grid gap-4 md:grid-cols-3">
        {ORIGINS.map((o) => (
          <Card
            key={o.id}
            title={o.label}
            subtitle={o.blurb}
            tally={m.byOrigin.get(o.id)!}
            metric={metric}
            onClick={() => onGo(o.id, null)}
            big
          />
        ))}
      </div>
    );
  }
  return (
    <div className="grid gap-3 sm:grid-cols-2 lg:grid-cols-3 2xl:grid-cols-4">
      {componentsOf(m, origin).map((c) => (
        <Card key={c.id} title={c.id} subtitle={c.description} tally={c.tally} metric={metric} onClick={() => onGo(c.origin, c.id)} mono />
      ))}
    </div>
  );
}

function Card({
  title,
  subtitle,
  tally,
  metric,
  onClick,
  big = false,
  mono = false,
}: {
  title: string;
  subtitle: string;
  tally: Tally;
  metric: Metric;
  onClick: () => void;
  big?: boolean;
  mono?: boolean;
}) {
  const values = metric === 'functions' ? tally.fns : tally.bytes;
  const unit = metric === 'functions' ? 'functions' : 'B';
  return (
    <button
      type="button"
      onClick={onClick}
      className="group flex items-center gap-4 rounded-xl border border-slate-700/50 bg-slate-800/40 p-4 text-left transition-all hover:-translate-y-0.5 hover:border-teal-500/40 hover:bg-slate-800/70 hover:shadow-lg hover:shadow-teal-500/5"
    >
      <Waffle values={values} size={big ? 132 : 96} label={`${title}: ${pct(share(tally, metric))} matched`} />
      <div className="min-w-0 flex-1">
        <h3 className={`truncate font-semibold text-slate-100 ${mono ? 'font-mono text-sm' : 'text-base'}`}>{title}</h3>
        <div className={`mt-1 font-mono font-bold text-teal-200 ${big ? 'text-3xl' : 'text-2xl'}`}>{pct(share(tally, metric), 1)}</div>
        <div className="font-mono text-xs text-slate-400">
          {num(matched(tally, metric))} / {num(total(tally, metric))} {unit}
        </div>
        <p className="mt-1.5 line-clamp-2 text-xs leading-snug text-slate-500">{subtitle}</p>
      </div>
      <span className="self-start text-slate-600 transition-colors group-hover:text-teal-300">→</span>
    </button>
  );
}

function ComponentHeader({ row, metric }: { row: ComponentRow; metric: Metric }) {
  const t = row.tally;
  return (
    <section className="flex flex-wrap items-center gap-6 rounded-xl border border-slate-700/50 bg-slate-800/40 p-5">
      <Waffle values={metric === 'functions' ? t.fns : t.bytes} size={120} label={`${row.id}: ${pct(share(t, metric))} matched`} />
      <div className="min-w-0 flex-1">
        <h2 className="font-mono text-xl font-semibold text-slate-100">{row.id}</h2>
        <p className="mt-0.5 text-sm text-slate-400">{row.description}</p>
        <div className="mt-3 flex flex-wrap gap-6">
          <div>
            <div className="text-[11px] uppercase tracking-wide text-slate-500">functions</div>
            <div className="font-mono text-lg text-slate-100">
              {num(matched(t, 'functions'))} / {num(total(t, 'functions'))}{' '}
              <span className="text-teal-300">{pct(share(t, 'functions'))}</span>
            </div>
          </div>
          <div>
            <div className="text-[11px] uppercase tracking-wide text-slate-500">code bytes</div>
            <div className="font-mono text-lg text-slate-100">
              {num(matched(t, 'bytes'))} / {num(total(t, 'bytes'))} <span className="text-teal-300">{pct(share(t, 'bytes'))}</span>
            </div>
          </div>
        </div>
        <VerdictBar tally={t} metric={metric} className="mt-4 max-w-xl" />
      </div>
    </section>
  );
}

const VERDICT_RANK = new Map(VERDICTS.map((v, i) => [v, i]));

function order(fns: Fn[], sort: Sort): Fn[] {
  const out = [...fns];
  if (sort === 'verdict') {
    out.sort((a, b) => VERDICT_RANK.get(a.verdict)! - VERDICT_RANK.get(b.verdict)! || a.index - b.index);
  } else if (sort === 'size') {
    out.sort((a, b) => b.size - a.size || a.index - b.index);
  }
  return out;
}

function groupsFor(m: Meter, origin: OriginId | null, component: ComponentRow | null, sort: Sort): FnGroup[] {
  const byComponent = new Map<string, Fn[]>();
  for (const f of m.fns) {
    const list = byComponent.get(f.component);
    if (list === undefined) {
      byComponent.set(f.component, [f]);
    } else {
      list.push(f);
    }
  }
  const caption = (name: string, t: Tally) => `${name}  ·  ${num(t.fns[0])} / ${num(total(t, 'functions'))} matched`;
  if (component !== null) {
    return [{ key: component.id, label: caption(component.id, component.tally), fns: order(byComponent.get(component.id) ?? [], sort) }];
  }
  if (origin !== null) {
    return componentsOf(m, origin).map((c) => ({ key: c.id, label: caption(c.id, c.tally), fns: order(byComponent.get(c.id) ?? [], sort) }));
  }
  return ORIGINS.map((o) => ({
    key: o.id,
    label: caption(o.label, m.byOrigin.get(o.id)!),
    fns: order(
      componentsOf(m, o.id).flatMap((c) => byComponent.get(c.id) ?? []),
      sort,
    ),
  }));
}

function seriesFor(m: Meter, data: ProgressData, origin: OriginId | null, component: ComponentRow | null, metric: Metric): Series[] {
  const id = new Map(data.components.map((c, i) => [c, i]));
  const ids = (rows: ComponentRow[]) => rows.map((r) => id.get(r.id)!);
  if (component !== null) {
    return [{ name: component.id, color: SERIES_COLORS[1], components: [id.get(component.id)!], total: total(component.tally, metric) }];
  }
  if (origin !== null) {
    const rows = componentsOf(m, origin);
    return [
      { name: label(origin), color: SERIES_TOTAL, components: ids(rows), total: total(m.byOrigin.get(origin)!, metric) },
      ...rows.map((r, i) => ({
        name: r.id,
        color: SERIES_COLORS[i % SERIES_COLORS.length],
        components: [id.get(r.id)!],
        total: total(r.tally, metric),
      })),
    ];
  }
  return [
    { name: 'all functions', color: SERIES_TOTAL, components: data.components.map((_, i) => i), total: total(m.all, metric) },
    ...ORIGINS.map((o) => ({
      name: o.label,
      color: ORIGIN_COLOR[o.id],
      components: ids(componentsOf(m, o.id)),
      total: total(m.byOrigin.get(o.id)!, metric),
    })),
  ];
}

function FunctionCard({
  fn,
  data,
  onOpenInPlayground,
  onGo,
}: {
  fn: Fn | null;
  data: ProgressData;
  onOpenInPlayground: (index: number) => void;
  onGo: (o: OriginId | null, c: string | null) => void;
}) {
  if (fn === null) {
    return (
      <Panel title="Function" className="xl:sticky xl:top-4 xl:self-start">
        <p className="text-sm leading-relaxed text-slate-400">
          Click a square to see the function: its name, its verdict and, for one that differs, where our body first
          departs from the target's.
        </p>
        <dl className="mt-4 space-y-2 text-xs text-slate-400">
          {VERDICT_LABELS.map((l, i) => (
            <div key={l} className="flex gap-2">
              <span className="mt-0.5 h-2.5 w-2.5 shrink-0 rounded-[3px]" style={{ backgroundColor: VERDICT_COLORS[i] }} />
              <span>
                <span className="font-medium text-slate-300">{l}</span>: {VERDICT_HELP[i]}
              </span>
            </div>
          ))}
        </dl>
      </Panel>
    );
  }
  const v = VERDICT_RANK.get(fn.verdict)!;
  const detail = data.detail[fn.index];
  return (
    <Panel title={<span className="font-mono">f{fn.index}</span>} className="xl:sticky xl:top-4 xl:self-start">
      <div className="flex flex-col gap-3">
        <span
          className="inline-flex w-fit items-center gap-1.5 rounded-md px-2 py-0.5 text-xs font-medium"
          style={{ backgroundColor: `${VERDICT_COLORS[v]}26`, color: VERDICT_TINTS[v] }}
        >
          <span className="h-2 w-2 rounded-sm" style={{ backgroundColor: VERDICT_COLORS[v] }} />
          {VERDICT_LABELS[v]}
        </span>
        <div className="break-all font-mono text-sm text-slate-100">{fn.name || <span className="text-slate-500">no name yet</span>}</div>
        <dl className="grid grid-cols-[auto_1fr] gap-x-3 gap-y-1 text-xs">
          <dt className="text-slate-500">component</dt>
          <dd>
            <button type="button" className="font-mono text-teal-300 hover:underline" onClick={() => onGo(null, fn.component)}>
              {fn.component}
            </button>
          </dd>
          <dt className="text-slate-500">size</dt>
          <dd className="font-mono text-slate-200">{num(fn.size)} B</dd>
        </dl>
        {detail && (
          <div>
            <div className="text-[11px] uppercase tracking-wide text-slate-500">first difference</div>
            <div className="mt-1 break-words rounded-md bg-slate-900/70 p-2 font-mono text-[11px] text-slate-300">{detail}</div>
          </div>
        )}
        <button
          type="button"
          onClick={() => onOpenInPlayground(fn.index)}
          className="mt-1 rounded-lg border border-teal-500/30 bg-teal-500/15 px-3 py-2 text-sm font-medium text-teal-200 transition-colors hover:bg-teal-500/25"
        >
          Open in the playground →
        </button>
      </div>
    </Panel>
  );
}

const VERDICT_HELP = [
  'our link builds its body byte for byte (a line of splat/matched.txt)',
  'our link has a body for it and it differs: the closest to a match',
  'our body has an operand with no counterpart in the target',
  'our link has no body for it yet',
];

function Timeline({ data, m }: { data: ProgressData; m: Meter }) {
  const rows = data.history
    .map((p, i) => {
      const n = p.matched.reduce((a, c) => a + c[0], 0);
      const prev = i === 0 ? null : data.history[i - 1].matched.reduce((a, c) => a + c[0], 0);
      return { p, n, delta: prev === null ? null : n - prev };
    })
    .reverse();
  const all = total(m.all, 'functions');
  return (
    <div className="mt-4 border-t border-slate-700/50 pt-3">
      <h4 className="mb-2 text-xs font-semibold uppercase tracking-wide text-slate-500">Commits</h4>
      <ol className="scroll-slim max-h-64 space-y-1 overflow-y-auto pr-2 text-xs">
        {rows.map(({ p, n, delta }) => (
          <li key={p.date + (p.commit ?? '')} className="grid grid-cols-[4.5rem_8.5rem_minmax(0,1fr)_auto] items-baseline gap-3">
            <span
              className={`text-right font-mono ${
                delta === null ? 'text-slate-500' : delta > 0 ? 'text-teal-300' : delta < 0 ? 'text-rose-300' : 'text-slate-500'
              }`}
            >
              {delta === null ? 'start' : delta > 0 ? `+${delta}` : delta === 0 ? '±0' : delta}
            </span>
            <span className="font-mono text-slate-500">{p.date.slice(0, 16).replace('T', ' ')}</span>
            <span className="truncate text-slate-300">
              {p.commit ? (
                <>
                  <span className="font-mono text-slate-500">{p.commit}</span> {p.subject}
                </>
              ) : (
                <span className="text-slate-500">not in the history of main</span>
              )}
            </span>
            <span className="font-mono text-slate-400">
              {num(n)} <span className="text-slate-600">· {((100 * n) / all).toFixed(2)}%</span>
            </span>
          </li>
        ))}
      </ol>
    </div>
  );
}
