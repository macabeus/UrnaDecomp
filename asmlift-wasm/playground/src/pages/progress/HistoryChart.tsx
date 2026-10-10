// The matches at every commit of main that changed splat/matched.txt, as the share of each series' functions
// or code bytes (today's scan gives every point its components and sizes, so the shares are comparable).
import { type Metric, matchedAt, num } from '../../lib/progress';
import type { ProgressData } from '../../lib/progress-data';
import { type EChartsOption, ReactEChartsCore, echarts } from '../../shared/echarts';
import { CHART } from '../../shared/theme';

export interface Series {
  name: string;
  color: string;
  components: number[];
  /** the series' functions or code bytes today: the denominator */
  total: number;
}

const esc = (s: string): string => s.replace(/[&<>"]/g, (c) => `&#${c.charCodeAt(0)};`);

export function HistoryChart({ data, series, metric }: { data: ProgressData; series: Series[]; metric: Metric }) {
  const points = data.history;
  const single = series.length === 1;
  const option: EChartsOption = {
    backgroundColor: 'transparent',
    animationDuration: 600,
    grid: { left: 8, right: 16, top: 40, bottom: 8, containLabel: true },
    legend: {
      top: 0,
      left: 0,
      type: 'scroll',
      textStyle: { color: CHART.axisLabel },
      pageTextStyle: { color: CHART.axisLabel },
      icon: 'roundRect',
      itemWidth: 12,
      itemHeight: 4,
    },
    tooltip: {
      trigger: 'axis',
      backgroundColor: CHART.tooltipBg,
      borderColor: CHART.tooltipBorder,
      textStyle: { color: CHART.tooltipText, fontSize: 12 },
      extraCssText: 'max-width: 420px; white-space: normal;',
      formatter: (params: unknown) => {
        const ps = params as { dataIndex: number; seriesIndex: number; color: string }[];
        if (ps.length === 0) {
          return '';
        }
        const p = points[ps[0].dataIndex];
        const head = p.commit
          ? `<div style="font-family:ui-monospace,monospace;color:${CHART.tooltipMuted}">${p.commit} · ${p.date.slice(0, 16).replace('T', ' ')} UTC</div><div style="margin:2px 0 6px;color:${CHART.tooltipText}">${esc(p.subject ?? '')}</div>`
          : `<div style="font-family:ui-monospace,monospace;color:${CHART.tooltipMuted}">${p.date.slice(0, 16).replace('T', ' ')} UTC</div><div style="margin:2px 0 6px;color:${CHART.tooltipMuted}">not in the history of main</div>`;
        const rows = ps.map((q) => {
          const s = series[q.seriesIndex];
          const c = matchedAt(p, s.components, metric);
          return `<div style="display:flex;justify-content:space-between;gap:16px"><span><span style="display:inline-block;width:8px;height:8px;border-radius:2px;margin-right:6px;background:${q.color}"></span>${esc(s.name)}</span><span style="font-family:ui-monospace,monospace">${num(c)} / ${num(s.total)} · ${((100 * c) / s.total).toFixed(2)}%</span></div>`;
        });
        return head + rows.join('');
      },
    },
    xAxis: {
      type: 'time',
      axisLabel: { color: CHART.axisLabel, hideOverlap: true },
      axisLine: { lineStyle: { color: CHART.grid } },
      splitLine: { show: false },
    },
    yAxis: {
      type: 'value',
      scale: true,
      axisLabel: { color: CHART.axisLabel, formatter: (v: number) => `${v.toFixed(v < 10 ? 1 : 0)}%` },
      splitLine: { lineStyle: { color: CHART.grid, opacity: 0.5 } },
    },
    series: series.map((s) => ({
      type: 'line',
      name: s.name,
      step: 'end',
      showSymbol: true,
      symbolSize: 6,
      lineStyle: { width: 2.5, color: s.color },
      itemStyle: { color: s.color },
      emphasis: { focus: 'series' },
      areaStyle: single
        ? {
            color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
              { offset: 0, color: `${s.color}55` },
              { offset: 1, color: `${s.color}00` },
            ]),
          }
        : undefined,
      data: points.map((p) => [p.date, (100 * matchedAt(p, s.components, metric)) / s.total]),
    })),
  };
  return (
    <ReactEChartsCore
      echarts={echarts}
      option={option}
      notMerge
      style={{ height: 320, width: '100%' }}
      aria-label="matches over time"
    />
  );
}
