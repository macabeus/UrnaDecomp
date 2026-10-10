// A link to UrnaEmu, the emulator that runs the module this project decompiles. Every link leaves the app, in a new
// tab; nothing here is fetched.

const URNAEMU = 'https://eleicoes.rlaneth.com/urnaemu/';
const TWITTER = 'https://twitter.com/rlaneth';
const GITHUB = 'https://github.com/rlaneth/urnaemu';

export function UrnaEmu() {
  return (
    <section className="relative overflow-hidden rounded-2xl border border-slate-700/50 bg-slate-800/40 p-6 shadow-xl sm:p-10">
      <div className="pointer-events-none absolute -right-24 -top-24 h-72 w-72 rounded-full bg-teal-500/10 blur-3xl" />
      <div className="pointer-events-none absolute -bottom-24 left-1/4 h-60 w-60 rounded-full bg-amber-400/10 blur-3xl" />
      <div className="relative max-w-3xl">
        <p className="text-xs font-semibold uppercase tracking-[0.2em] text-teal-300/80">A sister project</p>
        <h2 className="mt-3 text-4xl font-bold tracking-tight text-slate-100 sm:text-5xl">
          <span className="bg-gradient-to-r from-teal-300 via-amber-300 to-blue-400 bg-clip-text text-transparent">UrnaEmu</span>
        </h2>
        <p className="mt-4 text-base leading-relaxed text-slate-300">
          An emulator of the Brazilian electronic voting machine, by Rodrigo Laneth. It runs VOTA, the voting application of
          TSE's voting simulator, the same <span className="font-mono text-slate-200">vota_web_wasm.wasm</span> this project
          decompiles, in your browser, together with the equipment around it: the poll worker's terminal, the printer, the
          clock, the biometric reader and the signed result files. Where UrnaDecomp reads the module, UrnaEmu runs it.
        </p>
        <a
          href={URNAEMU}
          target="_blank"
          rel="noopener noreferrer"
          className="mt-7 inline-flex items-center gap-2 rounded-xl border border-teal-500/40 bg-teal-500/20 px-5 py-3 text-base font-semibold text-teal-100 shadow-lg shadow-teal-500/10 transition-colors hover:bg-teal-500/30"
        >
          Open UrnaEmu
          <span aria-hidden>↗</span>
        </a>
        <p className="mt-3 text-xs text-slate-500">It needs a Chromium-based browser.</p>
        <div className="mt-8 flex flex-wrap items-center gap-x-5 gap-y-2 text-sm text-slate-400">
          <a href={TWITTER} target="_blank" rel="noopener noreferrer" className="hover:text-teal-300">
            @rlaneth on Twitter ↗
          </a>
          <span className="text-slate-600" aria-hidden>
            ·
          </span>
          <a href={GITHUB} target="_blank" rel="noopener noreferrer" className="font-mono hover:text-teal-300">
            github.com/rlaneth/urnaemu ↗
          </a>
        </div>
      </div>
    </section>
  );
}
