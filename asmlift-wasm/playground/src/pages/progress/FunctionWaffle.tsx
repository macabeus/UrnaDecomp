// One square per target function, coloured by its verdict, drawn on a canvas (the whole module is 14,343
// squares). Each group starts on a new row under its label. Hover names a function; click selects it.
import { type MouseEvent, useCallback, useEffect, useMemo, useRef, useState } from 'react';

import { type Fn, num } from '../../lib/progress';
import { VERDICTS } from '../../lib/progress-data';
import { RING_HOVER, RING_SELECTED, VERDICT_COLORS, VERDICT_LABELS, VERDICT_TINTS, WAFFLE_LABEL } from '../../shared/theme';

export interface FnGroup {
  key: string;
  label: string;
  fns: Fn[];
}

interface Layout {
  cols: number;
  height: number;
  /** per group: the y of its first row of squares */
  top: number[];
}

const LABEL_H = 22;
const GROUP_GAP = 12;

function layout(groups: readonly FnGroup[], width: number, cell: number, gap: number, labels: boolean): Layout {
  const cols = Math.max(1, Math.floor((width + gap) / (cell + gap)));
  const top: number[] = [];
  let y = 0;
  groups.forEach((g, i) => {
    if (i > 0) {
      y += GROUP_GAP;
    }
    if (labels) {
      y += LABEL_H;
    }
    top.push(y);
    y += Math.ceil(g.fns.length / cols) * (cell + gap);
  });
  return { cols, height: Math.max(y - gap, cell), top };
}

const VERDICT_ID = new Map(VERDICTS.map((v, i) => [v, i]));

