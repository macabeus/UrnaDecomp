// Every results file under benchmark/results/ at build time, loaded on first use.
import type { BenchResults } from '@asmlift/wasm-benchmark/schema';
import { useEffect, useState } from 'react';

import { sortRuns } from '../../lib/bench';

const files = import.meta.glob<BenchResults>('../../../../benchmark/results/*.json', { import: 'default' });

let runs: Promise<BenchResults[]> | null = null;

export function loadRuns(): Promise<BenchResults[]> {
  runs ??= Promise.all(Object.values(files).map((load) => load())).then(sortRuns);
  return runs;
}

/** null while loading; newest first. */
export function useRuns(): BenchResults[] | null {
  const [value, setValue] = useState<BenchResults[] | null>(null);
  useEffect(() => {
    let live = true;
    void loadRuns().then((r) => live && setValue(r));
    return () => {
      live = false;
    };
  }, []);
  return value;
}

/** The run a `run` URL value names, else the newest. */
export function pickRun(all: readonly BenchResults[], label: string | null): BenchResults | null {
  return all.find((r) => r.label === label) ?? all[0] ?? null;
}
