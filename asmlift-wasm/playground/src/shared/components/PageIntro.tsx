import type { ReactNode } from 'react';

/** What a page is for, above it: a title, a lead paragraph and the page's parts, one line each. */
export function PageIntro({ title, lead, parts }: { title: string; lead: ReactNode; parts: [string, ReactNode][] }) {
  return (
    <section className="relative mb-5 overflow-hidden rounded-2xl border border-slate-700/50 bg-slate-800/40 p-5 sm:p-6">
      <div className="absolute inset-y-0 left-0 w-1 bg-gradient-to-b from-teal-500 via-amber-400 to-blue-500" />
      <h2 className="text-lg font-semibold text-slate-100">{title}</h2>
      <p className="mt-1.5 max-w-4xl text-sm leading-relaxed text-slate-300">{lead}</p>
      <dl className="mt-4 grid gap-3 sm:grid-cols-2 xl:grid-cols-4">
        {parts.map(([term, text]) => (
          <div key={term} className="rounded-xl border border-slate-700/40 bg-slate-900/40 p-3">
            <dt className="text-xs font-semibold uppercase tracking-wide text-teal-300">{term}</dt>
            <dd className="mt-1 text-xs leading-relaxed text-slate-400">{text}</dd>
          </div>
        ))}
      </dl>
    </section>
  );
}
