// A run's Markdown report, and the comparison of two runs.
import { FEATURES, type BenchResults, type RowResult, type Totals } from './schema';
import { totals } from './run';

const fixed = (x: number | null): string => (x === null ? '–' : x.toFixed(3));
const pct = (n: number, d: number): string => (d === 0 ? '–' : `${((100 * n) / d).toFixed(1)}%`);

function table(head: string[], body: (string | number)[][]): string[] {
  return [
    `| ${head.join(' | ')} |`,
    `| ${head.map(() => '---').join(' | ')} |`,
    ...body.map((r) => `| ${r.map((c) => String(c).replace(/\|/g, '\\|').replace(/\n/g, ' ')).join(' | ')} |`),
  ];
}

const TOTALS_HEAD = ['rows', 'lifted', 'compiled', '-Oz linked', 'exact raw', 'exact -Oz', 'exact share', 'median ratio', 'median -Oz', 'mean -Oz'];

function totalsCells(t: Totals): (string | number)[] {
  return [
    t.rows,
    t.lifted,
    t.compiled,
    t.optimised,
    t.exactRaw,
    t.exact,
    pct(t.exact, t.rows),
    fixed(t.medianRatio),
    fixed(t.medianOptRatio),
    fixed(t.meanOptRatio),
  ];
}

export function renderBenchReport(r: BenchResults, setupTitles: ReadonlyMap<string, string> = new Map()): string {
  const out: string[] = [];
  out.push(`# Benchmark: ${r.label}`, '');
  out.push(
    `Target \`${r.target.path}\` (sha256 \`${r.target.sha256.slice(0, 16)}…\`), image \`${r.image}\`, ` +
      `${r.dataset.rows} rows (rows.json \`${r.dataset.rowsSha256.slice(0, 12)}\`, setups.json \`${r.dataset.setupsSha256.slice(0, 12)}\`), ` +
      `urna \`${r.commit ?? '–'}\`, ${r.createdAt}. Lift ${r.durationMs.lift} ms, compile and optimise ` +
      `${(r.durationMs.compileAndOptimise / 1000).toFixed(1)} s, total ${(r.durationMs.total / 1000).toFixed(1)} s.`,
    '',
  );
  out.push(
    'Each row is lifted, compiled with `em++ -c` at its setup\'s flags, and scored against the target at depth 0 ' +
      'with callees, data addresses, the stack pointer and the indirect table canonicalised on both sides (raw); then ' +
      'the object is linked alone and put through the target link\'s wasm-opt -Oz passes and scored again (-Oz). ' +
      'The ratio is differing rows over target rows; exact is a -Oz score of 0. An isolated function misses what the ' +
      'whole-program link does to it, so no score here is a byte match.',
    '',
  );
  out.push('## Overall', '', ...table(TOTALS_HEAD, [totalsCells(r.totals.overall)]), '');
  out.push('## By compile setup', '');
  out.push(
    ...table(
      ['setup', ...TOTALS_HEAD, 'failures'],
      Object.entries(r.totals.bySetup).map(([k, t]) => [
        setupTitles.has(k) ? `\`${k}\` ${setupTitles.get(k)}` : `\`${k}\``,
        ...totalsCells(t),
        Object.entries(t.outcomes)
          .filter(([o]) => o !== 'exact' && o !== 'differs')
          .map(([o, n]) => `${o} ${n}`)
          .join(', ') || '–',
      ]),
    ),
    '',
  );
  const bands = [...new Set(r.rows.map((x) => x.band))];
  const bandOrder = ['tiny', 'small', 'medium', 'large', 'huge'];
  out.push('## By size band', '');
  out.push(
    ...table(
      ['band', ...TOTALS_HEAD],
      bandOrder.filter((b) => bands.includes(b)).map((b) => [b, ...totalsCells(totals(r.rows.filter((x) => x.band === b)))]),
    ),
    '',
  );
  out.push('## By feature', '', 'A row counts under every feature it has.', '');
  out.push(
    ...table(
      ['feature', ...TOTALS_HEAD],
      FEATURES.filter((f) => r.rows.some((x) => x.features.includes(f))).map((f) => [
        f,
        ...totalsCells(totals(r.rows.filter((x) => x.features.includes(f)))),
      ]),
    ),
    '',
  );
  const failed = r.rows.filter((x) => x.failure !== null);
  out.push('## Failures', '');
  if (failed.length === 0) {
    out.push('None: every row lifted, compiled, linked and was found.', '');
  } else {
    const classes = new Map<string, RowResult[]>();
    for (const x of failed) {
      classes.set(x.failure!.class, [...(classes.get(x.failure!.class) ?? []), x]);
    }
    out.push(
      ...table(
        ['class', 'rows', 'ids', 'first line'],
        [...classes]
          .sort((a, b) => b[1].length - a[1].length)
          .map(([c, xs]) => [c, xs.length, xs.map((x) => x.id).join(', '), `\`${xs[0].failure!.line.slice(0, 160)}\``]),
      ),
      '',
    );
  }
  const worst = r.rows
    .filter((x) => x.opt !== null && x.opt.score > 0)
    .sort((a, b) => b.opt!.ratio - a.opt!.ratio || a.index - b.index)
    .slice(0, 15);
  out.push('## Largest -Oz ratios', '');
  out.push(
    ...table(
      ['id', 'size', 'target rows', 'raw', 'ratio', '-Oz', 'ratio -Oz', 'features'],
      worst.map((x) => [
        x.id,
        x.size,
        x.targetRows,
        x.raw?.score ?? '–',
        fixed(x.raw?.ratio ?? null),
        x.opt!.score,
        fixed(x.opt!.ratio),
        x.features.join(' '),
      ]),
    ),
    '',
  );
  return out.join('\n');
}

