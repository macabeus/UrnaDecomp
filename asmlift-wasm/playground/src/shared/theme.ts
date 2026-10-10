// Outcome colours, shared by the benchmark chart, its tables and the playground's row card. Checked
// with the dataviz palette validator against the chart surface (#1e293b): lightness band, chroma,
// colour-blind and normal-vision separation, contrast all pass.
import type { Outcome } from '@asmlift/wasm-benchmark/schema';

export const EXACT = '#0d9488';
export const DIFFERS = '#c0840b';
export const FAILED = '#8b5cf6';

export function outcomeColor(o: Outcome): string {
  return o === 'exact' ? EXACT : o === 'differs' ? DIFFERS : FAILED;
}

export const CHART = {
  grid: '#334155',
  axisLabel: '#94a3b8',
  tooltipBg: '#1e293b',
  tooltipBorder: '#334155',
  tooltipText: '#e2e8f0',
} as const;

/** The outcome's colour for text on a pill: a lighter step of the same hue, readable on slate. */
export function outcomeTint(o: Outcome): string {
  return o === 'exact' ? '#5eead4' : o === 'differs' ? '#fcd34d' : '#c4b5fd';
}
