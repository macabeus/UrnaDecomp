/*
 * PEREncoder.cxx
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

unsigned CountBits(UINT_TYPE range);

void PEREncoder::encodeBitMap(const ASN1_STD vector<char>& bitData, unsigned nBits)
{
  int idx = 0;
  unsigned bitsLeft = nBits;
  while (bitsLeft >= 8) {
    encodeMultiBit((unsigned)bitData[idx++],8);
    bitsLeft -= 8;
  }

  if (bitsLeft > 0)
    encodeMultiBit((unsigned)(bitData[idx] >> (8 - bitsLeft)), bitsLeft);
}

inline void PEREncoder::byteAlign()
{
  if (bitOffset != 8)
    bitOffset = 8;
}

bool PEREncoder::do_visit(const Null&)
{
  return true;
}

bool PEREncoder::do_visit(const BOOLEAN& value)
{
  // X.931 Section 11
  encodeSingleBit((bool)value);
  return true;
}

bool PEREncoder::do_visit(const INTEGER& integer)
{
  // X.931 Sections 12

  UINT_TYPE value = (UINT_TYPE) integer.getValue();
  if (encodeConstraint(integer, (INT_TYPE)value)) { //  12.1
    unsigned nBytes;
    UINT_TYPE adjusted_value = (integer.getConstraintType() == FixedConstraint ?
                value - integer.getLowerLimit() : value);
    unsigned nBits;
    if (adjusted_value == 0)
      nBits = 1;
    else if (integer.getConstraintType() != FixedConstraint )
    {
      if ((INT_TYPE)value < 0)
        nBits = CountBits( (~value) +1);
      else
        nBits = CountBits(value+1) +1;
    }
    else
      nBits = CountBits(adjusted_value+1);

    nBytes = (nBits+7)/8;
    encodeLength(nBytes, 0, INT_MAX);
    encodeMultiBit(adjusted_value, nBytes*8);
    return true;
  }

  if (static_cast<UINT_TYPE>(integer.getLowerLimit()) == integer.getUpperLimit()) // 12.2.1
    return true;

  // 12.2.2 which devolves to 10.5
  return encodeUnsigned((UINT_TYPE)value, (UINT_TYPE)integer.getLowerLimit(), integer.getUpperLimit());
}

bool PEREncoder::do_visit(const ENUMERATED& value)
{
  if (value.extendable()) {  // 13.3
    bool extended = value.asInt() > value.getMaximum();
    encodeSingleBit(extended);
    if (extended) {
      encodeSmallUnsigned(1+value.asInt());
      return encodeUnsigned((unsigned)value.asInt(), (unsigned)0, (unsigned)value.asInt());
    }
  }
  return encodeUnsigned((unsigned)value.asInt(), (unsigned)0, (unsigned)value.getMaximum());  // 13.2
}


bool PEREncoder::do_visit(const OBJECT_IDENTIFIER& value)
{
  // X.691 Section 23

  ASN1_STD vector<char> eObjId;
  value.encodeCommon(eObjId);
  if (!encodeLength(eObjId.size(), 0, 255))
    return false;

  if(eObjId.size())
    encodeBlock(&eObjId.front(), eObjId.size());
  return true;
}

bool PEREncoder::do_visit(const BIT_STRING& value)
{
  // X.691 Section 15

  if (!encodeConstrainedLength(value, value.size())) return false;

  if (value.size() == 0) return true;

  if (value.size() > 16 && aligned())
    encodeBlock(&*value.getData().begin(), (value.size()+7)/8);   // 15.9
  else // 15.8
    encodeBitMap(value.getData(), value.size());

  return true;
}

bool PEREncoder::do_visit(const OCTET_STRING& value)
{
  // X.691 Section 16
  unsigned nBytes = value.size();

  if (!encodeConstrainedLength(value, nBytes))
    return false;

  if (nBytes && value.getUpperLimit() != static_cast<UINT_TYPE>(value.getLowerLimit()))
  {
    encodeBlock(&*value.begin(), nBytes);  // 16.8
    return true;
  }

  switch (nBytes) {
    case 0 :  // 16.5
      break;

    case 1 :  // 16.6
      encodeMultiBit((unsigned)value[0], 8);
      break;

    case 2 :  // 16.6
      encodeMultiBit((unsigned)value[0], 8);
      encodeMultiBit((unsigned)value[1], 8);
      break;

    default: // 16.7
      encodeBlock(&*value.begin(), nBytes);
  }
  return true;
}


bool PEREncoder::do_visit(const AbstractString& value)
{
  // X.691 Section 26

  unsigned len = value.size();
  if (!encodeConstrainedLength(value, len))
    return false;

  unsigned nBits = value.getNumBits(aligned());

  if (value.getConstraintType() == Unconstrained || value.getUpperLimit()*nBits > 16) {
    if (nBits == 8) {
      encodeBlock(value.c_str(), len);
      return true;
    }
    if (aligned())
      byteAlign();
  }

  for (unsigned i = 0; i < len; i++) {
    if (nBits >= value.getCanonicalSetBits() && value.getCanonicalSetBits() > 4)
      encodeMultiBit((unsigned)value[i], nBits);
    else {
      const void * ptr = memchr(value.getCharacterSet(), value[i], value.getCharacterSetSize());
      if (!ptr) return false;

      unsigned pos = ((const char *)ptr - value.getCharacterSet());
      encodeMultiBit(pos, nBits);
    }
  }
  return true;
}

bool PEREncoder::do_visit(const BMPString& value)
{
  // X.691 Section 26

  unsigned len = value.size();
  if (!encodeConstrainedLength(value, len))
    return false;

  unsigned nBits = value.getNumBits(aligned());

  if ((value.getConstraintType() == Unconstrained || value.getUpperLimit()*nBits > 16) && aligned())
    byteAlign();

  for (unsigned i = 0; i < len; i++)
    encodeMultiBit((unsigned)(value[i] - value.getFirstChar()), nBits);

  return true;
}

bool PEREncoder::do_visit(const CHOICE& value)
{
  if (value.currentSelection() < 0)
    return false;

  if (value.extendable()) {
    bool extended = value.currentSelection() >= static_cast<int>(value.getNumChoices());
    encodeSingleBit(extended);
    if (extended) {
      encodeSmallUnsigned(value.currentSelection() - value.getNumChoices());
      return encodeAnyType(value.getSelection());
    }
  }

  if (value.getNumChoices() > 1)
    if (!encodeUnsigned((unsigned)value.currentSelection(), (unsigned)0, (unsigned)(value.getNumChoices()-1)))
      return false;

  return value.getSelection()->accept(*this);
}

bool PEREncoder::do_visit(const SEQUENCE_OF_Base& value)
{
  unsigned sz = value.size();
  if (!encodeConstrainedLength(value, sz))
    return false;

  SEQUENCE_OF_Base::const_iterator first = value.begin(), last = value.end();
  for (; first != last; ++first)
    if (!(*first)->accept(*this))
      return false;
  return true;
}

bool PEREncoder::do_visit(const OpenData& value)
{
  return encodeAnyType(&value.get_data());
}

bool PEREncoder::do_visit(const GeneralizedTime& value)
{
  ASN1_STD string notion(value.get());
  encodeLength(notion.size(), 0, UINT_MAX);
  if(notion.size())
    encodeBlock(&*notion.begin(), notion.size());
  return true;
}

bool PEREncoder::do_visit(const UTCTime& value)
{
  ASN1_STD string notion(value.get());
  encodeLength(notion.size(), 0, UINT_MAX);
  if(notion.size())
    encodeBlock(&*notion.begin(), notion.size());
  return true;
}

bool PEREncoder::preVisitExtensionRoots(const SEQUENCE& value)
{
  // X.691 Section 18
  if (value.extendable()) {
    bool hasExtensions = false;
    for (unsigned i = 0; i < value.extensionMap.size(); i++) {
      if (value.extensionMap[i]) {
        hasExtensions = true;
        break;
      }
    }
    encodeSingleBit(hasExtensions);  // 18.1
  }
  encodeBitMap(value.optionMap.bitData, value.optionMap.size());// 18.2
  return true;
}

bool PEREncoder::visitExtensionRoot(const SEQUENCE& value, int index)
{
  return value.fields[index]->accept(*this);
}

bool PEREncoder::preVisitExtensions(const SEQUENCE& value)
{
  int totalExtensions = value.extensionMap.size();
  encodeSmallUnsigned(totalExtensions-1);
  encodeBitMap(value.extensionMap.bitData, value.extensionMap.size());
  return true;
}

bool PEREncoder::visitKnownExtension(const SEQUENCE& value, int index)
{
  return encodeAnyType(value.fields[index]);
}

bool PEREncoder::encodeConstrainedLength(const ConstrainedObject & obj, unsigned length)
{
  return (encodeConstraint(obj, length) ? // 26.4
          encodeLength(length, 0, INT_MAX) :
          encodeLength(length, obj.getLowerLimit(), obj.getUpperLimit()));
}

bool PEREncoder::encodeConstraint(const ConstrainedObject & obj, UINT_TYPE value)
{
  if (!obj.extendable())
    return obj.getConstraintType() != FixedConstraint;

  bool needsExtending = value > obj.getUpperLimit();

  if (!needsExtending) {
    if (obj.getLowerLimit() < 0) {
      if ((INT_TYPE)value < obj.getLowerLimit())
        needsExtending = true;
    }
    else {
      if (value < (UINT_TYPE)obj.getLowerLimit())
        needsExtending = true;
    }
  }

  encodeSingleBit(needsExtending);

  return needsExtending || obj.getConstraintType() < FixedConstraint;
}

void PEREncoder::encodeSingleBit(bool value)
{
  if (bitOffset == 8)
    encodedBuffer.push_back(0);

  --bitOffset;

  if (value)
    encodedBuffer.back() |= 1 << bitOffset;

  if (bitOffset == 0)
    byteAlign();
}

void PEREncoder::encodeSmallUnsigned(unsigned value)
{
  if (value < 64) {
    encodeMultiBit(value, 7);
    return;
  }

  encodeSingleBit(true);// 10.6.2

  unsigned len = 4;
  if (value < 256)
    len = 1;
  else if (value < 65536)
    len = 2;
  else if (value < 0x1000000)
    len = 3;
  encodeLength(len, 0, INT_MAX);  // 10.9
  byteAlign();
  encodeMultiBit(value, len*8);
}

bool PEREncoder::encodeLength(unsigned len, unsigned lower, unsigned upper)
{
  if (len < lower || len > upper)
    return false;

  // X.691 section 10.9

  if (upper != INT_MAX && !alignedFlag) {
    if (upper - lower >= 0x10000)  // 10.9.4.2 unsupperted
      return false;

    encodeMultiBit(len - lower, CountBits(upper - lower + 1));   // 10.9.4.1
    return true;
  }

  if (upper < 65536)  // 10.9.3.3
    return encodeUnsigned(len, lower, upper);

  byteAlign();

  if (len < 128) {
    encodeMultiBit(len, 8);   // 10.9.3.6
    return true;
  }

  encodeSingleBit(true);

  if (len < 0x2000) {
    encodeMultiBit(len, 15);    // 10.9.3.7
    return true;
  }

  encodeSingleBit(true);
  if (len >= 0x2000)  // 10.9.3.8 unsupported
    return false;

  return true;
}

bool PEREncoder::encodeAnyType(const AbstractData * value)
{
  OpenBuf buf;
  PEREncoder subEncoder(buf);

  if (value != 0)
    if (!value->accept(subEncoder))
      return false;

  if (buf.size() == 0)           // Make sure extension has at least one
    subEncoder.encodeSingleBit(false); // byte in its ANY type encoding.

  unsigned nBytes = buf.size();
  encodeLength(nBytes, 0, INT_MAX);
  if(nBytes)
    encodeBlock(&*buf.begin(), nBytes);
  return true;
}

void PEREncoder::encodeByte(unsigned value)
{
  byteAlign();
  encodedBuffer.push_back(value);
}

void PEREncoder::encodeBlock(const char * bufptr, unsigned nBytes)
{
  if (!nBytes) return;

  byteAlign();
  encodedBuffer.insert(encodedBuffer.end(), bufptr, bufptr + nBytes);
}

} // namespace ASN1

// <stubs>
// f3464 � 291 B � unmatched � placement: link order � name unknown
// ? f3464(i32, i32) { ... }
// </stubs>
