import tailwindcss from '@tailwindcss/vite';
import react from '@vitejs/plugin-react';
import { defineConfig } from 'vite';

export default defineConfig({
  base: process.env.VITE_BASE_URL ?? './',
  plugins: [react(), tailwindcss()],
  // @asmlift/wasm and the benchmark are workspace TypeScript sources: serve them as they are
  optimizeDeps: { exclude: ['@asmlift/wasm', '@asmlift/wasm-benchmark'] },
  build: { target: 'es2022', chunkSizeWarningLimit: 1500 },
  // the toolkit's sources and the benchmark's results live outside this package
  server: { fs: { allow: ['..'] } },
});
