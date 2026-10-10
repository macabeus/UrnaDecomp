import { defineConfig } from 'vitest/config';

// The suites read the real target and the project's build outputs from the urna checkout around this
// directory, and some compile through the pinned emsdk image; they run one file at a time so those
// compiles and the large module parses do not contend. A normalisation of the whole target holds about a
// gigabyte, and the census and score suites build several, which a small machine's default heap does not fit.
export default defineConfig({
  test: {
    environment: 'node',
    include: ['test/**/*.test.ts'],
    fileParallelism: false,
    poolOptions: { forks: { execArgv: ['--max-old-space-size=4096'] } },
    testTimeout: 120_000,
    hookTimeout: 120_000,
  },
});
