import { describe, expect, it } from 'vitest';

import { localCommands, shellQuote, tryTuComponent } from '../src/lib/commands';

describe('shellQuote', () => {
  it('leaves a plain word as it is', () => {
    expect(shellQuote('export:_ZN3api15CPosixSemaphore4LockEv')).toBe('export:_ZN3api15CPosixSemaphore4LockEv');
  });

  it('single-quotes a word with spaces or quotes', () => {
    expect(shellQuote('-O3 -fPIC')).toBe(`'-O3 -fPIC'`);
    expect(shellQuote(`a'b`)).toBe(`'a'\\''b'`);
  });
});

describe('localCommands', () => {
  it('ranks the lifted file at the flags it was lifted for, and reruns the setup of a benchmark row', () => {
    const blocks = localCommands({
      index: 390,
      symbol: 'atoi',
      name: 'atoi',
      component: '-',
      flags: '-Os -fno-builtin',
      setup: 'libc',
    });
    const lines = blocks.flatMap((b) => b.lines);
    expect(lines).toContain(
      `pnpm wasm rank ../target/vota_web_wasm.wasm 390 --extract export:atoi --flags '-Os -fno-builtin' /tmp/lift/390.cpp`,
    );
    expect(lines).toContain('pnpm bench run --label my-change --setup libc');
  });

  it('adds a function that is not a benchmark row', () => {
    const lines = localCommands({ index: 137, symbol: 'f137', name: 'f137', component: '-', flags: '-O2', setup: null }).flatMap(
      (b) => b.lines,
    );
    expect(lines).toContain('pnpm bench add 137');
  });

  it('scores an application function with try-tu.sh at the flags of the directory it lands in', () => {
    const lines = localCommands({
      index: 2669,
      symbol: '_ZNK7ecourna3app5dados29CRegistroIdentificacaoEleitor27GetIdentificadorHabilitacaoEv',
      name: 'ecourna::app::dados::CRegistroIdentificacaoEleitor::GetIdentificadorHabilitacao() const',
      component: 'ecourna',
      flags: '-O3',
      setup: null,
    }).flatMap((b) => b.lines);
    expect(lines).toContain(
      'tools/round/try-tu.sh --single 2669 --extract export:_ZNK7ecourna3app5dados29CRegistroIdentificacaoEleitor27GetIdentificadorHabilitacaoEv --component ecourna/app/dados /tmp/lift/2669.cpp',
    );
  });

  it('does not offer try-tu.sh for a library function', () => {
    const lines = localCommands({ index: 390, symbol: 'atoi', name: 'atoi', component: 'libc', flags: '-Os', setup: null }).flatMap(
      (b) => b.lines,
    );
    expect(lines.some((l) => l.includes('try-tu.sh'))).toBe(false);
  });
});

describe('tryTuComponent', () => {
  it('follows the namespaces of an ecourna function down to its directory', () => {
    expect(tryTuComponent('ecourna', 'ecourna::api::compression::CZip::~CZip()')).toBe('ecourna/api/compression');
    expect(tryTuComponent('ecourna', 'ecourna::util::trim(char const*)')).toBe('ecourna/util');
  });

  it('keeps the ecourna root for a name outside its namespaces', () => {
    expect(tryTuComponent('ecourna', '__multi3')).toBe('ecourna');
  });

  it('maps the uenux2 components and the ASN.1 runtime and modules to their trees', () => {
    expect(tryTuComponent('comum', 'comum::CRdvVota::Candidato(unsigned char, unsigned int, unsigned char) const')).toBe('uenux2');
    expect(tryTuComponent('api', 'api::CTickManager::StartTick()')).toBe('uenux2');
    expect(tryTuComponent('ASN1', 'ASN1::AbstractData::isValid() const')).toBe('asn1');
    expect(tryTuComponent('asn1-modules', 'ModuloTiposEleitorais::UF::~UF()')).toBe('asn1/modules');
  });

  it('returns null for the libraries and unattributed functions', () => {
    expect(tryTuComponent('openssl', 'EVP_DigestInit')).toBeNull();
    expect(tryTuComponent('-', 'f137')).toBeNull();
  });
});
