// The small rounded label: outcome badges, gap sizes, evidence kinds, feature tags, symbol names.
//
// TINTED — a `tint` hex drives the text color and a wash of it behind. The color stays the
// caller's, since what it MEANS belongs to the caller's scale, not here. NEUTRAL — no tint, slate
// on slate, for labels that identify rather than classify.
import type { ComponentPropsWithoutRef } from 'react';

export interface PillProps extends ComponentPropsWithoutRef<'span'> {
  /** hex color giving the text its color and the background a wash of it; omit for neutral slate */
  tint?: string;
  /** leading dot in the tint color — reads as a status marker rather than a label */
  dot?: boolean;
  mono?: boolean;
  /** `sm` for prose-sized badges, `xs` for the dense tags that sit inside table cells */
  size?: 'sm' | 'xs';
}

export function Pill({
  tint,
  dot = false,
  mono = false,
  size = 'sm',
  className = '',
  style,
  children,
  ...rest
}: PillProps) {
  const metrics = size === 'sm' ? 'gap-1.5 rounded-md px-2 py-0.5 text-xs' : 'gap-1 rounded px-1.5 py-0.5 text-[11px]';
  const palette = tint ? 'font-medium' : 'bg-slate-700/60 text-slate-300';
  return (
    <span
      className={`inline-flex items-center ${metrics} ${palette} ${mono ? 'font-mono' : ''} ${className}`}
      // `22` is ~13% alpha — enough to separate the pill from the surface, not enough to compete
      // with the text on it
      style={tint ? { backgroundColor: `${tint}22`, color: tint, ...style } : style}
      {...rest}
    >
      {dot && tint && <span className="h-1.5 w-1.5 shrink-0 rounded-full" style={{ backgroundColor: tint }} />}
      {children}
    </span>
  );
}
