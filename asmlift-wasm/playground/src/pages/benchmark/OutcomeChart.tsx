import type { BenchResults } from '@asmlift/wasm-benchmark/schema';

import { type SetupSummary, failures } from '../../lib/bench';
import { type EChartsOption, ReactEChartsCore, echarts } from '../../shared/echarts';
import { CHART, DIFFERS, EXACT, FAILED } from '../../shared/theme';

/** Rows per setup split by outcome: exact at -Oz, differs, failed (lift, compile, extract or link). */
export function OutcomeChart({
  run,
  setups,
  onSetup,
}: {
  run: BenchResults;
  setups: SetupSummary[];
  onSetup: (key: string) => void;
}) {
  const keys = [...setups].reverse();
  const series = [
    { name: 'exact at -Oz', color: EXACT, value: (s: SetupSummary) => s.totals.exact },
    { name: 'differs', color: DIFFERS, value: (s: SetupSummary) => s.totals.outcomes.differs ?? 0 },
    { name: 'failed', color: FAILED, value: (s: SetupSummary) => failures(s.totals) },
  ];
  const option: EChartsOption = {
    backgroundColor: 'transparent',
    animation: false,
    grid: { left: 8, right: 24, top: 36, bottom: 8, containLabel: true },
    legend: {
      top: 0,
      left: 0,
      textStyle: { color: CHART.axisLabel },
      icon: 'roundRect',
      itemWidth: 12,
      itemHeight: 12,
    },
    tooltip: {
      trigger: 'axis',
      axisPointer: { type: 'shadow' },
      backgroundColor: CHART.tooltipBg,
      borderColor: CHART.tooltipBorder,
      textStyle: { color: CHART.tooltipText, fontSize: 12 },
    },
    xAxis: {
      type: 'value',
      minInterval: 1,
      axisLabel: { color: CHART.axisLabel },
      splitLine: { lineStyle: { color: CHART.grid, opacity: 0.5 } },
    },
    yAxis: {
      type: 'category',
      data: keys.map((s) => s.key),
      axisLabel: { color: CHART.axisLabel, fontFamily: 'ui-monospace, monospace' },
      axisLine: { lineStyle: { color: CHART.grid } },
      axisTick: { show: false },
    },
    series: series.map((s) => ({
      type: 'bar',
      name: s.name,
      stack: 'rows',
      barWidth: 14,
      // a 2px surface gap between segments: green and gold are told apart by more than hue
      itemStyle: { color: s.color, borderColor: CHART.tooltipBg, borderWidth: 2, borderRadius: 2 },
      emphasis: { focus: 'series' },
      data: keys.map(s.value),
    })),
  };
  return (
    <ReactEChartsCore
      echarts={echarts}
      option={option}
      notMerge
      style={{ height: Math.max(260, 28 * keys.length + 60), width: '100%' }}
      onEvents={{ click: (p: { name?: string }) => p.name && onSetup(p.name) }}
      aria-label={`rows per setup by outcome, run ${run.label}`}
    />
  );
}
