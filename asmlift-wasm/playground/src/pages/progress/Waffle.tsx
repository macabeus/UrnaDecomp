import { waffleCells } from '../../lib/progress';
import { VERDICT_COLORS } from '../../shared/theme';

/** A 10 × 10 waffle: 100 squares split by verdict in proportion to `values` (VERDICTS order), filled
 *  row by row from the top left, matched first. */
export function Waffle({ values, size = 120, label }: { values: readonly number[]; size?: number; label: string }) {
  const counts = waffleCells(values);
  const colors: string[] = [];
  counts.forEach((n, v) => {
    for (let k = 0; k < n; k++) {
      colors.push(VERDICT_COLORS[v]);
    }
  });
  const step = 10;
  return (
    <svg viewBox={`0 0 ${10 * step} ${10 * step}`} width={size} height={size} role="img" aria-label={label} className="shrink-0">
      {colors.map((c, i) => (
        <rect
          key={i}
          x={(i % 10) * step + 1}
          y={Math.floor(i / 10) * step + 1}
          width={step - 2}
          height={step - 2}
          rx={1.6}
          fill={c}
          style={{ transition: 'fill 300ms ease', transitionDelay: `${i * 4}ms` }}
        />
      ))}
    </svg>
  );
}
