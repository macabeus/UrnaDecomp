// The notice about TSE's files, under the header on every page, folded by default: what the project is, that the
// TSE's files are used unmodified, why it exists, and how a competent authority asks for it to be taken down. Modelled
// on UrnaEmu's notice and disclaimer (https://github.com/rlaneth/urnaemu), in English and Portuguese like UrnaEmu's.
import type { ReactNode } from 'react';

const ISSUES = 'https://github.com/macabeus/UrnaDecomp/issues';
const NEWS =
  'https://www.estadao.com.br/tecmundo/ciberseguranca/simulador-de-votacao-exibe-codigo-do-app-da-urna-eletronica-tse-afirma-intencao/';

function Out({ href, children }: { href: string; children: ReactNode }) {
  return (
    <a href={href} target="_blank" rel="noopener noreferrer" className="text-teal-300 underline-offset-2 hover:underline">
      {children}
    </a>
  );
}

const wasm = <span className="font-mono text-slate-200">vota_web_wasm.wasm</span>;

const POINTS: { lang: string; title: string; items: [string, ReactNode][]; news: ReactNode }[] = [
  {
    lang: 'en',
    title: 'Notice about the TSE files',
    items: [
      [
        'A matching decompilation.',
        <>
          UrnaDecomp rebuilds C++ source that compiles back, byte for byte, to {wasm}: VOTA, the voting application of the
          TSE's voting simulator. The target files belong to the TSE and are used and redistributed{' '}
          <strong className="text-slate-200">unmodified</strong>; this site loads that same module to read it.
        </>,
      ],
      [
        'Independent.',
        'The files are the ones the TSE publishes in its Voting Simulator. The project is not affiliated with or endorsed by the TSE or the Electoral Justice.',
      ],
      [
        'Why.',
        'To document, openly and verifiably, how VOTA works as the TSE publishes it in the simulator: anyone can rebuild the matched code and check it against the module, byte for byte.',
      ],
      [
        'No evidence of fraud.',
        "Nothing in this project is evidence of electoral fraud, and it cannot be: it studies the simulator's module, not the software sealed into the voting machines, and says nothing about the result of any election. Presenting this code as evidence of fraud misrepresents it; where the reconstruction and the module disagree, the reconstruction is wrong.",
      ],
      [
        'Takedown.',
        <>
          The TSE or another competent authority can ask for this project, or the TSE files in it, to be taken down by
          opening an issue on <Out href={ISSUES}>GitHub</Out>.
        </>,
      ],
    ],
    news: (
      <>
        In the news: <Out href={NEWS}>Estadão / TecMundo</Out> (in Portuguese). The TSE told the outlet that reusing the
        voting machine's C++ code in the WebAssembly simulator was a deliberate choice, that code reconstructed from it
        reveals no secrets, and that it does not intend to ask for its removal.
      </>
    ),
  },
  {
    lang: 'pt-BR',
    title: 'Aviso sobre os arquivos do TSE',
    items: [
      [
        'Uma decompilação por correspondência.',
        <>
          O UrnaDecomp reconstrói código-fonte C++ que, compilado, reproduz byte a byte o {wasm}: o VOTA, aplicativo de
          votação do Simulador de Votação do TSE. Os arquivos-alvo pertencem ao TSE e são usados e redistribuídos{' '}
          <strong className="text-slate-200">sem modificação</strong>; este site carrega esse mesmo módulo para lê-lo.
        </>,
      ],
      [
        'Independente.',
        'Os arquivos são os que o TSE publica em seu Simulador de Votação. O projeto não tem vínculo com o TSE ou a Justiça Eleitoral, nem endosso deles.',
      ],
      [
        'Por quê.',
        'Para documentar, de forma aberta e verificável, como funciona o VOTA tal como o TSE o publica no simulador: qualquer pessoa pode recompilar o código reconstruído e conferi-lo com o módulo, byte a byte.',
      ],
      [
        'Nenhuma evidência de fraude.',
        'Nada neste projeto é evidência de fraude eleitoral, nem pode ser: ele estuda o módulo do simulador, não o software lacrado nas urnas, e não diz nada sobre o resultado de nenhuma eleição. Apresentar este código como evidência de fraude é deturpá-lo; onde a reconstrução e o módulo divergem, o erro é da reconstrução.',
      ],
      [
        'Remoção.',
        <>
          O TSE ou outra autoridade competente pode pedir a retirada deste projeto, ou dos arquivos do TSE nele, abrindo
          uma issue no <Out href={ISSUES}>GitHub</Out>.
        </>,
      ],
    ],
    news: (
      <>
        Na imprensa: <Out href={NEWS}>Estadão / TecMundo</Out>. O TSE afirmou à reportagem que reutilizar o código C++
        da urna no simulador em WebAssembly foi uma escolha deliberada, que o código reconstruído a partir dele não
        revela segredos e que não pretende pedir sua remoção.
      </>
    ),
  },
];

export function Notice() {
  return (
    <details className="group mb-6 overflow-hidden rounded-2xl border border-amber-400/25 bg-amber-400/[0.06]">
      <summary className="flex cursor-pointer list-none items-center gap-3 px-5 py-3 text-sm marker:hidden hover:bg-amber-400/[0.05] [&::-webkit-details-marker]:hidden">
        <span aria-hidden className="text-amber-300">
          ⚠
        </span>
        <span className="font-semibold text-amber-200">Notice</span>
        <span className="min-w-0 flex-1 truncate text-slate-400">
          An independent matching decompilation of the TSE's unmodified files.
        </span>
        <span aria-hidden className="text-slate-500 transition-transform group-open:rotate-90">
          ›
        </span>
      </summary>
      <div className="grid gap-6 border-t border-amber-400/15 px-5 py-5 lg:grid-cols-2">
        {POINTS.map((p) => (
          <section key={p.lang} lang={p.lang}>
            <h2 className="text-sm font-semibold text-amber-200">{p.title}</h2>
            <ul className="mt-3 space-y-2.5 text-sm leading-relaxed text-slate-300">
              {p.items.map(([term, text]) => (
                <li key={term}>
                  <strong className="text-slate-100">{term}</strong> {text}
                </li>
              ))}
            </ul>
            <p className="mt-4 border-t border-slate-700/50 pt-3 text-xs leading-relaxed text-slate-400">{p.news}</p>
          </section>
        ))}
      </div>
    </details>
  );
}
