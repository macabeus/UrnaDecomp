// Unpack the Emscripten file_packager payload referenced by the glue.
// usage: node tools/setup/unpack_fs.mjs <glue.js> <payload.data> <outdir> <manifest.tsv>
import { readFileSync, writeFileSync, mkdirSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { dirname, join, normalize } from 'node:path';

const [glue, dataPath, outDir, manifestPath] = process.argv.slice(2);
const js = readFileSync(glue, 'utf8');
const m = /loadPackage\((\{files:\[.*?\],remote_package_size:\d+\})\)/s.exec(js);
if (!m) throw new Error('metadata object not found');
const meta = new Function('return ' + m[1])(); // JS object literal, not JSON
const data = readFileSync(dataPath);
if (meta.remote_package_size !== data.length) throw new Error(`size mismatch ${meta.remote_package_size} vs ${data.length}`);
const rows = [];
for (const f of meta.files) {
  const rel = normalize(f.filename).replace(/^(\.\.(\/|\\|$))+/, '');
  const out = join(outDir, rel);
  mkdirSync(dirname(out), { recursive: true });
  const bytes = data.subarray(f.start, f.end);
  writeFileSync(out, bytes);
  rows.push([f.filename, f.end - f.start, createHash('sha256').update(bytes).digest('hex')].join('\t'));
}
writeFileSync(manifestPath, 'path\tsize\tsha256\n' + rows.join('\n') + '\n');
console.log(`files=${meta.files.length} bytes=${meta.files.reduce((a, f) => a + f.end - f.start, 0)} payload=${data.length}`);