export function FunctionWaffle({
  groups,
  cell,
  gap = 2,
  selected,
  onSelect,
  labels = true,
}: {
  groups: FnGroup[];
  cell: number;
  gap?: number;
  selected: number | null;
  onSelect: (fn: Fn) => void;
  labels?: boolean;
}) {
  const box = useRef<HTMLDivElement>(null);
  const base = useRef<HTMLCanvasElement>(null);
  const overlay = useRef<HTMLCanvasElement>(null);
  const [width, setWidth] = useState(0);
  const [hover, setHover] = useState<{ fn: Fn; x: number; y: number } | null>(null);

  useEffect(() => {
    const el = box.current;
    if (el === null) {
      return;
    }
    const ro = new ResizeObserver(([e]) => setWidth(Math.floor(e.contentRect.width)));
    ro.observe(el);
    return () => ro.disconnect();
  }, []);

  const lay = useMemo(() => layout(groups, width, cell, gap, labels), [groups, width, cell, gap, labels]);
  const where = useMemo(() => {
    const m = new Map<number, [number, number]>();
    groups.forEach((g, gi) => {
      g.fns.forEach((f, k) => {
        m.set(f.index, [(k % lay.cols) * (cell + gap), lay.top[gi] + Math.floor(k / lay.cols) * (cell + gap)]);
      });
    });
    return m;
  }, [groups, lay, cell, gap]);

  useEffect(() => {
    const c = base.current;
    if (c === null || width === 0) {
      return;
    }
    const dpr = window.devicePixelRatio || 1;
    for (const canvas of [c, overlay.current!]) {
      canvas.width = width * dpr;
      canvas.height = lay.height * dpr;
      canvas.style.width = `${width}px`;
      canvas.style.height = `${lay.height}px`;
    }
    const ctx = c.getContext('2d')!;
    ctx.scale(dpr, dpr);
    ctx.clearRect(0, 0, width, lay.height);
    const r = Math.min(2, cell / 4);
    groups.forEach((g, gi) => {
      if (labels) {
        ctx.font = '600 12px ui-sans-serif, system-ui, sans-serif';
        ctx.fillStyle = WAFFLE_LABEL;
        ctx.textBaseline = 'alphabetic';
        ctx.fillText(g.label, 0, lay.top[gi] - 8);
      }
      for (const f of g.fns) {
        const [x, y] = where.get(f.index)!;
        ctx.fillStyle = VERDICT_COLORS[VERDICT_ID.get(f.verdict)!];
        ctx.beginPath();
        ctx.roundRect(x, y, cell, cell, r);
        ctx.fill();
      }
    });
  }, [groups, lay, where, width, cell, labels]);

  useEffect(() => {
    const c = overlay.current;
    if (c === null || width === 0) {
      return;
    }
    const dpr = window.devicePixelRatio || 1;
    const ctx = c.getContext('2d')!;
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    ctx.clearRect(0, 0, width, lay.height);
    const ring = (index: number, color: string, w: number) => {
      const p = where.get(index);
      if (p === undefined) {
        return;
      }
      ctx.strokeStyle = color;
      ctx.lineWidth = w;
      ctx.beginPath();
      ctx.roundRect(p[0] - w / 2 - 0.5, p[1] - w / 2 - 0.5, cell + w + 1, cell + w + 1, 2);
      ctx.stroke();
    };
    if (selected !== null) {
      ring(selected, RING_SELECTED, 2);
    }
    if (hover !== null) {
      ring(hover.fn.index, RING_HOVER, 1.5);
    }
  }, [selected, hover, where, width, lay, cell]);

  const hit = useCallback(
    (x: number, y: number): Fn | null => {
      for (let gi = groups.length - 1; gi >= 0; gi--) {
        if (y < lay.top[gi]) {
          continue;
        }
        const col = Math.floor(x / (cell + gap));
        const row = Math.floor((y - lay.top[gi]) / (cell + gap));
        if (col >= lay.cols || x - col * (cell + gap) > cell || y - lay.top[gi] - row * (cell + gap) > cell) {
          return null;
        }
        return groups[gi].fns[row * lay.cols + col] ?? null;
      }
      return null;
    },
    [groups, lay, cell, gap],
  );

  const onMove = (e: MouseEvent<HTMLCanvasElement>) => {
    const rect = e.currentTarget.getBoundingClientRect();
    const fn = hit(e.clientX - rect.left, e.clientY - rect.top);
    setHover(fn === null ? null : { fn, x: e.clientX - rect.left, y: e.clientY - rect.top });
  };

  return (
    <div ref={box} className="relative w-full" style={{ height: lay.height }}>
      <canvas ref={base} className="absolute inset-0" aria-hidden />
      <canvas
        ref={overlay}
        className={`absolute inset-0 ${hover ? 'cursor-pointer' : ''}`}
        role="img"
        aria-label={`${num(groups.reduce((a, g) => a + g.fns.length, 0))} functions, one square each, coloured by verdict`}
        onMouseMove={onMove}
        onMouseLeave={() => setHover(null)}
        onClick={(e) => {
          const rect = e.currentTarget.getBoundingClientRect();
          const fn = hit(e.clientX - rect.left, e.clientY - rect.top);
          if (fn !== null) {
            onSelect(fn);
          }
        }}
      />
      {hover !== null && <Tooltip {...hover} width={width} />}
    </div>
  );
}

function Tooltip({ fn, x, y, width }: { fn: Fn; x: number; y: number; width: number }) {
  const v = VERDICT_ID.get(fn.verdict)!;
  const left = Math.min(Math.max(0, x + 14), Math.max(0, width - 320));
  return (
    <div
      className="pointer-events-none absolute z-20 w-[320px] rounded-lg border border-slate-700 bg-slate-900/95 p-3 text-xs shadow-2xl backdrop-blur"
      style={{ left, top: y + 16 }}
    >
      <div className="flex items-center justify-between gap-2">
        <span className="font-mono text-slate-400">f{fn.index}</span>
        <span className="flex items-center gap-1.5 font-medium" style={{ color: VERDICT_TINTS[v] }}>
          <span className="h-2 w-2 rounded-sm" style={{ backgroundColor: VERDICT_COLORS[v] }} />
          {VERDICT_LABELS[v]}
        </span>
      </div>
      <div className="mt-1.5 line-clamp-3 break-all font-mono text-[11.5px] text-slate-100">{fn.name || 'no name yet'}</div>
      <div className="mt-1.5 text-slate-400">
        {fn.component} · {num(fn.size)} B
      </div>
    </div>
  );
}
