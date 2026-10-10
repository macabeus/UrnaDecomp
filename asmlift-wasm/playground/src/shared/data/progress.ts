// The Progress page's data, fetched once from data/progress.json (scripts/progress.ts puts it there).
import { useEffect, useState } from 'react';

import type { ProgressData } from '../../lib/progress-data';
import type { Loaded } from './target';

let progress: Promise<ProgressData> | null = null;

function loadProgress(): Promise<ProgressData> {
  progress ??= fetch(`${import.meta.env.BASE_URL}data/progress.json`).then((r) => {
    if (!r.ok) {
      throw new Error(`data/progress.json: HTTP ${r.status}. Run \`pnpm data\` in asmlift-wasm/playground to put it there.`);
    }
    return r.json() as Promise<ProgressData>;
  });
  return progress;
}

export function useProgress(): Loaded<ProgressData> {
  const [value, setValue] = useState<Loaded<ProgressData>>({ state: 'loading' });
  useEffect(() => {
    let live = true;
    loadProgress().then(
      (d) => live && setValue({ state: 'ready', value: d }),
      (e: unknown) => live && setValue({ state: 'error', error: e instanceof Error ? e.message : String(e) }),
    );
    return () => {
      live = false;
    };
  }, []);
  return value;
}
