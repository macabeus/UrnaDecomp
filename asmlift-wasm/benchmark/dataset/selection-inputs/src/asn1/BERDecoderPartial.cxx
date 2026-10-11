/*
 * BERDecoderPartial.cxx
 *
 * TSE's partial BER decoder (reconstructed from vota_web_wasm.wasm): the BERDecoder of III-ASN.1 reading
 * through an ISeeder, reporting every decoded node to the registered IAbstractVisitor by its dotted path
 * and logging every decoding error into an ostringstream.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "asn1.h"
#include "BufferSeeder.h"

namespace ASN1 {

static BERDecoderPartial::Visitantes defaultVisitors;
static const ASN1_STD string badEndOfStream(" bad end of stream");

BERDecoderPartial::BERDecoderPartial(ASN1_STD shared_ptr<ISeeder> seeder, CoderEnv* coder)
: Visitor(coder)
, m_checkTag(true)
, m_seeder(seeder)
{
}

BERDecoderPartial::~BERDecoderPartial()
{
}

void BERDecoderPartial::notify(int index, const ASN1_STD string& name, AbstractData* data, unsigned position, unsigned length, bool mayFree)
{
  ASN1_STD string path;
  for (unsigned i = 0; i < m_path.size(); ++i) {
    path += m_path[i];
    path += ".";
  }

  if (name.empty())
    path.erase(path.size() - 1);
  else
    path += name;

  Visitantes::iterator it = m_visitors.begin();
  while (it != m_visitors.end()) {
    if (it->getName() == path)
      break;
    ++it;
  }

  if (it != m_visitors.end()) {
    ASN1_STD shared_ptr<IAbstractVisitor> visitor = it->getVisitor();
    if (mayFree && visitor->visit(index, path, data, position, length) == 1) {
      DataFree dataFree;
      data->accept(dataFree);
    }
  }
}

bool BERDecoderPartial::do_visit(Null& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  m_seeder->skip(len);
  return true;
}

bool BERDecoderPartial::do_visit(BOOLEAN& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  while (len-- > 0) {
    if (m_seeder->eof()) {
      m_log << "BOOLEAN" << badEndOfStream;
      return false;
    }

    value = (m_seeder->read() != 0);
  }

  return true;
}

bool BERDecoderPartial::do_visit(INTEGER& value)
{
  unsigned len;
  if (!decodeHeader(value, len) || len == 0 || m_seeder->eof())
    return false;

  INTEGER::int_type accumulator = static_cast<signed char>(m_seeder->read()); // sign extended first byte
  while (--len > 0) {
    if (m_seeder->eof()) {
      m_log << "INTEGER" << badEndOfStream;
      return false;
    }

    accumulator = (accumulator << 8) | m_seeder->read();
  }

  value = accumulator;
  trace_invalid(m_log, "", value);
  return true;
}

bool BERDecoderPartial::do_visit(ENUMERATED& value)
{
  unsigned len;
  if (!decodeHeader(value, len) || len == 0 || m_seeder->eof())
    return false;

  unsigned val = 0;
  while (len-- > 0) {
    if (m_seeder->eof()) {
      m_log << "ENUMERATED" << badEndOfStream;
      return false;
    }

    val = (val << 8) | m_seeder->read();
  }

  value.setFromInt(val);
  trace_invalid(m_log, "", value);
  return true;
}

bool BERDecoderPartial::do_visit(OBJECT_IDENTIFIER& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  if (m_seeder->eof()) {
    m_log << "OBJECT_IDENTIFIER" << badEndOfStream;
    return false;
  }

  ASN1_STD shared_ptr<char[]> buffer(new char[len]);
  m_seeder->read(buffer.get(), len);
  bool result = value.decodeCommon(buffer.get(), len);
  trace_invalid(m_log, "", value);
  return result;
}

bool BERDecoderPartial::do_visit(BIT_STRING& value)
{
  unsigned len;
  if (!decodeHeader(value, len) || len == 0 || m_seeder->eof())
    return false;

  value.totalBits = (len-1)*8 - m_seeder->read();
  unsigned nBytes = (value.totalBits+7)/8;
  if(!nBytes) // crash on VS8 platform when BitString is empty
    return true;
  value.bitData.resize(nBytes);
  unsigned read = m_seeder->read(&*value.bitData.begin(), nBytes);
  trace_invalid(m_log, "", value);
  return read == nBytes;
}

bool BERDecoderPartial::do_visit(OCTET_STRING& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  if(len == 0)
    return true;

  value.resize(len);
  unsigned read = m_seeder->read(&*value.begin(), len);
  trace_invalid(m_log, "", value);
  return read == len;
}

bool BERDecoderPartial::do_visit(AbstractString& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  if(len == 0)
    return true;

  value.resize(len);
  unsigned read = m_seeder->read(&*value.begin(), len);
  trace_invalid(m_log, "", value);
  return read == len;
}

bool BERDecoderPartial::do_visit(BMPString& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  if(len == 0)
    return true;

  value.resize(len/2);
  if (m_seeder->remaining() < len) {
    m_log << "BMPString" << badEndOfStream;
    return false;
  }

  for (unsigned i = 0; i < len/2; ++i)
    value[i] = (m_seeder->read() << 8) | m_seeder->read();
  trace_invalid(m_log, "", value);
  return true;
}

bool BERDecoderPartial::decodeChoicePreamle(CHOICE& value, unsigned& nextPosition)
{
  unsigned savedPosition = m_seeder->tell();

  unsigned tag;
  bool primitive;
  unsigned entryLen;
  if (!decodeHeader(tag, primitive, entryLen))
    return false;

  nextPosition = m_seeder->tell() + entryLen;

  if(value.getTag()) {
    if(m_checkTag && makePrimitive(value.getTag()) != tag)
      return true;

    savedPosition = m_seeder->tell();
    decodeHeader(tag, primitive, entryLen);
  }

  if (value.setID(tag & 0x1fffffff, tag >> 30))
  {
    if (!value.isSelectionExplicit()) {
      m_seeder->seek(savedPosition);
      m_checkTag = false;
    }
    return true;
  }

  m_seeder->seek(savedPosition);
  m_log << " CHOICE Preamble unknown selection at " << savedPosition;
  return false;
}

struct FlagSaver
{
  FlagSaver(bool& rfl) : m_value(rfl), m_ref(rfl) {}
  ~FlagSaver() { m_ref = m_value; }

private:
  bool m_value, &m_ref;
};

bool BERDecoderPartial::do_visit(CHOICE& value)
{
  unsigned memento;

  FlagSaver fs(m_checkTag);

  if (decodeChoicePreamle(value,memento))
  {
    switch (value.currentSelection()) {
    case CHOICE::unknownSelection_:
      m_log << " CHOICE unknown selection";
      return false;
    case CHOICE::unselected_:
      m_log << " CHOICE unselected";
      return false;
    default:
      if (!value.getSelection()->accept(*this)) {
        m_log << " CHOICE selection unaccepted";
        return false;
      }
      m_seeder->seek(memento); //position at the next element
      return true;
    }
  }
  else for(int i=0; i<(int)value.getNumChoices(); ++i) { //try all nested selections
    value.select(i);

    if(value.getSelectionInfoTag()==0 && value.getSelection()->accept(*this))
      return true;
  }

  return false;
}

bool BERDecoderPartial::do_visit(SEQUENCE_OF_Base& value)
{
  value.clear();

  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  if (len > m_seeder->remaining()) {
    m_log << " SEQUENCE OF no space left " << len << " greater than " << m_seeder->remaining() << " at " << m_seeder->tell();
    return false;
  }

  unsigned endPos = m_seeder->tell() + len;

  SEQUENCE_OF_Base::iterator it = value.begin(), last = value.end();
  while (m_seeder->tell() < endPos && it != last)
  {
    if (!(*it)->accept(*this))
    {
      value.erase(it, last);
      m_log << " SEQUENCE OF element unaccepted " << m_seeder->tell();
      return false;
    }
    ++it;
  }

  if (it != last)
    value.erase(it, last);

  while (m_seeder->tell() < endPos) {
    asn_ptr<AbstractData> obj(value.createElement());
    unsigned position = m_seeder->tell();
    if (!obj->accept(*this)) {
      m_log << " SEQUENCE OF object unaccepted " << m_seeder->tell();
      return false;
    }

    if ((obj->getTag() & ~1u) != 16) {
      unsigned end = m_seeder->tell();
      ASN1_STD string name("OF");
      notify(0, name, obj.get(), position, end - position, false);
    }

    value.push_back(obj.release());
  }

  if (m_seeder->tell() == endPos)
    return true;

  m_log << " SEQUENCE OF incorrect final position " << m_seeder->tell() << " should be " << endPos;
  return false;
}

bool BERDecoderPartial::do_visit(OpenData& value)
{
  unsigned savedPosition = m_seeder->tell();

  unsigned tag;
  bool primitive;
  unsigned entryLen;
  if (!decodeHeader(tag, primitive, entryLen))
    return false;

  if (value.getTag() == 0) {
    entryLen += m_seeder->tell() - savedPosition;
    m_seeder->seek(savedPosition);
  }

  if (!value.has_buf())
    value.grab(new OpenBuf);

  value.get_buf().resize(entryLen);
  m_seeder->read(&*value.get_buf().begin(), entryLen);
  trace_invalid(m_log, "", value);
  return true;
}

bool BERDecoderPartial::do_revisit(OpenData& value)
{
  if (!value.has_buf() || !value.has_data() || value.get_buf().empty()) return false;

  ASN1_STD shared_ptr<ISeeder> seeder(new BufferSeeder(&*value.get_buf().begin(), value.get_buf().size()));
  BERDecoderPartial decoder(seeder, get_env());
  bool result = value.get_data().accept(decoder);
  trace_invalid(m_log, "", value);
  return result;
}

bool BERDecoderPartial::do_visit(TypeConstrainedOpenData& value)
{
  unsigned savedPosition = m_seeder->tell();

  unsigned tag;
  bool primitive;
  unsigned entryLen;
  if (!decodeHeader(tag, primitive, entryLen))
    return false;

  if (value.getTag() == 0)
    m_seeder->seek(savedPosition);
  bool result = value.get_data().accept(*this);
  trace_invalid(m_log, "", value);
  return result;
}

template <class TIME>
bool BERDecoderPartial::decodeTime(TIME& value, unsigned& len)
{
  if (!decodeHeader(value, len))
    return false;

  if(len == 0)
    return true;

  ASN1_STD vector<char> block(len+1, '\0');
  unsigned read = m_seeder->read(&*block.begin(), len);
  if (read == len)
  {
    value.set(&*block.begin());
    trace_invalid(m_log, "", value);
  }
  return read == len;
}

bool BERDecoderPartial::do_visit(UTCTime& value)
{
  unsigned len;
  bool result = decodeTime(value, len);
  if (!result)
    m_log << "UTCTime invalid decodeblock size " << len;
  return result;
}

bool BERDecoderPartial::do_visit(GeneralizedTime& value)
{
  unsigned len;
  bool result = decodeTime(value, len);
  if (!result)
    m_log << "GeneralizedTime invalid decodeblock size " << len;
  return result;
}

Visitor::VISIT_SEQ_RESULT BERDecoderPartial::preVisitExtensionRoots(SEQUENCE& value)
{
  unsigned position = m_seeder->tell();
  unsigned len;
  if (!decodeHeader(value, len)) return FAIL;

  unsigned headerLength = m_seeder->tell() - position;
  if (m_path.empty()) {
    m_path.push_back(value.getTypeName());
    ASN1_STD string name("");
    notify(0, name, &value, position, len + headerLength, false);
  }
  else {
    ASN1_STD string name("OF");
    notify(0, name, &value, position, len + headerLength, false);
  }

  endSEQUENCEPositions.push_back(m_seeder->tell() + len);
  return !m_seeder->eof() ? CONTINUE : STOP;
}

Visitor::VISIT_SEQ_RESULT BERDecoderPartial::visitExtensionRoot(SEQUENCE& value, int index, int optional_id)
{
  if(m_seeder->tell() >= endSEQUENCEPositions.back()) {
    endSEQUENCEPositions.pop_back();
    m_path.pop_back();
    return optional_id==-1 ? FAIL : STOP;
  }

  unsigned savedPosition = m_seeder->tell();

  m_checkTag = true;

  unsigned tag;
  bool primitive;
  unsigned entryLen;

  if (!decodeHeader(tag, primitive, entryLen)) return FAIL;

  unsigned headerLength = m_seeder->tell() - savedPosition;
  unsigned fieldTag = value.getFieldTag(index);

  if (!fieldTag || (makePrimitive(fieldTag) == tag))
  {
    if (optional_id != -1)
      value.includeOptionalField(optional_id, index);

    AbstractData* field = value.getField(index);
    if (field)
    {
      if(fieldTag) {
        m_checkTag = false;
        if(!isExplicit(fieldTag)) {//is IMPLICIT
          m_seeder->seek(savedPosition);
        }
      }
      else
        m_seeder->seek(savedPosition); //EXPLICIT or native tag

      bool accepted;
      if ((tag & ~1u) == 16) {
        {
          ASN1_STD string name(value.getFieldName(index));
          notify(index, name, field, savedPosition, entryLen + headerLength, true);
          m_path.push_back(ASN1_STD string(value.getFieldName(index)));
        }
        accepted = field->accept(*this);
        if (accepted)
          m_path.pop_back();
      }
      else {
        accepted = field->accept(*this);
        if (accepted) {
          ASN1_STD string name(value.getFieldName(index));
          notify(index, name, field, savedPosition, entryLen + headerLength, true);
        }
      }

      if (accepted)
      {
        if(m_seeder->tell() >= endSEQUENCEPositions.back()) {
          endSEQUENCEPositions.pop_back();
          return STOP;
        }
        return CONTINUE;
      }

      m_seeder->seek(savedPosition);

      if (optional_id != -1)
      {
        value.removeOptionalField(optional_id);
        return CONTINUE;
      }
      return FAIL;
    }
  }
  m_seeder->seek(savedPosition);
  if (optional_id != -1)
    return CONTINUE;

  m_log << "visitExtensionRoot unidentified non optional field at" << savedPosition;
  return FAIL;
}


Visitor::VISIT_SEQ_RESULT BERDecoderPartial::visitKnownExtension(SEQUENCE& value, int index, int optional_id)
{
  return visitExtensionRoot(value, index, optional_id);
}

bool BERDecoderPartial::visitUnknownExtensions(SEQUENCE& value)
{
  m_seeder->seek(endSEQUENCEPositions.back());
  endSEQUENCEPositions.pop_back();
  m_path.pop_back();
  return true;
}

bool BERDecoderPartial::decodeHeader(unsigned& tag, bool & primitive, unsigned & len)
{
  if(m_seeder->eof())
    return false;

  unsigned tagVal, tagClass;
  unsigned char ident = m_seeder->read();

  if(!ident) // universal tag 0 is not valid
    return false;

  tagClass = ident >> 6;
  primitive = ((ident&0x20) == 0);

  tagVal = ident&0x1f;

  if (tagVal == 0x1f) {
    unsigned char b;
    tagVal = 0;
    do {
      if (m_seeder->eof())
        return false;

      b = m_seeder->read();
      tagVal = (tagVal << 7) | (b&0x7f);
    } while ((b&0x80) != 0);

    if (tagVal < 0x1f)
      return false;
  }

  tag = tagVal | (tagClass << 30);

  if (m_seeder->eof())
    return false;

  unsigned char len_len = m_seeder->read();
  if ((len_len & 0x80) == 0) {
    len = len_len;
    return true;
  }

  len_len &= 0x7f;

  len = 0;
  while (len_len-- > 0) {
    if (m_seeder->eof())
      return false;

    len = (len << 8) | m_seeder->read();
  }

  return true;
}

bool BERDecoderPartial::decodeHeader(AbstractData & obj, unsigned & len)
{
  unsigned pos = m_seeder->tell();

  unsigned tag;
  bool primitive;

  if (decodeHeader(tag, primitive, len) &&
      (tag == makePrimitive(obj.getTag()) || !m_checkTag)) {
    m_checkTag = true;

    if(isExplicit(obj.getTag())) {
      unsigned tag1;
      bool p1;

      if(decodeHeader(tag1, p1, len) && tag1 == obj.getParentTag())
        return true;
    }
    else //is implicit
      return true;
  }

  m_seeder->seek(pos);
  ASN1_STD string typeName = obj.getTypeName();
  m_log << " This HEADER has invalid on tag " << tag << " on type " << typeName << " on position " << pos;
  return false;
}

}//namespace

// <stubs>
// f3469 � 117 B � unmatched � placement: link order � name unknown
// ? f3469(i32, i32, i32) { ... }
// </stubs>
