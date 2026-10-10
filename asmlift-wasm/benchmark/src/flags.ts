// Compile flags as the build spells them, and two derived forms: the effective set (what decides
// the code: last -O and -std win, PIC and exceptions are on or off) to compare spellings from
// different sources of truth, and the flags a lifted C++ source is compiled with.

/** Split a command line the way a POSIX shell would for plain words, '…' and "…". */
export function splitFlags(s: string): string[] {
  const out: string[] = [];
  let cur = '';
  let inWord = false;
  let quote: "'" | '"' | null = null;
  for (let i = 0; i < s.length; i++) {
    const c = s[i];
    if (quote !== null) {
      if (c === quote) {
        quote = null;
      } else if (c === '\\' && quote === '"' && i + 1 < s.length) {
        cur += s[++i];
      } else {
        cur += c;
      }
    } else if (c === "'" || c === '"') {
      quote = c;
      inWord = true;
    } else if (/\s/.test(c)) {
      if (inWord) {
        out.push(cur);
        cur = '';
        inWord = false;
      }
    } else {
      cur += c;
      inWord = true;
    }
  }
  if (inWord) {
    out.push(cur);
  }
  return out;
}

/** Flags that name places or diagnostics, never code: include paths, VFS overlays, path maps, warnings,
 *  debug info. */
function isNoise(t: string): boolean {
  return (
    t.startsWith('-I') ||
    t === '-ivfsoverlay' ||
    t.startsWith('-ffile-prefix-map=') ||
    t.startsWith('-fdebug-compilation-dir') ||
    (t.startsWith('-W') && !t.startsWith('-Wl,')) ||
    t === '-w' ||
    t === '-g' ||
    t === '-sSTRICT' ||
    t === '-c'
  );
}

/** The code-generation flags of a compile command's arguments: noise, `-o <file>` and input files dropped. */
export function codegenFlags(tokens: readonly string[]): string[] {
  const out: string[] = [];
  for (let i = 0; i < tokens.length; i++) {
    const t = tokens[i];
    if (t === '-o') {
      i++;
    } else if (t.startsWith('-') && !isNoise(t)) {
      out.push(t);
    }
  }
  return out;
}

/** A canonical spelling of what the flags decide: the last -O and -std, whether PIC and C++
 *  exceptions end up on, and every other flag once, sorted. Two flag strings that compile the same
 *  way spell the same here (`-fPIC … -fno-PIC` and `-fno-pic` both say non-PIC). */
export function effectiveFlags(flags: string | readonly string[]): string {
  const tokens = codegenFlags(typeof flags === 'string' ? splitFlags(flags) : flags);
  let opt = '-O0';
  let std: string | null = null;
  let pic = false;
  let exceptions = false;
  const rest = new Set<string>();
  for (const t of tokens) {
    if (/^-O/.test(t)) {
      opt = t;
    } else if (t.startsWith('-std=')) {
      std = t;
    } else if (t === '-fPIC' || t === '-fpic') {
      pic = true;
    } else if (t === '-fno-PIC' || t === '-fno-pic') {
      pic = false;
    } else if (t === '-fexceptions') {
      exceptions = true;
    } else if (t === '-fno-exceptions') {
      exceptions = false;
    } else {
      rest.add(t);
    }
  }
  return [opt, std, pic ? '-fPIC' : null, exceptions ? '-fexceptions' : null, ...[...rest].sort()]
    .filter((x) => x !== null)
    .join(' ');
}

/** The em++ flags a lifted source of a function built with `flags` compiles with: the lift is C++
 *  that names no macro, so defines go, and so does a C `-std`. */
export function liftCompileFlags(flags: string): string {
  return codegenFlags(splitFlags(flags))
    .filter((t) => !/^-[DU]/.test(t) && !/^-std=(c|gnu)\d/.test(t))
    .join(' ');
}
