// LEB128 readers over a byte array. Every reader returns the value and the position AFTER it, so a
// caller never has to know how many bytes the encoding took — which is the whole trap of LEB: a
// `i32.const 7` can legally be 1 or 5 bytes long (relocatable objects pad to 5), and the only way to
// know is to read it.

export class DecodeError extends Error {
  constructor(
    message: string,
    public readonly offset: number,
  ) {
    super(`${message} at byte offset 0x${offset.toString(16)}`);
  }
}

export interface Cursor {
  readonly bytes: Uint8Array;
  pos: number;
}

export function u8(c: Cursor): number {
  if (c.pos >= c.bytes.length) {
    throw new DecodeError('unexpected end of input', c.pos);
  }
  return c.bytes[c.pos++];
}

/** Unsigned LEB128, at most 5 bytes (u32). */
export function u32(c: Cursor): number {
  let result = 0;
  let shift = 0;
  const start = c.pos;
  for (;;) {
    const b = u8(c);
    result |= (b & 0x7f) << shift;
    if ((b & 0x80) === 0) {
      break;
    }
    shift += 7;
    if (shift > 28) {
      throw new DecodeError('u32 LEB longer than 5 bytes', start);
    }
  }
  // `|` made it signed; a 5-byte u32 whose top bit is set reads negative.
  return result >>> 0;
}

/** Signed LEB128 of up to 33 bits (s32 and the s33 block type share this). Returned as a JS number. */
export function s33(c: Cursor): number {
  let result = 0;
  let shift = 0;
  let b: number;
  const start = c.pos;
  do {
    b = u8(c);
    result |= (b & 0x7f) << shift;
    shift += 7;
    if (shift > 35) {
      throw new DecodeError('s33 LEB longer than 5 bytes', start);
    }
  } while (b & 0x80);
  if (shift < 32 && b & 0x40) {
    result |= -1 << shift;
  }
  return result;
}

/** Signed 32-bit LEB128 (the `i32.const` immediate). The result is the two's-complement i32. */
export function s32(c: Cursor): number {
  return s33(c) | 0;
}

/** Signed 64-bit LEB128 (the `i64.const` immediate), as a BigInt. */
export function s64(c: Cursor): bigint {
  let result = 0n;
  let shift = 0n;
  let b: number;
  const start = c.pos;
  do {
    b = u8(c);
    result |= BigInt(b & 0x7f) << shift;
    shift += 7n;
    if (shift > 70n) {
      throw new DecodeError('s64 LEB longer than 10 bytes', start);
    }
  } while (b & 0x80);
  if (shift < 64n && b & 0x40) {
    result |= -1n << shift;
  }
  return BigInt.asIntN(64, result);
}

export function f32(c: Cursor): number {
  if (c.pos + 4 > c.bytes.length) {
    throw new DecodeError('truncated f32', c.pos);
  }
  const v = new DataView(c.bytes.buffer, c.bytes.byteOffset + c.pos, 4).getFloat32(0, true);
  c.pos += 4;
  return v;
}

export function f64(c: Cursor): number {
  if (c.pos + 8 > c.bytes.length) {
    throw new DecodeError('truncated f64', c.pos);
  }
  const v = new DataView(c.bytes.buffer, c.bytes.byteOffset + c.pos, 8).getFloat64(0, true);
  c.pos += 8;
  return v;
}

/** The raw bit pattern of a float immediate, so two NaNs with different payloads stay different. */
export function f32bits(bytes: Uint8Array, at: number): number {
  return new DataView(bytes.buffer, bytes.byteOffset + at, 4).getUint32(0, true);
}

export function f64bits(bytes: Uint8Array, at: number): bigint {
  return new DataView(bytes.buffer, bytes.byteOffset + at, 8).getBigUint64(0, true);
}

export function name(c: Cursor): string {
  const len = u32(c);
  if (c.pos + len > c.bytes.length) {
    throw new DecodeError('truncated name', c.pos);
  }
  const s = new TextDecoder().decode(c.bytes.subarray(c.pos, c.pos + len));
  c.pos += len;
  return s;
}

export function bytes(c: Cursor, len: number): Uint8Array {
  if (c.pos + len > c.bytes.length) {
    throw new DecodeError(`truncated byte run of ${len}`, c.pos);
  }
  const out = c.bytes.subarray(c.pos, c.pos + len);
  c.pos += len;
  return out;
}
