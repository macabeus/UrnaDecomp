// The app's chart and canvas colours, from the Brazilian flag (verde #009C3B, amarelo #FFDF00, azul #002776,
// branco) and the scales index.css redefines. The fills of squares and bars are the flag's green, a deep gold and a
// light blue rather than the flag's own yellow and white: on the navy surfaces those two are far brighter than the
// rest and have no hue to tell apart, and green next to a bright yellow collapses for red-green colour-blind readers.
// The three were checked with the dataviz skill's validate_palette.js against #0c1d47 (dark mode, all pairs, as
// waffle squares touch on every side): lightness band, chroma and contrast pass, normal vision ΔE 19.9, and CVD
// ΔE 6.9, in the floor band that is legal only with a secondary encoding, which the 1–3 px gaps between squares and
// bar segments and the counts beside every legend provide. Outcome colours are shared by the benchmark chart, its
// tables and the playground's row card: green for exact, gold for differs, blue for failed.
import type { Outcome } from '@asmlift/wasm-benchmark/schema';

export const EXACT = '#1fae55';
export const DIFFERS = '#ad720d';
export const FAILED = '#4886fe';

export function outcomeColor(o: Outcome): string {
  return o === 'exact' ? EXACT : o === 'differs' ? DIFFERS : FAILED;
}

export const CHART = {
  grid: '#2c3f6c',
  axisLabel: '#8193c4',
  tooltipBg: '#0c1d47',
  tooltipBorder: '#2c3f6c',
  tooltipText: '#e8edf8',
  /** secondary text inside a tooltip */
  tooltipMuted: '#8193c4',
} as const;

export const EXACT_TINT = '#7ee2a2';

/** The outcome's colour for text on a pill: a lighter step of the same hue, readable on the surfaces. */
export function outcomeTint(o: Outcome): string {
  return o === 'exact' ? EXACT_TINT : o === 'differs' ? '#e8b25a' : '#9dbcff';
}

/** The progress page's verdict colours, in VERDICTS order: matched, differs and unresolved as the outcomes above,
 *  and no candidate, the "not yet" state, an empty slot a step above the surfaces rather than a fourth hue. */
export const VERDICT_COLORS = [EXACT, DIFFERS, FAILED, '#22366d'] as const;
export const VERDICT_TINTS = [EXACT_TINT, '#e8b25a', '#9dbcff', '#aab8dc'] as const;
export const VERDICT_LABELS = ['matched', 'differs', 'unresolved', 'no candidate'] as const;

/** Line colours for the progress history: the total in white, then one per series, the flag's yellow, green and
 *  blue first (TSE code, upstream libraries, not attributed). */
export const SERIES_TOTAL = '#ffffff';
export const SERIES_COLORS = [
  '#ffdf00',
  '#22b95c',
  '#5b8def',
  '#f472b6',
  '#a3e635',
  '#fb923c',
  '#c084fc',
  '#f87171',
  '#2dd4bf',
  '#fff3a3',
  '#93deae',
  '#8fb0f5',
] as const;
/** The selected square's ring and the hovered one's on the function waffle. */
export const RING_SELECTED = '#ffdf00';
export const RING_HOVER = '#ffffff';
/** The function waffle's group labels. */
export const WAFFLE_LABEL = '#cfd8ee';
