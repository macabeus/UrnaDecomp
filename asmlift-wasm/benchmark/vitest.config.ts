import { defineConfig } from 'vitest/config';

// The suites read the real target, the ledger and the build's objects from the urna checkout around
// this package, and some run through the pinned image; one file at a time so those do not contend.
export default defineConfig({
  test: {
    environment: 'node',
    include: ['test/**/*.test.ts'],
    fileParallelism: false,
    testTimeout: 600_000,
    hookTimeout: 600_000,
  },
});
