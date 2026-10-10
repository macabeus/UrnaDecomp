// The local commands that do what the page cannot: em++ does not run in a browser, so compiling and
// scoring a function's C++ happens in the checkout, through the toolkit's CLI and urna's try-tu.sh.
const TARGET_FROM_KIT = '../target/vota_web_wasm.wasm';
export const LIFT_DIR = '/tmp/lift';

export function shellQuote(s: string): string {
  return /^[\w@%+=:,./-]+$/.test(s) ? s : `'${s.replace(/'/g, `'\\''`)}'`;
}

export interface CommandBlock {
  title: string;
  /** where the commands run, relative to the urna checkout */
  cwd: string;
  lines: string[];
}

export interface CommandInput {
  index: number;
  /** the symbol the lifted source defines */
  symbol: string;
  /** the function's demangled name */
  name: string;
  /** its ledger component (`-` when unattributed) */
  component: string;
  /** the em++ flags the lift was made for */
  flags: string;
  /** the benchmark setup when the function is a benchmark row */
  setup: string | null;
}

/** The directory under urna's src/ a function of this ledger component lands in, which `try-tu.sh --component`
 *  takes to pick the compile rule (and so the flags) of a file outside src/. ecourna's directories follow its
 *  namespaces, and the directory matters there: `app/dados` and `api/compression` compile without -fPIC. Null for
 *  the libraries and unattributed functions, which try-tu.sh does not build. */
export function tryTuComponent(component: string, name: string): string | null {
  switch (component) {
    case 'ecourna': {
      const ns = /^ecourna((?:::[a-z][a-z0-9_]*)*)::/.exec(name);
      return ns === null ? 'ecourna' : `ecourna${ns[1].replaceAll('::', '/')}`;
    }
    case 'vota':
    case 'vota-web':
    case 'comum':
    case 'simulador':
    case 'api':
      return 'uenux2';
    case 'ASN1':
      return 'asn1';
    case 'asn1-modules':
      return 'asn1/modules';
    default:
      return null;
  }
}

export function localCommands(c: CommandInput): CommandBlock[] {
  const file = `${LIFT_DIR}/${c.index}.cpp`;
  const extract = shellQuote(`export:${c.symbol}`);
  const blocks: CommandBlock[] = [
    {
      title: 'Read and lift it',
      cwd: 'asmlift-wasm',
      lines: [
        `pnpm wasm fn ${TARGET_FROM_KIT} ${c.index}`,
        `pnpm wasm lift ${TARGET_FROM_KIT} ${c.index} -o ${LIFT_DIR} --check`,
      ],
    },
    {
      title: `Compile ${file} alone at its flags, link it at -Oz and score it against the target`,
      cwd: 'asmlift-wasm',
      lines: [`pnpm wasm rank ${TARGET_FROM_KIT} ${c.index} --extract ${extract} --flags ${shellQuote(c.flags)} ${file}`],
    },
  ];
  const dir = tryTuComponent(c.component, c.name);
  if (dir !== null) {
    blocks.push({
      title: `The same at the build's flags for a file in src/${dir}`,
      cwd: '.',
      lines: [`tools/round/try-tu.sh --single ${c.index} --extract ${extract} --component ${dir} ${file}`],
    });
  }
  blocks.push(
    c.setup === null
      ? {
          title: 'Add it to the benchmark (refused when no compile setup covers it)',
          cwd: 'asmlift-wasm',
          lines: [`pnpm bench add ${c.index}`],
        }
      : {
          title: `Rerun its setup in the benchmark and compare with the baseline`,
          cwd: 'asmlift-wasm',
          lines: [`pnpm bench run --label my-change --setup ${c.setup}`, 'pnpm bench diff baseline my-change'],
        },
  );
  return blocks;
}
