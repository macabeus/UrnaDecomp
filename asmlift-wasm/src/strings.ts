// The ledger's strings (strings.tsv rows) as a function's constants point at them. No file is read
// here, so a browser can bundle it; readout.ts reads strings.tsv.
import type { Instr } from './disasm';

export interface LedgerString {
  addr: number;
  len: number;
  enc: string;
  text: string;
}

/** The string a data address falls in, with the offset into it. */
export function stringAt(strings: readonly LedgerString[], addr: number): { s: LedgerString; offset: number } | null {
  let lo = 0;
  let hi = strings.length - 1;
  let found = -1;
  while (lo <= hi) {
    const mid = (lo + hi) >> 1;
    if (strings[mid].addr <= addr) {
      found = mid;
      lo = mid + 1;
    } else {
      hi = mid - 1;
    }
  }
  if (found < 0) {
    return null;
  }
  const s = strings[found];
  return addr < s.addr + s.len ? { s, offset: addr - s.addr } : null;
}

/** Every string an `i32.const` of the body points at (a folded memarg offset is a LOAD from the
 *  address, not a pointer to it, so it is not a string reference and is not listed). */
export function referencedStrings(
  instrs: readonly Instr[],
  strings: readonly LedgerString[],
): { offset: number; addr: number; s: LedgerString; into: number }[] {
  const out: { offset: number; addr: number; s: LedgerString; into: number }[] = [];
  const seen = new Set<number>();
  for (const i of instrs) {
    if (i.opcode !== 0x41) {
      continue;
    }
    const addr = (i.imms[0] as { value: number }).value >>> 0;
    if (addr < 1024 || seen.has(addr)) {
      continue;
    }
    const hit = stringAt(strings, addr);
    if (hit !== null) {
      seen.add(addr);
      out.push({ offset: i.offset, addr, s: hit.s, into: hit.offset });
    }
  }
  return out;
}
