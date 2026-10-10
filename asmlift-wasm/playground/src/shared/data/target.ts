// The target module, its glue and the distilled ledger, fetched once from data/ (scripts/data.ts
// puts them there), and the direct-call graph, computed once after the module is open.
import { useEffect, useState } from 'react';

import type { LedgerData } from '../../lib/ledger-data';
import { type Target, directCallers, openTarget } from '../../lib/target';

async function fetchOk(name: string): Promise<Response> {
  const r = await fetch(`${import.meta.env.BASE_URL}data/${name}`);
  if (!r.ok) {
    throw new Error(`data/${name}: HTTP ${r.status}. Run \`pnpm data\` in asmlift-wasm/playground to put it there.`);
  }
  return r;
}

let target: Promise<Target> | null = null;
let callers: Promise<Map<number, number[]>> | null = null;

export function loadTarget(): Promise<Target> {
  target ??= Promise.all([
    fetchOk('vota_web_wasm.wasm').then((r) => r.arrayBuffer()),
    fetchOk('vota_web_wasm.js').then((r) => r.text()),
    fetchOk('ledger.json').then((r) => r.json() as Promise<LedgerData>),
  ]).then(([wasm, glue, data]) => openTarget(new Uint8Array(wasm), glue, data));
  return target;
}

function loadCallers(t: Target): Promise<Map<number, number[]>> {
  // a macrotask later, so the first function paints before the whole module is decoded
  callers ??= new Promise((resolve) => setTimeout(() => resolve(directCallers(t)), 0));
  return callers;
}

export type Loaded<T> = { state: 'loading' } | { state: 'error'; error: string } | { state: 'ready'; value: T };

export function useTarget(): Loaded<Target> {
  const [value, setValue] = useState<Loaded<Target>>({ state: 'loading' });
  useEffect(() => {
    let live = true;
    loadTarget().then(
      (t) => live && setValue({ state: 'ready', value: t }),
      (e: unknown) => live && setValue({ state: 'error', error: e instanceof Error ? e.message : String(e) }),
    );
    return () => {
      live = false;
    };
  }, []);
  return value;
}

export function useCallers(t: Target | null): Map<number, number[]> | null {
  const [value, setValue] = useState<Map<number, number[]> | null>(null);
  useEffect(() => {
    if (t === null) {
      return;
    }
    let live = true;
    void loadCallers(t).then((c) => live && setValue(c));
    return () => {
      live = false;
    };
  }, [t]);
  return value;
}
