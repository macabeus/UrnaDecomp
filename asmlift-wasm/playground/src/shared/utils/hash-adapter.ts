// The React glue of the fragment transport — the only part that touches `location` and `history`;
// the pure helpers are in hash-params.ts. As a nuqs adapter it changes nothing above it: nuqs hands
// an adapter a `URLSearchParams` and takes one back, saying nothing about where they live, so every
// parser, `useQueryStates`, `urlKeys` and `history: 'push'` keeps working. It is nuqs's own
// `adapters/react` minus what a Vite SPA has no use for: RSC, SSR, provider props, full-page
// navigation, history monkey-patching.
import { type unstable_AdapterOptions, unstable_createAdapterProvider } from 'nuqs/adapters/custom';
import { useState, useSyncExternalStore } from 'react';

import { createSnapshotCache, hashToSearchParams, hashUrl } from './hash-params';

/** `pushState` fires NEITHER `popstate` NOR `hashchange`, so this adapter's own writes are
 *  invisible to the DOM; this set is the channel that carries them, as nuqs's own emitter does. */
const listeners = new Set<() => void>();

/** Module-level, therefore stable, so `useSyncExternalStore` never resubscribes. Three sources,
 *  three disjoint causes: our own writes, Back/Forward, and a hand-edited address bar.
 *
 *  A `pushState` from code OUTSIDE this adapter stays invisible until the next event — nuqs's own
 *  React adapter behaves identically, which is what its opt-in `enableHistorySync` is for. Nothing
 *  in this app writes history out of band (`updateUrl` is the only writer), and the next write
 *  re-syncs rather than clobbers, because
 *  `getSearchParamsSnapshot` re-reads the live hash. */
function subscribe(onStoreChange: () => void): () => void {
  listeners.add(onStoreChange);
  window.addEventListener('popstate', onStoreChange);
  window.addEventListener('hashchange', onStoreChange);
  return () => {
    listeners.delete(onStoreChange);
    window.removeEventListener('popstate', onStoreChange);
    window.removeEventListener('hashchange', onStoreChange);
  };
}

/** nuqs's own adapter also aborts its write queue on `popstate` (`QueueReset` calling
 *  `resetQueues`), which no entry in nuqs's export map reaches. Without it, a Back pressed while an
 *  update is still queued — at most `defaultRateLimit.timeMs`, 50 ms outside Safari — lands that
 *  update on the entry popped to, taking the forward stack with it if the queued write was a
 *  push. */
function updateUrl(search: URLSearchParams, options: Required<unstable_AdapterOptions>): void {
  const url = hashUrl(window.location.href, search);
  (options.history === 'push' ? history.pushState : history.replaceState).call(history, history.state, '', url);
  for (const listener of listeners) {
    listener();
  }
  if (options.scroll) {
    window.scrollTo({ top: 0 });
  }
}

/** The BASE every write is merged onto. nuqs's default reads `location.search`, which is empty
 *  under this transport — so without this, a write of one key drops every OTHER key from the URL.
 *
 *  `dist/index.js:533` is the one place 2.10.1 still reads `location.search` behind an adapter's
 *  back. It is inert here because its guard `onCommittedPathname` (`:521`) compares the committed
 *  pathname against `adapter.pathname ?? location.pathname`, which under fragment routing never
 *  changes. Re-check on an upgrade. */
function getSearchParamsSnapshot(): URLSearchParams {
  return hashToSearchParams(window.location.hash);
}

function useHashAdapter(watchKeys: string[]) {
  // `useState`'s lazy initialiser, not `useRef(createSnapshotCache())`: a ref's argument is built
  // on every render and thrown away, and one cache cell per mounted adapter is the point.
  const [snapshot] = useState(createSnapshotCache);
  return {
    searchParams: useSyncExternalStore(subscribe, () => snapshot(window.location.hash, watchKeys)),
    updateUrl,
    getSearchParamsSnapshot,
  };
}

export const NuqsHashAdapter = unstable_createAdapterProvider(useHashAdapter);
