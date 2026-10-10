import { defineConfig } from 'vitest/config';

// The suites read the real target and the project's build outputs from the urna checkout around this
// directory, and some compile through the pinned emsdk image; they run one file at a time so those
// compiles and the large module parses do not contend.
export default defineConfig({
  test: {
    environment: 'node',
    include: ['test/**/*.test.ts'],
    fileParallelism: false,
    testTimeout: 120_000,
    hookTimeout: 120_000,
  },
});
