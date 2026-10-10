import { useMemo, useState } from 'react';

import { rowsBySetup } from '../../lib/bench';
import { type Target, displayName, lastIndex, searchFunctions } from '../../lib/target';
import { DATASET_ROWS, SETUPS } from '../../shared/data/dataset';

const GROUPS = rowsBySetup(DATASET_ROWS, SETUPS);

export function FunctionPicker({ t, index, onPick }: { t: Target; index: number; onPick: (index: number) => void }) {
  const [query, setQuery] = useState('');
  const [open, setOpen] = useState(false);
  const [cursor, setCursor] = useState(0);
  const hits = useMemo(() => searchFunctions(t, query), [t, query]);
  const first = t.m.importedFuncCount;
  const last = lastIndex(t);
  const isRow = DATASET_ROWS.some((r) => r.index === index);

  const pick = (i: number) => {
    onPick(i);
    setOpen(false);
    setQuery('');
    setCursor(0);
  };

  return (
    <div className="flex flex-wrap items-end gap-3 text-sm">
      <label className="relative flex min-w-[280px] flex-1 flex-col gap-1">
        <span className="text-xs uppercase tracking-wide text-slate-500">Function: index or name</span>
        <input
          value={query}
          onChange={(e) => {
            setQuery(e.target.value);
            setOpen(true);
            setCursor(0);
          }}
          onFocus={() => setOpen(true)}
          onBlur={() => setTimeout(() => setOpen(false), 150)}
          onKeyDown={(e) => {
            if (e.key === 'ArrowDown') {
              e.preventDefault();
              setCursor((c) => Math.min(c + 1, hits.length - 1));
            } else if (e.key === 'ArrowUp') {
              e.preventDefault();
              setCursor((c) => Math.max(c - 1, 0));
            } else if (e.key === 'Enter' && hits[cursor]) {
              pick(hits[cursor].index);
            } else if (e.key === 'Escape') {
              setOpen(false);
            }
          }}
          placeholder={`${first}–${last}, or part of a name: fopen, sqlite3_step, votaInit, CPosixSemaphore…`}
          className="rounded-md border border-slate-700 bg-slate-900 px-2 py-1.5 font-mono text-xs placeholder:text-slate-600"
          aria-label="search functions"
        />
        {open && query.trim() !== '' && (
          <ul className="scroll-slim absolute top-full z-20 mt-1 max-h-80 w-full overflow-auto rounded-md border border-slate-700 bg-slate-900 py-1 shadow-xl">
            {hits.length === 0 && <li className="px-3 py-1.5 text-xs text-slate-500">no defined function matches</li>}
            {hits.map((h, k) => (
              <li key={h.index}>
                <button
                  type="button"
                  onMouseDown={(e) => e.preventDefault()}
                  onClick={() => pick(h.index)}
                  className={`flex w-full gap-3 px-3 py-1 text-left font-mono text-xs ${
                    k === cursor ? 'bg-teal-500/15 text-teal-200' : 'text-slate-300 hover:bg-slate-800'
                  }`}
                >
                  <span className="w-12 shrink-0 text-right text-slate-500">{h.index}</span>
                  <span className="truncate">{h.name}</span>
                </button>
              </li>
            ))}
          </ul>
        )}
      </label>
      <label className="flex min-w-[260px] flex-1 flex-col gap-1">
        <span className="text-xs uppercase tracking-wide text-slate-500">Benchmark rows ({DATASET_ROWS.length}) by compile setup</span>
        <select
          value={isRow ? index : ''}
          onChange={(e) => e.target.value !== '' && pick(Number(e.target.value))}
          className="rounded-md border border-slate-700 bg-slate-900 px-2 py-1.5 text-xs"
          aria-label="benchmark rows"
        >
          <option value="">{isRow ? '' : 'not a benchmark row; pick one…'}</option>
          {GROUPS.map((g) => (
            <optgroup key={g.setup.key} label={`${g.setup.key}: ${g.setup.title}`}>
              {g.rows.map((r) => (
                <option key={r.id} value={r.index}>
                  {r.index} · {r.name ?? displayName(t, r.index)} · {r.size} B
                </option>
              ))}
            </optgroup>
          ))}
        </select>
      </label>
      <div className="flex gap-1">
        <button
          type="button"
          disabled={index <= first}
          onClick={() => pick(index - 1)}
          className="rounded-md border border-slate-700 px-2.5 py-1.5 text-xs text-slate-300 hover:bg-slate-800 disabled:opacity-40"
          title="previous function index"
        >
          ← {index - 1}
        </button>
        <button
          type="button"
          disabled={index >= last}
          onClick={() => pick(index + 1)}
          className="rounded-md border border-slate-700 px-2.5 py-1.5 text-xs text-slate-300 hover:bg-slate-800 disabled:opacity-40"
          title="next function index"
        >
          {index + 1} →
        </button>
      </div>
    </div>
  );
}
