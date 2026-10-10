/*
 * BERDecoder.cxx
 *
 * Abstract Syntax Notation 1 Encoding Rules
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 *
 * Copyright (c) 2001 Institute for Information Industry, Taiwan, Republic of China
 * (http://www.iii.org.tw/iiia/ewelcome.htm)
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): Huang-Ming Huang
 *
 * The code is adapted from asner.cxx of PWLib, but the dependancy on PWLib has
 * been removed.
 *
 * The code is modified by Genesys Telecommunications Labs UK, 2003-2011
 * Contributors: 
 *    Arunas Ruksnaitis <arunas.ruksnaitis@genesyslab.com>
 *    Rustam Mirzaev <rustam.mirzaev@genesyslab.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "asn1.h"

#if ASN_DEBUG
#  include <iostream>
#  include <iomanip>
#endif

namespace ASN1 {

extern unsigned CountBits(UINT_TYPE range);

class BERDecoder::BER_Element
{
public:
  typedef const char *position;
  typedef unsigned int size_type;
  typedef unsigned tag_type;

  BER_Element();
  ~BER_Element();

  bool decode(position start, position end);

  bool is_primitive() const { return m_primitive; }

  tag_type get_tag() const { return m_tag; }

  AbstractData* get_data() const { return m_data; }

  position get_buffer() const { return m_begin; }
  size_type get_size() const { return m_end - m_begin; }

#if ASN_DEBUG
  ASN1_STD ostream& print(ASN1_STD ostream& os) const;
#endif

private:
  position m_begin, m_end;

  tag_type m_tag;
  bool m_primitive;

  AbstractData *m_data;

  typedef ASN1_STD vector<BER_Element*> memlist_type;
  typedef memlist_type::iterator iterator;
  typedef memlist_type::const_iterator const_iterator;
  memlist_type m_members;

  friend class BERDecoder;
};

BERDecoder::BER_Element::BER_Element() :
m_begin(0),
m_end(0),
m_tag(0),
m_primitive(true),
m_data(0)
{
}

BERDecoder::BER_Element::~BER_Element()
{
  while(!m_members.empty()) {
    delete m_members.back();
    m_members.pop_back();
  }
}

#if ASN_DEBUG
ASN1_STD ostream& BERDecoder::BER_Element::print(ASN1_STD ostream& os) const
{
  char *tag_cl = 0;
  char cl_fmt[32];

  static int lvl =0;

  if(!m_tag) return os;

  switch(m_tag>>30) {
    case UniversalTagClass: tag_cl = "UNIVERSAL"; break;
    case ApplicationTagClass: tag_cl = "APPLICATION"; break;
    case ContextSpecificTagClass: tag_cl = "CONTEXT"; break;
    case PrivateTagClass: tag_cl = "PRIVATE"; break;

    default:
      sprintf(cl_fmt, "UNKNOWN-%d", m_tag>>30);
      tag_cl = cl_fmt;
      break;
  }
  os << ASN1_STD setw(lvl) << ""
     << '[' << ' ' << tag_cl << ' ' << ASN1_STD hex << (m_tag& ~(~0<<29)) << ASN1_STD dec
     << "]; size: " << get_size();

  if(m_primitive)
    os << ", primitive" << ASN1_STD endl;
  else {
    os << ", constructed" << ASN1_STD endl;
    lvl +=2;

    for(const_iterator i=m_members.begin(); i!=m_members.end(); ++i)
      (*i)->print(os);
    lvl -=2;
  }
  return os;
}
#endif

bool BERDecoder::decode(BER_Element &el, BERDecoder::memento_type end_pos)
{
  endPosition = end_pos;

  unsigned len;

  if(!decodeHeader(el.m_tag, el.m_primitive, len)) return false;

  el.m_begin = beginPosition;
  el.m_end = beginPosition + len;

  if(end_pos<el.m_end) return false;

  if(el.m_primitive)
    beginPosition = el.m_end;
  else {
    const memento_type e(endPosition);

    while(beginPosition<el.m_end) {
      el.m_members.push_back(new BER_Element);

      if(!decode(*el.m_members.back(), el.m_end)) {
        el.m_members.pop_back();
        return false;
      }
    }
    endPosition = e;
  }
  return true;
}

BERDecoder::BERDecoder(const char* first, size_t size, CoderEnv* coder)
: Visitor(coder)
, beginPosition(first)
, endPosition(first+size)
, dontCheckTag(0)
, m_parsed(new BER_Element)
{
#if ASN_DEBUG
  bool decode_result = decode(*m_parsed, first+size);
  if(decode_result)
    m_parsed->print(ASN1_STD cout << "BERDecoder RAW: \n");
  else
    ASN1_STD cout << "BERDecoder RAW: can't parse" << ASN1_STD endl;

  beginPosition = first;
  endPosition = first+size;
#endif
}

BERDecoder::~BERDecoder()
{
  delete m_parsed;
}

bool CHOICE::setID(unsigned tagVal, unsigned tagClass)
{
  bool result = true;
  unsigned tag = tagClass << 30 | tagVal;

  if (info()->tags == 0)
    choiceID = tagVal;
  else
  {
    result = false;

    for(choiceID = 0; choiceID<(int)info()->numChoices; ++choiceID) {
      if(makePrimitive(getSelectionTag())==tag) {
        result = true;
        break;
      }
    }
  }

  if (result)
  {
    if (!createSelection())
      return false;
  }
  return result;
}

inline bool BERDecoder::atEnd()
{
  return beginPosition >= endPosition;
}

inline unsigned char BERDecoder::decodeByte()
{
  assert(!atEnd());
  return *beginPosition++;
}

bool BERDecoder::do_visit(Null& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  beginPosition += len;
  return true;
}

bool BERDecoder::do_visit(BOOLEAN& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  while (len-- > 0) {
    if (atEnd())
      return false;

    value = (decodeByte() != 0);
  }

  return true;
}

bool BERDecoder::do_visit(INTEGER& value)
{
  unsigned len;
  if (!decodeHeader(value, len) || len == 0 || atEnd())
    return false;

  INTEGER::int_type accumulator = static_cast<signed char>(decodeByte()); // sign extended first byte
  while (--len > 0) {
    if (atEnd())
      return false;

    accumulator = (accumulator << 8) | decodeByte();
  }

  value = accumulator;
  return true;
}

bool BERDecoder::do_visit(ENUMERATED& value)
{
  unsigned len;
  if (!decodeHeader(value, len) || len == 0 || atEnd())
    return false;

  unsigned val = 0;
  while (len-- > 0) {
    if (atEnd())
      return false;

    val = (val << 8) |  decodeByte();
  }

  value.setFromInt(val);
  return true;
}

bool BERDecoder::do_visit(OBJECT_IDENTIFIER& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  if (atEnd())
    return false;

  beginPosition += len;
  return value.decodeCommon(beginPosition-len, len);

}

bool BERDecoder::do_visit(BIT_STRING& value)
{
  unsigned len;
  if (!decodeHeader(value, len) || len == 0 || atEnd())
    return false;

  value.totalBits = (len-1)*8 - decodeByte();
  unsigned nBytes = (value.totalBits+7)/8;
  if(!nBytes) // crash on VS8 platform when BitString is empty
    return true;
  value.bitData.resize(nBytes);
  return decodeBlock(&*value.bitData.begin(), nBytes) == nBytes;
}

bool BERDecoder::do_visit(OCTET_STRING& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  if(len == 0)
    return true;

  value.resize(len);
  return decodeBlock(&*value.begin(), len) == len;
}

bool BERDecoder::do_visit(AbstractString& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  if(len == 0)
    return true;

  value.resize(len);
  return decodeBlock(&*value.begin(), len) == len;
}

bool BERDecoder::do_visit(BMPString& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  if(len == 0)
    return true;

  value.resize(len/2);
  if ((beginPosition+len) >= endPosition)
    return false;

  for (unsigned i = 0; i < len/2; ++i)
    value[i] = (decodeByte() << 8) | decodeByte();
  return true;
}

bool BERDecoder::decodeChoicePreamle(CHOICE& value, memento_type& nextPosition)
{
  const char* savedPosition = beginPosition;

  unsigned tag;
  bool primitive;
  unsigned entryLen;
  if (!decodeHeader(tag, primitive, entryLen))
    return false;

  nextPosition = beginPosition + entryLen;

  if(value.getTag()) {
    if(!dontCheckTag && makePrimitive(value.getTag()) != tag)
      return true;

    savedPosition = beginPosition;
    decodeHeader(tag, primitive, entryLen);
  }

  if (value.setID(tag & 0x1fffffff, tag >> 30))
  {
    if (!value.isSelectionExplicit()) {
      beginPosition = savedPosition;
      dontCheckTag = 1;
    }

    return true;
  }

  beginPosition = savedPosition;
  return false;
}

struct FlagSaver
{
  FlagSaver(int& rfl) : m_value(rfl), m_ref(rfl) {}
  ~FlagSaver() { m_ref = m_value; }

private:
  int m_value, &m_ref;
};

bool BERDecoder::do_visit(CHOICE& value)
{
  memento_type memento;

  FlagSaver fs(dontCheckTag);

  if (decodeChoicePreamle(value,memento))
  {
#if ASN_DEBUG
    if(!value.isUnknownSelection() && !value.isUnselected())
      ASN1_STD cout << "visit(CHOICE): " << value.getSelectionName() << ASN1_STD endl;
    else
      ASN1_STD cout << "visit(CHOICE): unknown selection: " << value.currentSelection() << ASN1_STD endl;
#endif

    if (value.isUnknownSelection() || value.isUnselected() ||
        !value.getSelection()->accept(*this))
      return false;

    rollback(memento); //position at the next element
    return true;
  }
  else for(int i=0; i<(int)value.getNumChoices(); ++i) { //try all nested selections
    value.select(i);

    if(value.getSelectionInfoTag()==0 && value.getSelection()->accept(*this))
      return true;
  }

  return false;
}

bool BERDecoder::do_visit(SEQUENCE_OF_Base& value)
{
  value.clear();

  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  const char* endPos = beginPosition + len;

  if (endPos > endPosition )
    return false;

  SEQUENCE_OF_Base::iterator it = value.begin(), last = value.end();
  while (beginPosition < endPos && it != last)
  {
    if (!(*it)->accept(*this))
    {
      value.erase(it, last);
      return false;
    }
    ++it;
  }

  if (it != last)
    value.erase(it, last);

  while (beginPosition < endPos) {
    asn_ptr<AbstractData> obj(value.createElement());
    if (!obj->accept(*this)) return false;

    value.push_back(obj.release());
  }

  beginPosition = endPos;

  return true;
}

bool BERDecoder::do_visit(OpenData& value)
{
  const char* savedPosition = beginPosition;

  unsigned tag;
  bool primitive;
  unsigned entryLen;
  if (!decodeHeader(tag, primitive, entryLen))
    return false;

  if (value.getTag() == 0) {
    entryLen += beginPosition - savedPosition;
    beginPosition = savedPosition;
  }

  if (!value.has_buf())
    value.grab(new OpenBuf);

  value.get_buf().resize(entryLen);
  decodeBlock(&*value.get_buf().begin(), entryLen);
  return true;
}

bool BERDecoder::do_revisit(OpenData& value)
{
  if (!value.has_buf() || !value.has_data() || value.get_buf().empty()) return false;

  BERDecoder decoder(&*value.get_buf().begin(), value.get_buf().size(), get_env());
  return value.get_data().accept(decoder);
}

bool BERDecoder::do_visit(TypeConstrainedOpenData& value)
{
  assert(value.has_data());
  const char* savedPosition = beginPosition;

  unsigned tag;
  bool primitive;
  unsigned entryLen;
  if (!decodeHeader(tag, primitive, entryLen))
    return false;

  if (value.getTag() == 0)
    beginPosition = savedPosition;
  return value.get_data().accept(*this);
}

bool BERDecoder::do_visit(UTCTime& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  if(len == 0)
    return true;

  ASN1_STD vector<char> block(len+1, '\0');
  if (decodeBlock(&*block.begin(), len) == len)
  {
    value.set(&*block.begin());
    return true;
  }
  return false;
}

bool BERDecoder::do_visit(GeneralizedTime& value)
{
  unsigned len;
  if (!decodeHeader(value, len))
    return false;

  if(len == 0)
    return true;

  ASN1_STD vector<char> block(len+1, '\0');
  if (decodeBlock(&*block.begin(), len) == len)
  {
    value.set(&*block.begin());
    return true;
  }
  return false;
}

Visitor::VISIT_SEQ_RESULT BERDecoder::preVisitExtensionRoots(SEQUENCE& value)
{
  unsigned len;
  if (!decodeHeader(value, len)) return FAIL;

  endSEQUENCEPositions.push_back(beginPosition + len);
  return !atEnd() ? CONTINUE : STOP;
}

Visitor::VISIT_SEQ_RESULT BERDecoder::visitExtensionRoot(SEQUENCE& value, int index, int optional_id)
{
  if(beginPosition >= endSEQUENCEPositions.back()) {
    endSEQUENCEPositions.pop_back();
    return optional_id==-1 ? FAIL : STOP;
  }

  const char* savedPosition = beginPosition;

  dontCheckTag = 0;

  unsigned tag;
  bool primitive;
  unsigned entryLen;

  if (!decodeHeader(tag, primitive, entryLen)) return FAIL;

  unsigned fieldTag = value.getFieldTag(index);

  if (!fieldTag || (makePrimitive(fieldTag) == tag))
  {
    if (optional_id != -1)
      value.includeOptionalField(optional_id, index);

    AbstractData* field = value.getField(index);
    if (field)
    {
      if(fieldTag) {
        dontCheckTag = 1;
        if(!isExplicit(fieldTag)) {//is IMPLICIT
          beginPosition = savedPosition;
        }
      }
      else
        beginPosition = savedPosition; //EXPLICIT or native tag

      if (field->accept(*this))
      {
        if(beginPosition >= endSEQUENCEPositions.back()) {
          endSEQUENCEPositions.pop_back();
          return STOP;
        }
        return CONTINUE;
      }

      beginPosition = savedPosition;

      if (optional_id != -1)
      {
        value.removeOptionalField(optional_id);
        return CONTINUE;
      }
      return FAIL;
    }
  }
  beginPosition = savedPosition;
  return optional_id != -1 ? CONTINUE : FAIL;
}


Visitor::VISIT_SEQ_RESULT BERDecoder::visitKnownExtension(SEQUENCE& value, int index, int optional_id)
{
  return visitExtensionRoot(value, index, optional_id);
}

bool BERDecoder::visitUnknownExtensions(SEQUENCE& value)
{
  beginPosition = endSEQUENCEPositions.back();
  endSEQUENCEPositions.pop_back();
  return true;
}

bool BERDecoder::decodeHeader(unsigned& tag, bool & primitive, unsigned & len)
{
  if(atEnd())
    return false;

  unsigned tagVal, tagClass;
  unsigned char ident = decodeByte();

  if(!ident) // universal tag 0 is not valid
    return false;

  tagClass = ident >> 6;
  primitive = ((ident&0x20) == 0);

  tagVal = ident&0x1f;

  if (tagVal == 0x1f) {
    unsigned char b;
    tagVal = 0;
    do {
      if (atEnd())
        return false;

      b = decodeByte();
      tagVal = (tagVal << 7) | (b&0x7f);
    } while ((b&0x80) != 0);
  }

  tag = tagVal | (tagClass << 30);

  if (atEnd())
    return false;

  unsigned char len_len = decodeByte();
  if ((len_len & 0x80) == 0) {
    len = len_len;
    return true;
  }

  len_len &= 0x7f;

  len = 0;
  while (len_len-- > 0) {
    if (atEnd())
      return false;

    len = (len << 8) | decodeByte();
  }

  return true;
}

bool BERDecoder::decodeHeader(AbstractData & obj, unsigned & len)
{
  const char* pos = beginPosition;

  unsigned tag;
  bool primitive;

  if (decodeHeader(tag, primitive, len) &&
      (tag == makePrimitive(obj.getTag()) || dontCheckTag)) {
    dontCheckTag = 0;

    if(isExplicit(obj.getTag())) {
      unsigned tag1, len1 =len;
      bool p1;

      if(decodeHeader(tag1, p1, len) && tag1 == obj.getParentTag())
        return true;
    }
    else //is implicit
      return true;
  }

  beginPosition = pos;
  return false;
}

unsigned BERDecoder::decodeBlock(char * bufptr, unsigned nBytes)
{
  if (beginPosition+nBytes > endPosition)
    nBytes = endPosition - beginPosition;

  if (nBytes == 0) return 0;

  memcpy(bufptr, beginPosition, nBytes);

  beginPosition += nBytes;
  return nBytes;
}

}//namespace
