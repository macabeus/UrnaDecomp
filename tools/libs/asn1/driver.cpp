// Keep-alive driver for the ASN.1 runtime probe: instantiates the runtime the way the app's generated
// Modulo* types do (SEQUENCE / CHOICE / ENUMERATED subclasses with a static theInfo, SEQUENCE_OF<T>,
// Constrained_INTEGER<FixedConstraint,lo,hi>, GeneralString, NumericString, OCTET_STRING,
// Constrained_OCTET_STRING<SizeConstraint<FixedConstraint,4,4>>, GeneralizedTime, UTCTime, BOOLEAN)
// and drives every visitor the target's vtables hold (BER/PER/AVN encode+decode, ValidChecker,
// StrictlyValidChecker, InvalidTracer via trace_invalid, DataLengthCounter/PrimitiveChecker via the
// BER encoder). Roots are EMSCRIPTEN_KEEPALIVE so wasm-ld keeps them and binaryen inlines as in the app
// (an exported library blocks one-caller inlining; a bare link keeps only vtable code: docs/reference/facts/tool-traps.md §X1, §X2).
#include <emscripten.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include "asn1.h"
#include "BufferSeeder.h"

namespace Probe {
using namespace ASN1;

typedef Constrained_INTEGER<FixedConstraint, 0, 99> Pequeno;
typedef Constrained_INTEGER<FixedConstraint, 0, 99999999> Grande;
typedef Constrained_OCTET_STRING<SizeConstraint<FixedConstraint, 4, 4> > Quatro;

class Cor : public ENUMERATED {
  typedef ENUMERATED Inherited;
 protected:
  Cor(const void* info) : Inherited(info) {}
 public:
  enum NamedNumber { vermelho, verde, azul };
  Cor(NamedNumber v = vermelho) : Inherited(&theInfo) { setFromInt(v); }
  Cor* clone() const { return static_cast<Cor*>(Inherited::clone()); }
  static AbstractData* create(const void* info) { return new Cor(info); }
  static const InfoType theInfo;
};
const char* corNames[] = {"vermelho", "verde", "azul"};
const Cor::InfoType Cor::theInfo = {Cor::create, 0x0a, 0, false, 2, corNames};

class Item : public SEQUENCE {
  typedef SEQUENCE Inherited;
 protected:
  Item(const void* info) : Inherited(info) {}
 public:
  enum OptionalFields { e_opcional };
  Item() : Inherited(&theInfo) {}
  Item(const Item& o) : Inherited(o) {}
  Item& operator=(const Item& o) { Inherited::operator=(o); return *this; }
  Item* clone() const { return static_cast<Item*>(Inherited::clone()); }
  static AbstractData* create(const void* info) { return new Item(info); }
  Pequeno& get_codigo() { return *static_cast<Pequeno*>(fields[0]); }
  GeneralString& get_nome() { return *static_cast<GeneralString*>(fields[1]); }
  OCTET_STRING& get_dados() { return *static_cast<OCTET_STRING*>(fields[2]); }
  Cor& get_cor() { return *static_cast<Cor*>(fields[3]); }
  BOOLEAN& get_opcional() { includeOptionalField(e_opcional, 4); return *static_cast<BOOLEAN*>(fields[4]); }
  static const void* fieldInfos[5];
  static int fieldIds[5];
  static const char* fieldNames[5];
  static const Inherited::InfoType theInfo;
};
const void* Item::fieldInfos[5] = {&Pequeno::theInfo, &GeneralString::theInfo, &OCTET_STRING::theInfo, &Cor::theInfo, &BOOLEAN::theInfo};
int Item::fieldIds[5] = {-1, -1, -1, -1, 0};
const char* Item::fieldNames[5] = {"codigo", "nome", "dados", "cor", "opcional"};
const Item::InfoType Item::theInfo = {Item::create, 0x10, 0, false, Item::fieldInfos, Item::fieldIds, 5, 0, 1, NULL, &SEQUENCE::defaultTag, Item::fieldNames};

typedef SEQUENCE_OF<Item> Itens;
typedef SEQUENCE_OF<Pequeno> Numeros;
typedef SEQUENCE_OF<GeneralString> Nomes;
typedef SEQUENCE_OF<Item, SizeConstraint<FixedConstraint, 0, 10> > ItensLimitados;

class Escolha : public CHOICE {
  typedef CHOICE Inherited;
 protected:
  Escolha(const void* info) : Inherited(info) {}
 public:
  enum Choices { e_numero, e_texto };
  Escolha() : Inherited(&theInfo) {}
  Escolha* clone() const { return static_cast<Escolha*>(Inherited::clone()); }
  static AbstractData* create(const void* info) { return new Escolha(info); }
  Grande& select_numero() { return *static_cast<Grande*>(setSelection(e_numero, AbstractData::create(&Grande::theInfo))); }
  NumericString& select_texto() { return *static_cast<NumericString*>(setSelection(e_texto, AbstractData::create(&NumericString::theInfo))); }
  static const void* selectionInfos[2];
  static const char* selectionNames[2];
  static const InfoType theInfo;
};
const void* Escolha::selectionInfos[2] = {&Grande::theInfo, &NumericString::theInfo};
const char* Escolha::selectionNames[2] = {"numero", "texto"};
unsigned escolhaTags[2] = {0x80000000u | 0, 0x80000000u | 1};
const Escolha::InfoType Escolha::theInfo = {Escolha::create, 0xffffffff, 0, false, Escolha::selectionInfos, 2, 2, escolhaTags, Escolha::selectionNames};

class Pacote : public SEQUENCE {
  typedef SEQUENCE Inherited;
 protected:
  Pacote(const void* info) : Inherited(info) {}
 public:
  Pacote() : Inherited(&theInfo) {}
  Pacote* clone() const { return static_cast<Pacote*>(Inherited::clone()); }
  static AbstractData* create(const void* info) { return new Pacote(info); }
  Itens& get_itens() { return *static_cast<Itens*>(fields[0]); }
  Numeros& get_numeros() { return *static_cast<Numeros*>(fields[1]); }
  Nomes& get_nomes() { return *static_cast<Nomes*>(fields[2]); }
  Escolha& get_escolha() { return *static_cast<Escolha*>(fields[3]); }
  GeneralizedTime& get_quando() { return *static_cast<GeneralizedTime*>(fields[4]); }
  UTCTime& get_utc() { return *static_cast<UTCTime*>(fields[5]); }
  Quatro& get_quatro() { return *static_cast<Quatro*>(fields[6]); }
  ItensLimitados& get_limitados() { return *static_cast<ItensLimitados*>(fields[7]); }
  static const void* fieldInfos[8];
  static int fieldIds[8];
  static const char* fieldNames[8];
  static const Inherited::InfoType theInfo;
};
const void* Pacote::fieldInfos[8] = {&Itens::theInfo, &Numeros::theInfo, &Nomes::theInfo, &Escolha::theInfo, &GeneralizedTime::theInfo, &UTCTime::theInfo, &Quatro::theInfo, &ItensLimitados::theInfo};
int Pacote::fieldIds[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
const char* Pacote::fieldNames[8] = {"itens", "numeros", "nomes", "escolha", "quando", "utc", "quatro", "limitados"};
const Pacote::InfoType Pacote::theInfo = {Pacote::create, 0x10, 0, false, Pacote::fieldInfos, Pacote::fieldIds, 8, 0, 0, NULL, &SEQUENCE::defaultTag, Pacote::fieldNames};

static void preenche(Pacote& p) {
  Item it;
  it.get_codigo() = 7;
  it.get_nome() = "nome";
  it.get_dados().assign(3, 'x');
  it.get_cor() = Cor::verde;
  it.get_opcional() = true;
  p.get_itens().push_back(it);
  p.get_itens().push_back(it);
  p.get_numeros().push_back(Pequeno(42));
  p.get_nomes().push_back(GeneralString("abc"));
  p.get_escolha().select_numero() = 123456;
  p.get_quando().set("20261007123000Z");
  p.get_utc().set("261007123000Z");
  p.get_quatro().assign(4, 'q');
  p.get_limitados().push_back(it);
}
}  // namespace Probe

extern "C" {
EMSCRIPTEN_KEEPALIVE int probe_roundtrip(int rule, const char* in, int inLen, char* out, int outCap) {
  using namespace Probe;
  CoderEnv env;
  env.set_encodingRule(static_cast<CoderEnv::EncodingRules>(rule));
  Pacote a, b;
  preenche(a);
  std::vector<char> buf;
  if (!env.encode(a, std::back_inserter(buf))) return -1;
  if (!env.decode(buf.begin(), buf.end(), b, false)) return -2;
  if (in && inLen > 0 && !env.decode(in, inLen, b, true)) return -3;
  if (!(a == b)) return -4;
  if (a < b || a.compare(b) != 0) return -5;
  int n = buf.size() < (size_t)outCap ? buf.size() : outCap;
  for (int i = 0; i < n; ++i) out[i] = buf[i];
  return n;
}

EMSCRIPTEN_KEEPALIVE int probe_check(void) {
  using namespace Probe;
  Pacote a;
  preenche(a);
  Pacote* c = a.clone();
  int r = (a.isValid() ? 1 : 0) | (a.isStrictlyValid() ? 2 : 0) | (c->isValid() ? 4 : 0);
  c->get_numeros().push_back(Pequeno(100));
  std::ostringstream os;
  if (!trace_invalid(os, "pacote", *c)) r |= 8;
  Escolha e;
  e.select_texto() = "123";
  r |= e.isValid() ? 16 : 0;
  Item it;
  it.get_opcional();
  it.removeOptionalField(Item::e_opcional);
  r |= it.hasOptionalField(Item::e_opcional) ? 32 : 0;
  std::cout << os.str().size() << r;
  delete c;
  return r;
}

EMSCRIPTEN_KEEPALIVE int probe_avn(const char* text, int len) {
  using namespace Probe;
  Pacote a;
  preenche(a);
  std::ostringstream os;
  AVNEncoder enc(os);
  if (!a.accept(enc)) return -1;
  std::string s = os.str();
  if (text && len > 0) s.assign(text, len);
  std::istringstream is(s);
  AVNDecoder dec(is);
  Pacote b;
  if (!b.accept(dec)) return -2;
  return a == b ? (int)s.size() : -3;
}

EMSCRIPTEN_KEEPALIVE int probe_ber_raw(const char* in, int len) {
  using namespace Probe;
  Pacote b;
  BERDecoder dec(in, len);
  if (!b.accept(dec)) return -1;
  OpenBuf buf;
  BEREncoder enc(buf);
  if (!b.accept(enc)) return -2;
  PEREncoder penc(buf);
  if (!b.accept(penc)) return -3;
  PERDecoder pdec(&buf[0], buf.size());
  return b.accept(pdec) ? (int)buf.size() : -4;
}
}

namespace Probe {
// TSE's partial decoder: a visitor registered by path, DataFree on demand (the fork's classes)
class Contador : public ASN1::IAbstractVisitor {
 public:
  int visitas;
  Contador() : visitas(0) {}
  int visit(int index, const std::string& path, ASN1::AbstractData* data, unsigned position, unsigned length) {
    ++visitas;
    std::cout << index << path << position << length;
    return (data->getTag() == 4) ? 1 : 0;
  }
};
}  // namespace Probe

extern "C" {
EMSCRIPTEN_KEEPALIVE int probe_partial(const char* in, int len, int freeIt) {
  using namespace Probe;
  CoderEnv env;
  std::shared_ptr<ISeeder> seeder(new BufferSeeder(in, len));
  BERDecoderPartial dec(seeder, &env);
  std::shared_ptr<IAbstractVisitor> contador(new Contador);
  dec.addVisitor("Pacote.itens", contador);
  dec.addVisitor("Pacote.numeros.OF", new Contador);
  BERDecoderPartial::Visitantes vs;
  vs.push_back(BERDecoderPartial::Visitante("Pacote", contador));
  dec.setVisitors(vs);
  Pacote b;
  int r = b.accept(dec) ? 1 : 0;
  dec.clearVisitors();
  std::cout << dec.getLog().str();
  if (freeIt) {
    DataFree df;
    b.accept(df);
  }
  return r + static_cast<Contador&>(*contador).visitas;
}
}
