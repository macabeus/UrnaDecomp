/*
 * BEREncoder.cxx
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

namespace ASN1 {

extern unsigned CountBits(UINT_TYPE range);

class PrimitiveChecker : public ConstVisitor
{
private:
  using ConstVisitor::do_visit;
  bool do_visit(const AbstractData& value) { return true; }
  bool do_visit(const CHOICE& value) {
    assert(value.currentSelection() >= 0) ;
    return value.getSelection()->accept(*this);
  }
  bool do_visit(const OpenData& value) {
    assert(value.has_data());
    return value.get_data().accept(*this);
  }
  bool do_visit(const SEQUENCE_OF_Base& value) { return false; }
  bool preVisitExtensionRoots(const SEQUENCE& value) { return false; }
};

unsigned getIntegerDataLength(INT_TYPE value)
{
  // create a mask which is the top nine bits of a DWORD, or 0xFF800000
  // on a big endian machine
  int shift = (sizeof(value)-1)*8-1;

  // remove all sequences of nine 0's or 1's at the start of the value
  while (shift > 0 && ((value >> shift)&0x1ff) == (value < 0 ? 0x1ff : 0))
    shift -= 8;

  return (shift+9)/8;
}

static unsigned getDataLength(const AbstractData& data);
static unsigned getObjectLength(const AbstractData& data, unsigned tag);

static unsigned getHeaderLength(unsigned tag, int l)
{
  int len = 1;

  unsigned tagVal = tag & 0x1FFFFF;

  if (tagVal >= 31)
    len += (CountBits(tagVal)+6)/7;

  if (l < 128)
    ++len;
  else
    len += (CountBits(l)+7)/8 + 1;

  return len;
}

class DataLengthCounter : public ConstVisitor
{
public:
  DataLengthCounter() : length(0) {}

  unsigned getDataLen() const { return length; }

private:
  using ConstVisitor::do_visit;
  bool do_visit(const Null& value) {
    return true;
  }
  bool do_visit(const BOOLEAN& value) {
    ++length;
    return true;
  }
  bool do_visit(const INTEGER& value) {
    length += getIntegerDataLength(value.getValue());
    return true;
  }
  bool do_visit(const ENUMERATED& value) {
    length += getIntegerDataLength(value.asInt());
    return true;
  }
  bool do_visit(const OBJECT_IDENTIFIER& value) {
    ASN1_STD vector<char> dummy;
    value.encodeCommon(dummy);
    length += dummy.size();
    return true;
  }
  bool do_visit(const BIT_STRING& value) {
    length += (value.size()+7)/8 + 1;
    return true;
  }
  bool do_visit(const OCTET_STRING& value) {
    length += value.size();
    return true;
  }
  bool do_visit(const AbstractString& value) {
    length += value.size();
    return true;
  }
  bool do_visit(const BMPString& value) {
    length += value.size()*2;
    return true;
  }
  bool do_visit(const CHOICE& value) {
    if (value.currentSelection() >=0 )
      length +=  getObjectLength(*value.getSelection(), value.getSelectionTagInternal());
    return true;
  }
  bool do_visit(const OpenData& value) {
    if(value.has_data())
      length += getObjectLength(value.get_data(), value.get_data().getTag());
    else if(value.has_buf())
      length += value.get_buf().size();
    return true;
  }
  bool do_visit(const UTCTime& value) {
    length += value.get().size();
    return true;
  }
  bool do_visit(const GeneralizedTime& value) {
    length += value.get().size();
    return true;
  }
  bool do_visit(const SEQUENCE_OF_Base& value) {
    for (SEQUENCE_OF_Base::const_iterator i = value.begin(); i != value.end(); ++i)
      length += getObjectLength(**i, InvalidTag);
    return true;
  }

  bool preVisitExtensionRoots(const SEQUENCE& value) {
    return true;
  }
  bool visitExtensionRoot(const SEQUENCE& value, int index) {
    int l = getObjectLength(*value.getField(index), value.getFieldTag(index));
    length += l;
    return true;
  }
  bool VisitExtensions(const SEQUENCE& value) {
    return true;
  }
  bool visitKnownExtension(const SEQUENCE& value, int index) {
    length += getObjectLength(*value.getField(index), value.getFieldTag(index));
    return true;
  }

  unsigned length;
};

unsigned getDataLength(const AbstractData& data)
{
  DataLengthCounter counter;
  data.accept(counter);
  return counter.getDataLen();
}

unsigned getObjectLength(const AbstractData& data, unsigned tag)
{
  int l = getDataLength(data);

  unsigned obj_tag = tag;

  if (tag==InvalidTag)
    obj_tag = data.getTag();

  if(obj_tag==InvalidTag) return l;

  if(isExplicit(obj_tag) || isExplicit(data.getTag())) {
    unsigned tag1 = isExplicit(tag) ? data.getTag() : data.getParentTag();
    if(tag1) l += getHeaderLength(tag1, l);
  }
  return l + getHeaderLength(obj_tag, l);
}

inline void BEREncoder::encodeByte(unsigned value)
{
  encodedBuffer.push_back(value);
}

inline void BEREncoder::encodeBlock(const char * bufptr, unsigned nBytes)
{
  encodedBuffer.insert(encodedBuffer.end(), bufptr, bufptr + nBytes);
}

bool BEREncoder::do_visit(const Null& value)
{
  encodeHeader(value);
  return true;
}

bool BEREncoder::do_visit(const BOOLEAN& value)
{
  encodeHeader(value);
  encodeByte(!value ? '\x00' : '\xff');
  return true;
}

bool BEREncoder::do_visit(const INTEGER& value)
{
  encodeHeader(value);
  // output the integer bits
  for (int count = getIntegerDataLength(value.getValue())-1; count >= 0; --count)
    encodeByte(value.getValue() >> (count*8));
  return true;
}

bool BEREncoder::do_visit(const ENUMERATED& value)
{
  encodeHeader(value);
  // output the integer bits
  for (int count = getIntegerDataLength(value.asInt())-1; count >= 0; --count)
    encodeByte(value.asInt() >> (count*8));
  return true;
}

bool BEREncoder::do_visit(const OBJECT_IDENTIFIER& value)
{
  encodeHeader(value);
  ASN1_STD vector<char> data;
  value.encodeCommon(data);
  if(data.size())
    encodeBlock(&data.front(), data.size());
  return true;
}

bool BEREncoder::do_visit(const BIT_STRING& value)
{
  encodeHeader(value);
  if (value.size() == 0)
    encodeByte(0);
  else {
    encodeByte(7-(value.size()-1)%8);
    if(value.getData().size())
      encodeBlock(&*value.getData().begin(), (value.size()+7)/8);
  }
  return true;
}

bool BEREncoder::do_visit(const OCTET_STRING& value)
{
  encodeHeader(value);
  if(value.size())
    encodeBlock(&value[0], value.size());
  return true;
}

bool BEREncoder::do_visit(const AbstractString& value)
{
  encodeHeader(value);
  if(value.size())
    encodeBlock((const char*)value.c_str(), value.size());
  return true;
}

bool BEREncoder::do_visit(const BMPString& value)
{
  encodeHeader(value);
  for (unsigned i = 0; i < value.size(); ++i)
  {
    encodeByte(value[i] >> 8);
    encodeByte(value[i]);
  }
  return true;
}

bool BEREncoder::do_visit(const CHOICE& value)
{
  if (value.currentSelection() != CHOICE::unselected_)
  {
    if (tag!=InvalidTag || value.getTag()!=InvalidTag)
      encodeHeader(value);

    tag = value.getSelectionTagInternal();
    return value.getSelection()->accept(*this);
  }
  return false;
}

bool BEREncoder::do_visit(const SEQUENCE_OF_Base& value)
{
  encodeHeader(value);

  for(SEQUENCE_OF_Base::const_iterator i = value.begin(); i != value.end(); ++i)
  {
    if (!(*i)->accept(*this))
      return false;
  }
  return true;
}

bool BEREncoder::do_visit(const OpenData& value)
{
  if (value.has_data()) {
    if (tag==InvalidTag)
      tag = value.getTag();

    if (tag!=InvalidTag)
      encodeHeader(value);

    return value.get_data().accept(*this);
  }

  if (value.has_buf()) {
    if(value.get_buf().size())
      encodeBlock(&*value.get_buf().begin(), value.get_buf().size());
    return true;
  }

  return false;
}

bool BEREncoder::do_visit(const UTCTime& value)
{
  encodeHeader(value);
  ASN1_STD string data(value.get());
  if(data.size())
    encodeBlock((const char*)data.c_str(), data.size());
  return true;
}

bool BEREncoder::do_visit(const GeneralizedTime& value)
{
  encodeHeader(value);
  ASN1_STD string data(value.get());
  if(data.size())
    encodeBlock((const char*)data.c_str(), data.size());
  return true;
}

bool BEREncoder::preVisitExtensionRoots(const SEQUENCE& value)
{
  encodeHeader(value);
  return true;
}

bool BEREncoder::visitExtensionRoot(const SEQUENCE& value, int index)
{
  tag = value.getFieldTag(index);
  return value.getField(index)->accept(*this);
}

bool BEREncoder::visitKnownExtension(const SEQUENCE& value, int index)
{
  tag = value.getFieldTag(index);
  return value.getField(index)->accept(*this);
}

void BEREncoder::encodeTag(unsigned a_tag)
{
  char ident = (char)(a_tag >> 24);
  unsigned tagNumber = a_tag & 0x1ffffff;

  if (tagNumber < 0x1f)
    encodeByte(ident|tagNumber);
  else {
    encodeByte(ident|0x1f);
    unsigned count = (CountBits(tagNumber)+6)/7;
    while (--count > 0)
      encodeByte((tagNumber >> (count*7))&0x7f | 0x80);
    encodeByte(tagNumber & 0x7f);
  }
}

void BEREncoder::encodeLength(unsigned len)
{
  if (len < 128)
    encodeByte(len);
  else {
    unsigned count = (CountBits(len)+7)/8;
    encodeByte(count|0x80);
    while (count-- > 0)
      encodeByte(len >> (count*8));
  }
}

void BEREncoder::encodeHeader(const AbstractData & obj)
{
  unsigned obj_tag = tag ? tag : obj.getTag();

  unsigned len = getDataLength(obj);

  if(isExplicit(obj_tag) || isExplicit(obj.getTag())) {
    unsigned tag1 = isExplicit(tag) ? obj.getTag() : obj.getParentTag();
    if(tag1) {
      obj_tag |= (1<<29);
      encodeTag(obj_tag);
      encodeLength(len + getHeaderLength(tag1, len));
      obj_tag = tag1;
    }
  }

  tag = InvalidTag;

  if(obj_tag!=InvalidTag) {
    PrimitiveChecker checker;
    if (!obj.accept(checker))
      obj_tag |= (1<<29);

    encodeTag(obj_tag);
    encodeLength(len);
  }
}

} // namespace ASN1

// <stubs>
// f1532 � 252 B � unmatched � placement: link order � name unknown
// ? f1532(i32, i32) { ... }
// </stubs>
