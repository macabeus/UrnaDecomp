/*
 * PERDecoder.cxx
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

// When resizing strings or vectors, these values will be used as upper limit.
static unsigned MaximumArraySize = 512;
static unsigned MaximumStringSize = 16*1024;

extern unsigned CountBits(UINT_TYPE range);

bool PERDecoder::decodeBitMap(ASN1_STD vector<char>& bitData, unsigned nBit)
{
  unsigned theBits;
  int idx = 0;
  unsigned bitsLeft = nBit;
  while (bitsLeft >= 8) {
    if (!decodeMultiBit(8, theBits))
      return false;

    bitData[idx++] = (unsigned char)theBits;
    bitsLeft -= 8;
  }

  if (bitsLeft > 0) {
    if (!decodeMultiBit(bitsLeft, theBits))
      return false;

    bitData[idx] = (unsigned char)(theBits << (8-bitsLeft));
  }
  return true;
}

inline bool PERDecoder::atEnd()
{
  return beginPosition >= endPosition;
}

bool PERDecoder::do_visit(Null& value)
{
  return true;
}

bool PERDecoder::do_visit(BOOLEAN& value)
{
  if (atEnd())
    return false;

  // X.931 Section 11
  value = decodeSingleBit();
  return true;
}

bool PERDecoder::do_visit(INTEGER& integer)
{
  // X.931 Sections 12

  UINT_TYPE value;
  if ((integer.getConstraintType() == ExtendableConstraint && decodeSingleBit()) ||
      integer.getConstraintType() < FixedConstraint) { //  12.1
    unsigned len;
    if (!decodeLength(0, INT_MAX, len))
      return false;

    if (!decodeMultiBit(len*8, value))
      return false;

    if (!integer.constrained() && (value & 1 << ((len*8)-1) ) ) // negative value
      value = (0xFFFFFFFF << (len*8)) | value;

    integer = value;
    return true;
  }

  if ((UINT_TYPE)integer.getLowerLimit() != integer.getUpperLimit())  // 12.2.1
  {
    if (decodeUnsigned((UINT_TYPE)integer.getLowerLimit(), (UINT_TYPE)integer.getUpperLimit(), value)) // 12.2.2 which devolves to 10.5
    {
      integer = value;
      return true;
    }
  }

  integer = integer.getLowerLimit();
  return true;
}

bool PERDecoder::do_visit(ENUMERATED& value)
{
  // X.691 Section 13

  unsigned v;
  if (value.extendable()) {  // 13.3
    if (decodeSingleBit()) {
      unsigned len = 0;
      if(decodeSmallUnsigned(len) &&
         len > 0 &&
         decodeUnsigned((unsigned)0, (unsigned)(len-1), v))
      {
        value.setFromInt(v);
        return true;
      }
    }
  }

  if (decodeUnsigned((unsigned)0, (unsigned)value.getMaximum(), v))    // 13.2
  {
    value.setFromInt(v);
    return true;
  }
  return false;
}


bool PERDecoder::do_visit(OBJECT_IDENTIFIER& value)
{
  // X.691 Section 23

  unsigned dataLen;
  if (!decodeLength(0, 255, dataLen))
    return false;

  if (getBytesLeft() < dataLen)
    return false;

  beginPosition += dataLen;

  return value.decodeCommon(beginPosition-dataLen, dataLen);
}

bool PERDecoder::do_visit(BIT_STRING& value)
{
  // X.691 Section 15

  if (!decodeConstrainedLength(value, value.totalBits))
    return false;

  if (value.totalBits > MaximumStringSize)
    return false;

  value.resize(value.totalBits);

  if (value.size() == 0)
    return true;   // 15.7

  if (value.size() > getBitsLeft())
    return false;

  if (value.size() > 16 && aligned()) {
    unsigned nBytes = (value.size()+7)/8;
    return decodeBlock(&*(value.bitData.begin()), nBytes) == nBytes;   // 15.9
  }

  return decodeBitMap( value.bitData, value.size());
}

bool PERDecoder::do_visit(OCTET_STRING& value)
{
  // X.691 Section 16

  unsigned nBytes;
  if (!decodeConstrainedLength(value, nBytes))
    return false;

  if (nBytes > MaximumStringSize)
    return false;

  value.resize(nBytes);

  if (nBytes && value.getUpperLimit() != static_cast<UINT_TYPE>(value.getLowerLimit()))
    return decodeBlock(&(*value.begin()), nBytes) == nBytes;  // 16.8

  unsigned theBits;
  switch (nBytes) {
    case 0 :  // 16.5
      break;

    case 1 :  // 16.6
      if (!decodeMultiBit(8, theBits))
        return false;

      value[0] = (char)theBits;
      break;

    case 2 :  // 16.6
      if (!decodeMultiBit(8, theBits))
        return false;
      value[0] = (char)theBits;

      if (!decodeMultiBit(8, theBits))
        return false;
      value[1] = (char)theBits;
      break;

    default: // 16.7
      return decodeBlock(&(*value.begin()), nBytes) == nBytes;
  }

  return true;
}

bool PERDecoder::do_visit(AbstractString& value)
{
  // X.691 Section 26

  unsigned len;
  if (!decodeConstrainedLength(value, len))
    return false;

  unsigned nBits = value.getNumBits(aligned());
  if (len > MaximumStringSize)
    return false;

  value.resize(len);
  if (value.getConstraintType() == Unconstrained || value.getUpperLimit()*nBits > 16) {
    if (nBits == 8)
    {
      if (decodeBlock(&(*value.begin()), len) != len)
        value.resize(0);

      return value.size() == len;
    }

    if (aligned())
      byteAlign();
  }

  unsigned i;
  for (i = 0; i < (unsigned)len; ++i) {
    unsigned theBits;
    if (!decodeMultiBit(nBits, theBits))
      return false;

    if (nBits >= value.getCanonicalSetBits() && value.getCanonicalSetBits() > 4)
      value[i] = (char)theBits;
    else if (theBits < value.getCharacterSetSize())
      value[i] = value.getCharacterSet()[theBits];
    else
      return false;
  }
  return true;
}

bool PERDecoder::do_visit(BMPString& value)
{
  // X.691 Section 26

  unsigned len;
  if (!decodeConstrainedLength(value, len))
    return false;

  if (len > MaximumStringSize)
    return false;

  value.resize(len);

  unsigned nBits = value.getNumBits(aligned());
  if ((value.getConstraintType() == Unconstrained || value.getUpperLimit()*nBits > 16) && aligned())
    byteAlign();

  for (unsigned i = 0; i < (unsigned)len; ++i) {
    unsigned theBits;
    if (!decodeMultiBit(nBits, theBits))
      return false;

    value[i] = (wchar_t)(theBits + value.getFirstChar());
  }
  return true;
}

bool PERDecoder::decodeChoicePreamle(CHOICE& value, memento_type& nextPosition)
{
  // X.691 Section 22
  if (atEnd())
    return false;

  bool ok = true;
  unsigned choiceID;

  if (value.extendable()) {
    if (decodeSingleBit()) {
      if (!decodeSmallUnsigned(choiceID))
        return false;
      choiceID += value.getNumChoices();
      unsigned len;
      if (!decodeLength(0, INT_MAX, len))
        return false;
      nextPosition.bytePosition = beginPosition + len;
      nextPosition.bitPosition = 8;
      return value.select(choiceID);
    }
  }

  if (value.getNumChoices() < 2)
    choiceID = 0;
  else {
    if (!decodeUnsigned((unsigned)0, (unsigned)(value.getNumChoices()-1), choiceID))
      return false;
  }

  nextPosition.bytePosition = 0;
  return value.select(choiceID);
}

bool PERDecoder::do_visit(CHOICE& value)
{
  memento_type memento;
  if (!decodeChoicePreamle(value, memento))
    return false;

  if (!value.isUnknownSelection() && !value.getSelection()->accept(*this))
    return false;

  rollback(memento);
  return true;
}

bool PERDecoder::do_visit(SEQUENCE_OF_Base& value)
{
  unsigned size;
  if (!decodeConstrainedLength(value, size))
    return false;

  if (size > MaximumArraySize)
    return false;

  value.resize(size);

  SEQUENCE_OF_Base::iterator it = value.begin(), last = value.end();
  for (; it != last; ++it) {
    if(!getBytesLeft())
      break;

    if (*it == 0)
      *it = value.createElement();

    if (!(*it)->accept(*this))
    {
      value.erase(it, last);
      return false;
    }
  }
  return true;
}

bool PERDecoder::do_visit(OpenData& data)
{
  OCTET_STRING value;
  if (!value.accept(*this))
    return false;

  if (!data.has_buf())
    data.grab(new OpenBuf);
  data.get_buf().swap(value);
  return true;
}

bool PERDecoder::do_revisit(OpenData& value)
{
  if (!value.has_buf() || !value.has_data())
    return false;

  PERDecoder decoder(&(*value.get_buf().begin()), value.get_buf().size(), get_env());
  return value.get_data().accept(decoder);
}

bool PERDecoder::do_visit(TypeConstrainedOpenData& value)
{
  assert(value.has_data());
  unsigned len;
  if (!decodeLength(0, INT_MAX, len))
    return false;

  const char* nextPosition = beginPosition + len;
  bool ok = value.get_data().accept(*this);
  setPosition(nextPosition);
  return ok;
}

bool PERDecoder::do_visit(UTCTime& value)
{
  unsigned len;
  if (decodeLength(0, INT_MAX, len))
  {
    ASN1_STD vector<char> data(len);
    if (decodeBlock(&data[0], len))
    {
      value.set(&data[0]);
      return true;
    }
  }
  return false;
}

bool PERDecoder::do_visit(GeneralizedTime& value)
{
  unsigned len;
  if (decodeLength(0, INT_MAX, len))
  {
    ASN1_STD vector<char> data(len);
    if (decodeBlock(&data[0], len))
    {
      value.set(&data[0]);
      return true;
    }
  }
  return false;
}

Visitor::VISIT_SEQ_RESULT PERDecoder::preVisitExtensionRoots(SEQUENCE& value)
{
  // X.691 Section 18

  bool hasExtension = false;
  if (value.extendable()) {
    if (atEnd())
      return FAIL;

    hasExtension = decodeSingleBit() ;  // 18.1
  }
  return decodeBitMap(value.optionMap.bitData, value.optionMap.size()) ?
               (hasExtension  ? CONTINUE : NO_EXTENSION) : FAIL;  // 18.2
}

Visitor::VISIT_SEQ_RESULT PERDecoder::visitExtensionRoot(SEQUENCE& value, int index, int optional_id)
{
  if (optional_id == SEQUENCE::mandatory_ || value.hasOptionalField(optional_id))
  {
    if (value.getField(index) == 0)
      value.fields[index] = AbstractData::create(value.info()->fieldInfos[index]);

    if (value.getField(index) == 0 || !value.getField(index)->accept(*this))
      return FAIL;
  }
  return CONTINUE;
}

Visitor::VISIT_SEQ_RESULT PERDecoder::preVisitExtensions(SEQUENCE& value)
{
  unsigned totalExtensions;
  if (!decodeSmallUnsigned(totalExtensions))
    return FAIL;

  ++totalExtensions;
  if (totalExtensions > MaximumArraySize)
    return FAIL;

  value.extensionMap.resize(totalExtensions);
  Visitor::VISIT_SEQ_RESULT result;
  result = decodeBitMap(value.extensionMap.bitData, value.extensionMap.size()) ? CONTINUE : FAIL;
  return result;
}

Visitor::VISIT_SEQ_RESULT PERDecoder::visitKnownExtension(SEQUENCE& value, int index, int optional_id)
{
  if (!value.hasOptionalField(optional_id))
    return CONTINUE;

  unsigned len;
  if (!decodeLength(0, INT_MAX, len))
    return FAIL;

  const char* nextExtensionPosition = beginPosition + len;
  if (value.getField(index) == 0)
    value.fields[index] = AbstractData::create(value.info()->fieldInfos[index]);

  bool ok = value.getField(index) == 0 || value.getField(index)->accept(*this);
  setPosition(nextExtensionPosition);
  return ok ? CONTINUE : FAIL;
}

bool PERDecoder::visitUnknownExtensions(SEQUENCE& value)
{
  if (value.extensionMap.size() <= value.info()->knownExtensions)
      return true; // Already read them

  for (unsigned i = value.info()->knownExtensions; i < (unsigned)value.extensionMap.size(); ++i) {
    if (!value.extensionMap[i])
      continue;

    unsigned len;
    if (!decodeLength(0, INT_MAX, len))
      return false;

    if (getBytesLeft() < len)
      return false;

    beginPosition += len;
  }

  return true;
}

void PERDecoder::setPosition(const char* newPos)
{
  beginPosition = newPos < endPosition ? newPos : endPosition;
  bitOffset = 8;
}

void PERDecoder::byteAlign()
{
  if (bitOffset != 8) {
    bitOffset = 8;
    ++beginPosition;
  }
}

unsigned PERDecoder::getBitsLeft() const
{
  return (endPosition - beginPosition)*8 - (8 - bitOffset);
}

unsigned PERDecoder::getBytesLeft() const
{
  return beginPosition < endPosition ? endPosition - beginPosition : 0;
}

bool PERDecoder::decodeConstrainedLength(ConstrainedObject & obj, unsigned & length)
{
  // The execution order is important in the following. The decodeSingleBit() function
  // must be called if extendableFlag is true, no matter what.
  if ((obj.getConstraintType() == ExtendableConstraint && decodeSingleBit()) ||
      obj.getConstraintType() == Unconstrained)
    return decodeLength(0, INT_MAX, length);

  return decodeLength(obj.getLowerLimit(), obj.getUpperLimit(), length);
}

bool PERDecoder::decodeSingleBit()
{
  if (getBitsLeft() == 0)
    return false;

  --bitOffset;
  bool value = (*beginPosition & (1 << bitOffset)) != 0;

  if (bitOffset == 0) {
    byteAlign();
  }

  return value;
}

bool PERDecoder::decodeSmallUnsigned(unsigned & value)
{
  // X.691 Section 10.6

  if (!decodeSingleBit())
    return decodeMultiBit(6, value);      // 10.6.1

  unsigned len;
  if (!decodeLength(0, INT_MAX, len))  // 10.6.2
    return false;

  byteAlign();
  return decodeMultiBit(len*8, value);
}


bool PERDecoder::decodeLength(unsigned lower, unsigned upper, unsigned & len)
{
  // X.691 section 10.9

  if (upper != INT_MAX && !alignedFlag) {
    assert(upper - lower < 0x10000);  // 10.9.4.2 unsupported
    unsigned base;
    if (!decodeMultiBit(CountBits(upper - lower + 1), base))
      return false;
  }

  if (upper < 65536)  // 10.9.3.3
    return decodeUnsigned(lower, upper, len);

  // 10.9.3.5
  byteAlign();
  if (atEnd())
    return false;

  if (!decodeSingleBit())
    return decodeMultiBit(7, len);

  if (!decodeSingleBit())
    return decodeMultiBit(14, len);

  // 10.9.3.8 unsupported
  return false;
}

unsigned PERDecoder::decodeBlock(char * bufptr, unsigned nBytes)
{
  if (nBytes == 0)
    return 0;

  byteAlign();

  if (beginPosition+nBytes > endPosition)
    nBytes = endPosition - beginPosition;

  if (nBytes == 0)
    return 0;

  memcpy(bufptr, beginPosition, nBytes);

  beginPosition += nBytes;
  return nBytes;
}

} // namespace ASN1