const delta = (a: number | null, b: number | null, digits = 3): string =>
  a === null || b === null ? `${a ?? '–'} → ${b ?? '–'}` : `${a.toFixed(digits)} → ${b.toFixed(digits)}`;

const optKey = (x: RowResult | undefined): number => (x === undefined ? Number.NaN : (x.opt?.score ?? Infinity));

/** What moved from run `a` to run `b`: totals per setup, then every row whose outcome or -Oz score changed. */
export function renderDiff(a: BenchResults, b: BenchResults): string {
  const out: string[] = [];
  out.push(`# Benchmark diff: ${a.label} → ${b.label}`, '');
  if (a.dataset.rowsSha256 !== b.dataset.rowsSha256) {
    out.push('> The two runs used different datasets: rows only in one run are listed apart.', '');
  }
  const keys = [...new Set([...Object.keys(a.totals.bySetup), ...Object.keys(b.totals.bySetup)])];
  const row = (k: string, ta: Totals | undefined, tb: Totals | undefined): (string | number)[] => [
    k,
    `${ta?.rows ?? 0} → ${tb?.rows ?? 0}`,
    `${ta?.compiled ?? 0} → ${tb?.compiled ?? 0}`,
    `${ta?.exactRaw ?? 0} → ${tb?.exactRaw ?? 0}`,
    `${ta?.exact ?? 0} → ${tb?.exact ?? 0}`,
    delta(ta?.medianRatio ?? null, tb?.medianRatio ?? null),
    delta(ta?.medianOptRatio ?? null, tb?.medianOptRatio ?? null),
  ];
  out.push(
    ...table(
      ['setup', 'rows', 'compiled', 'exact raw', 'exact -Oz', 'median ratio', 'median -Oz'],
      [row('**overall**', a.totals.overall, b.totals.overall), ...keys.map((k) => row(`\`${k}\``, a.totals.bySetup[k], b.totals.bySetup[k]))],
    ),
    '',
  );
  const ra = new Map(a.rows.map((x) => [x.id, x]));
  const rb = new Map(b.rows.map((x) => [x.id, x]));
  const moved: [RowResult | undefined, RowResult | undefined][] = [];
  for (const id of new Set([...ra.keys(), ...rb.keys()])) {
    const x = ra.get(id);
    const y = rb.get(id);
    if (x === undefined || y === undefined || x.outcome !== y.outcome || optKey(x) !== optKey(y)) {
      moved.push([x, y]);
    }
  }
  const better = moved.filter(([x, y]) => x !== undefined && y !== undefined && optKey(y) < optKey(x)).length;
  const worse = moved.filter(([x, y]) => x !== undefined && y !== undefined && optKey(y) > optKey(x)).length;
  out.push(`${better} rows scored better at -Oz, ${worse} worse, ${moved.length - better - worse} changed otherwise or are in one run only.`, '');
  if (moved.length > 0) {
    const cell = (x: RowResult | undefined): string =>
      x === undefined ? 'absent' : x.opt === null ? x.outcome : `${x.opt.score} (${x.opt.ratio.toFixed(3)})`;
    moved.sort(([x1, y1], [x2, y2]) => (x1 ?? y1)!.id.localeCompare((x2 ?? y2)!.id));
    out.push(...table(['id', a.label, b.label], moved.map(([x, y]) => [(x ?? y)!.id, cell(x), cell(y)])), '');
  }
  return out.join('\n');
}
