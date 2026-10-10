import type { ReactNode } from 'react';

interface PanelProps {
  title?: ReactNode;
  subtitle?: ReactNode;
  /** right side of the title row */
  aside?: ReactNode;
  children: ReactNode;
  className?: string;
}

/** A translucent content panel. */
export function Panel({ title, subtitle, aside, children, className }: PanelProps) {
  return (
    <section className={`rounded-lg bg-slate-800/30 p-4 ${className ?? ''}`}>
      {(title || aside) && (
        <div className="mb-3 flex items-start justify-between gap-3">
          <div className="min-w-0">
            {title && <h3 className="text-sm font-semibold text-slate-200">{title}</h3>}
            {subtitle && <p className="mt-0.5 text-xs text-slate-400">{subtitle}</p>}
          </div>
          {aside}
        </div>
      )}
      {children}
    </section>
  );
}

/** A headline number tile. */
export function Stat({ label, value, hint }: { label: string; value: ReactNode; hint?: ReactNode }) {
  return (
    <div className="flex flex-col gap-1 rounded-xl bg-slate-800/60 p-4">
      <span className="text-xs uppercase tracking-wide text-slate-400">{label}</span>
      <span className="font-mono text-2xl font-bold text-slate-100">{value}</span>
      {hint && <span className="text-xs text-slate-500">{hint}</span>}
    </div>
  );
}

/** A `term  value` line of a facts list. */
export function Fact({ term, children }: { term: string; children: ReactNode }) {
  return (
    <>
      <dt className="text-xs uppercase tracking-wide text-slate-500">{term}</dt>
      <dd className="min-w-0 break-words text-sm text-slate-200">{children}</dd>
    </>
  );
}
