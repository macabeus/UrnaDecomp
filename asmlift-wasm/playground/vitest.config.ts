import { defineConfig } from 'vitest/config';

// The suites read the real target, glue, ledger and benchmark results from the urna checkout.
export default defineConfig({
  test: {
    environment: 'node',
    include: ['test/**/*.test.ts'],
    testTimeout: 120_000,
    hookTimeout: 120_000,
  },
});
