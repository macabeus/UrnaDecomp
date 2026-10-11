/*
 * asn1.cpp
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
 *
 * - support proper DER encoding
 * - remove dependency from BOOST libraries
 * - ASN1_HAS_IOSTREAM is no longer used
 * - ASN1_STD definition is used across all sources in order to support
 *   compilers without proper std namespace (f.i., on HPUX +AP - HP_aCC_RW has to be defined)
 * - Ability to use longer integers (64-bit) added (ASN_USE_LONG has to be defined)
 */

#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "asn1.h"
#include <algorithm>

#if defined(HP_aCC_RW)
#  include <iomanip.h>
#else
#  include <iomanip>
#endif

#include <typeinfo>
#include <cstring>
#include <limits.h>
#include <cstdio>

namespace ASN1 {

unsigned CountBits(UINT_TYPE range)
{
  if (range == 0) return sizeof(UINT_TYPE)*8;

  unsigned nBits = 0;
  while (nBits < (sizeof(UINT_TYPE)*8) && range > (UINT_TYPE)(1 << nBits))
    ++nBits;

  return nBits;
}

int lexicographic_compare_bytes(const char* first1, size_t size1,
                const char* first2, size_t size2)
{
  if(size1 > size2)
    return 1;
  if(size1 < size2)
    return -1;

  int r;
  for (; size1; ++first1, --size1, ++first2)
  {
    r = *first1 - *first2;
    if (r != 0) return r;
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////

AbstractData::AbstractData(const void* information)
: info_(information)
{
}

////////////////////////////////////////////////////////////

ASN1_API const Null::InfoType Null::theInfo = {
    Null::create,
    UniversalTagClass << 30 | UniversalNull,
    0
};

AbstractData* Null::create(const void* info)
{
  return new Null(info);
}

bool Null::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool Null::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

AbstractData * Null::do_clone() const
{
  assert(typeid(*this) == typeid(Null));
  return new Null(*this);
}

INT_TYPE Null::do_compare(const AbstractData& other) const
{
  assert(typeid(other) == typeid(Null));
  return 0;
}

/////////////////////////////////////////////////////////

ASN1_API const BOOLEAN::InfoType BOOLEAN::theInfo = {
  BOOLEAN::create,
  UniversalTagClass << 30 | UniversalBoolean,
  0
};

BOOLEAN::BOOLEAN(const void* info)
: AbstractData(info), value(false)
{
}

BOOLEAN::BOOLEAN(bool b, const void* info)
: AbstractData(info),value(b)
{
}

BOOLEAN::BOOLEAN(const BOOLEAN& that)
: AbstractData(that), value(that.value)
{
}

AbstractData* BOOLEAN::create(const void* info)
{
  return new BOOLEAN(info);
}

bool BOOLEAN::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool BOOLEAN::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

AbstractData * BOOLEAN::do_clone() const
{
  assert(typeid(*this) == typeid(BOOLEAN));
  return new BOOLEAN(*this);
}

INT_TYPE BOOLEAN::do_compare(const AbstractData& data) const
{
  return value - dynamic_cast<const BOOLEAN&>(data);
}

///////////////////////////////////////////////////////////////////////

ASN1_API const INTEGER::InfoType INTEGER::theInfo = {
  &INTEGER::create,
  UniversalTagClass << 30 | UniversalInteger,
  0,
  Unconstrained,
  0,
  UINT_MAX
};

INTEGER::INTEGER(const void* info)
: ConstrainedObject(info), value(0)
{
}

INTEGER::INTEGER(int_type val, const void* info)
: ConstrainedObject(info), value(val)
{
}

INTEGER::INTEGER(const INTEGER& other)
: ConstrainedObject(other), value(other.value)
{
}

AbstractData* INTEGER::create(const void* info)
{
  return new INTEGER(info);
}

bool INTEGER::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool INTEGER::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

AbstractData * INTEGER::do_clone() const
{
  return new INTEGER(*this);
}

INT_TYPE INTEGER::do_compare(const AbstractData& data) const
{
  const INTEGER& that = dynamic_cast<const INTEGER&>(data);
  if (getLowerLimit() >= 0)
    return value - that.value;

  return (INT_TYPE) value - (INT_TYPE) that.value;
}

///////////////////////////////////////////////////////
IntegerWithNamedNumber::IntegerWithNamedNumber(const void* info)
: INTEGER(info)
{
}

IntegerWithNamedNumber::IntegerWithNamedNumber(INT_TYPE val, const void* info)
: INTEGER(val, info)
{
}

AbstractData* IntegerWithNamedNumber::create(const void* info)
{
  return new IntegerWithNamedNumber(info);
}

bool IntegerWithNamedNumber::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool IntegerWithNamedNumber::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

//////////////////////////////////////////////////////
ENUMERATED::ENUMERATED(const void* info)
: AbstractData(info), value(0)
{
    assert(getMaximum() >= 0);
}

ENUMERATED::ENUMERATED(const ENUMERATED& that)
: AbstractData(that), value(that.value)
{
}

ENUMERATED& ENUMERATED::operator = (const ENUMERATED& that)
{
  assert(info() == that.info()); // compatible type check
  value = that.value;
  return *this;
}

bool ENUMERATED::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool ENUMERATED::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

AbstractData * ENUMERATED::do_clone() const
{
  return new ENUMERATED(*this);
}

INT_TYPE ENUMERATED::do_compare(const AbstractData& other) const
{
  const ENUMERATED& that = *static_cast<const ENUMERATED*>(&other);
  assert(info_ == that.info_); // compatible type check
  return value - that.value;
}

AbstractData* ENUMERATED::create(const void* info)
{
  return new ENUMERATED(info);
}

///////////////////////////////////////////////////////////////////////

ASN1_API const OBJECT_IDENTIFIER::InfoType OBJECT_IDENTIFIER::theInfo = {
    OBJECT_IDENTIFIER::create,
    UniversalTagClass << 30 | UniversalObjectId,
    0
};

OBJECT_IDENTIFIER::OBJECT_IDENTIFIER(const OBJECT_IDENTIFIER & other)
: AbstractData(other), value(other.value)
{
}

OBJECT_IDENTIFIER::OBJECT_IDENTIFIER(unsigned nelem, ...)
: AbstractData(&theInfo)
{
  va_list lst;
  va_start(lst, nelem);
  assign(nelem, lst);
  va_end(lst);
}

void OBJECT_IDENTIFIER::assign(unsigned nelem, ... /*list of unsigned*/)
{
  va_list lst;
  va_start(lst, nelem);
  assign(nelem, lst);
  va_end(lst);
}

void OBJECT_IDENTIFIER::assign(unsigned nelem, va_list lst)
{
  value.resize(nelem, 0);
  for(unsigned i=0; i<nelem; ++i)
    value[i] = va_arg(lst, unsigned);
}

AbstractData* OBJECT_IDENTIFIER::create(const void* info)
{
  return new OBJECT_IDENTIFIER(info);
}

bool OBJECT_IDENTIFIER::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool OBJECT_IDENTIFIER::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

bool OBJECT_IDENTIFIER::decodeCommon(const char* strm, unsigned dataLen)
{
  unsigned byteOffset=0;

  value.clear();

  // handle zero length strings correctly
  if (dataLen == 0) return true;

  unsigned subId;

  // Avoid reallocations in the while-loop below.
  value.reserve(dataLen+1);

  // start at the second identifier in the buffer, because we will later
  // expand the first number into the first two IDs
  value.push_back(0);

  while (dataLen > 0) {
    unsigned byte;
    subId = 0;
    do {    /* shift and add in low order 7 bits */
      if (dataLen == 0)
        return false;

      byte = strm[byteOffset++];
      subId = (subId << 7) + (byte & 0x7f);
      --dataLen;
    } while ((byte & 0x80) != 0);
    value.push_back(subId);
  }

  /*
   * The first two subidentifiers are encoded into the first component
   * with the value (X * 40) + Y, where:
   *  X is the value of the first subidentifier.
   *  Y is the value of the second subidentifier.
  */
  subId = value[1];
  if (subId < 40) {
    value[0] = 0;
    value[1] = subId;
  }
  else if (subId < 80) {
    value[0] = 1;
    value[1] = subId-40;
  }
  else {
    value[0] = 2;
    value[1] = subId-80;
  }

  return true;
}

void OBJECT_IDENTIFIER::encodeCommon(ASN1_STD vector<char> & encodecObjectId) const
{
  unsigned length = value.size();
  if (length < 2) {
    // Thise case is really illegal, but we have to do SOMETHING
    encodecObjectId.resize(0);
    return;
  }

  const unsigned* objId = &value[0];

  unsigned subId = (objId[0] * 40) + objId[1];
  objId += 2;

  unsigned outputPosition = 0;
  encodecObjectId.reserve(length);
  ASN1_STD insert_iterator<ASN1_STD vector<char> > insertItr(encodecObjectId, encodecObjectId.begin());
  while (--length > 0) {
    if (subId < 128)
      *insertItr++ = (char)subId;
    else {
      unsigned mask = 0x7F; /* handle subid == 0 case */
      int bits = 0;

      /* testmask *MUST* !!!! be of an unsigned type */
      unsigned testmask = 0x7F;
      int      testbits = 0;
      while (testmask != 0) {
        if (subId & testmask) {  /* if any bits set */
          mask = testmask;
          bits = testbits;
        }
        testmask <<= 7;
        testbits += 7;
      }

      // mask can't be zero here
      while (mask != 0x7F) {
        // fix a mask that got truncated above
        if (mask == 0x1E00000)
          mask = 0xFE00000;

        *insertItr++ = (char)(((subId & mask) >> bits) | 0x80);

        mask >>= 7;
        bits -= 7;
      }

      *insertItr++ = (char)(subId & mask);
    }
    if (length >1)
      subId = *objId++;
  }
}

AbstractData * OBJECT_IDENTIFIER::do_clone() const
{
  return new OBJECT_IDENTIFIER(*this);
}

//! \note this addition to address issue with compiling using VS6 (incorrect headers: min() is not in namespace 'std')
template <typename T>
T MIN_ASN1(T a, T b)
{
  return a < b ? a : b;
}

INT_TYPE OBJECT_IDENTIFIER::do_compare(const AbstractData& other) const
{
  const OBJECT_IDENTIFIER& that = dynamic_cast<const OBJECT_IDENTIFIER&>(other);
  int min_level = MIN_ASN1(levels(), that.levels());
  for (int i = 0; i < min_level; ++i)
    if (value[i] != that.value[i])
      return value[i] - that.value[i];

  return levels() - that.levels();
}

///////////////////////////////////////////////////////////////////////

ASN1_API const BIT_STRING::InfoType BIT_STRING::theInfo = {
  BIT_STRING::create,
  UniversalTagClass << 30 | UniversalBitString,
  0,
  Unconstrained,
  0,
  UINT_MAX,
  0,
  0
};


BIT_STRING::BIT_STRING(const void* info)
: ConstrainedObject(info), totalBits(0)
{
}

BIT_STRING::BIT_STRING(const BIT_STRING & other)
 : ConstrainedObject(other), totalBits(other.totalBits), bitData(other.bitData)
{
}

AbstractData* BIT_STRING::create(const void* info)
{
  return new BIT_STRING(info);
}

bool BIT_STRING::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool BIT_STRING::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

AbstractData* BIT_STRING::do_clone() const
{
  return new BIT_STRING(*this);
}

INT_TYPE BIT_STRING::do_compare(const AbstractData& other) const
{
  const BIT_STRING& that = dynamic_cast<const BIT_STRING&>(other);
  int nBytes = MIN_ASN1(bitData.size(), that.bitData.size());
  for (int i = 0 ; i < nBytes; ++i)
  {
    char mask = bitData[i] ^ that.bitData[i]; // find the first byte which differs
    if (mask != 0)
      return (bitData[i] & mask) - (that.bitData[i] & mask);
  }
  return totalBits - that.totalBits;
}

///////////////////////////////////////////////////////////////////////
ASN1_API const OCTET_STRING::InfoType OCTET_STRING::theInfo = {
  OCTET_STRING::create,
  UniversalTagClass << 30 | UniversalOctetString,
  0,
  Unconstrained,
  0,
  UINT_MAX
};

AbstractData* OCTET_STRING::create(const void* info)
{
    return new OCTET_STRING(info);
}

bool OCTET_STRING::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool OCTET_STRING::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

AbstractData * OCTET_STRING::do_clone() const
{
  return new OCTET_STRING(*this);
}

INT_TYPE OCTET_STRING::do_compare(const AbstractData& other) const
{
  const OCTET_STRING& that = dynamic_cast<const OCTET_STRING&>(other);
  return lexicographic_compare_bytes(&(*begin()), size(), &(*that.begin()), that.size());
}

///////////////////////////////////////////////////////////////////////
AbstractString::AbstractString(const AbstractString& other)
: ConstrainedObject(other.info_), base_string(other)
{
}

AbstractString::AbstractString(const void* info)
: ConstrainedObject(info)
{
}

AbstractString::AbstractString(const void* info, const ASN1_STD string& str)
: ConstrainedObject(info), base_string(str)
{
}

AbstractString::AbstractString(const void* info, const char* str)
: ConstrainedObject(info), base_string(str)
{
}

bool AbstractString::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool AbstractString::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

AbstractData* AbstractString::do_clone() const
{
  return new AbstractString(*this);
}

INT_TYPE AbstractString::do_compare(const AbstractData& other) const
{
  const AbstractString& that = dynamic_cast<const AbstractString&>(other);
  return base_string::compare(that);
}

AbstractData* AbstractString::create(const void* info)
{
  return new AbstractString(info);
}

/////////////////////////////////////////////////////////////////////////////

static const char NumericStringSet[]   =
  " 0123456789";

static const char PrintableStringSet[] =
  " '()+,-./0123456789:=?"
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz";

static const char VisibleStringSet[]   =
  " !\"#$%&'()*+,-./0123456789:;<=>?"
  "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
  "`abcdefghijklmnopqrstuvwxyz{|}~";

static const char IA5StringSet[]       =
  "\000\001\002\003\004\005\006\007"
  "\010\011\012\013\014\015\016\017"
  "\020\021\022\023\024\025\026\027"
  "\030\031\032\033\034\035\036\037"
  " !\"#$%&'()*+,-./0123456789:;<=>?"
  "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
  "`abcdefghijklmnopqrstuvwxyz{|}~\177";

static const char GraphicStringSet[]   =
  " !\"#$%&'()*+,-./0123456789:;<=>?"
  "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
  "`abcdefghijklmnopqrstuvwxyz{|}~\177"
  "\200\201\202\203\204\205\206\207"
  "\210\211\212\213\214\215\216\217"
  "\220\221\222\223\224\225\226\227"
  "\230\231\232\233\234\235\236\237"
  "\240\241\242\243\244\245\246\247"
  "\250\251\252\253\254\255\256\257"
  "\260\261\262\263\264\265\266\267"
  "\270\271\272\273\274\275\276\277"
  "\300\301\302\303\304\305\306\307"
  "\310\311\312\313\314\315\316\317"
  "\320\321\322\323\324\325\326\327"
  "\330\331\332\333\334\335\336\337"
  "\340\341\342\343\344\345\346\347"
  "\350\351\352\353\354\355\356\357"
  "\360\361\362\363\364\365\366\367"
  "\370\371\372\373\374\375\376\377";

static const char GeneralStringSet[]   =
  "\000\001\002\003\004\005\006\007"
  "\010\011\012\013\014\015\016\017"
  "\020\021\022\023\024\025\026\027"
  "\030\031\032\033\034\035\036\037"
  " !\"#$%&'()*+,-./0123456789:;<=>?"
  "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
  "`abcdefghijklmnopqrstuvwxyz{|}~\177"
  "\200\201\202\203\204\205\206\207"
  "\210\211\212\213\214\215\216\217"
  "\220\221\222\223\224\225\226\227"
  "\230\231\232\233\234\235\236\237"
  "\240\241\242\243\244\245\246\247"
  "\250\251\252\253\254\255\256\257"
  "\260\261\262\263\264\265\266\267"
  "\270\271\272\273\274\275\276\277"
  "\300\301\302\303\304\305\306\307"
  "\310\311\312\313\314\315\316\317"
  "\320\321\322\323\324\325\326\327"
  "\330\331\332\333\334\335\336\337"
  "\340\341\342\343\344\345\346\347"
  "\350\351\352\353\354\355\356\357"
  "\360\361\362\363\364\365\366\367"
  "\370\371\372\373\374\375\376\377";

ASN1_API const NumericString::InfoType NumericString::theInfo = {
  AbstractString::create,
  UniversalTagClass << 30 | UniversalNumericString,
  0,
  Unconstrained,
  0,
  UINT_MAX,
  NumericStringSet,
  11,
  4,
  4,
  4
};

ASN1_API const PrintableString::InfoType PrintableString::theInfo = {
  AbstractString::create,
  UniversalTagClass << 30 | UniversalPrintableString,
  0,
  Unconstrained,
  0,
  UINT_MAX,
  PrintableStringSet,
  74,
  7,
  7,
  8
};

ASN1_API const VisibleString::InfoType VisibleString::theInfo = {
  AbstractString::create,
  UniversalTagClass << 30 | UniversalVisibleString,
  0,
  Unconstrained,
  0,
  UINT_MAX,
  VisibleStringSet,
  95,
  7,
  7,
  8
};

ASN1_API const IA5String::InfoType IA5String::theInfo = {
  AbstractString::create,
  UniversalTagClass << 30 | UniversalIA5String,
  0,
  Unconstrained,
  0,
  UINT_MAX,
  IA5StringSet,
  128,
  7,
  7,
  8
};

ASN1_API const GraphicString::InfoType GraphicString::theInfo = {
  AbstractString::create,
  UniversalTagClass << 30 | UniversalGraphicString,
  0,
  Unconstrained,
  0,
  UINT_MAX,
  GraphicStringSet,
  sizeof(GraphicStringSet)-1,
  8,
  8,
  8
};

ASN1_API const GeneralString::InfoType GeneralString::theInfo = {
  AbstractString::create,
  UniversalTagClass << 30 | UniversalGeneralString,
  0,
  Unconstrained,
  0,
  UINT_MAX,
  GeneralStringSet,
  256,
  8,
  8,
  8
};

///////////////////////////////////////////////////////////////////////

ASN1_API const BMPString::InfoType BMPString::theInfo = {
  BMPString::create,
  UniversalTagClass << 30 | UniversalBMPString,
  0,
  Unconstrained,
  0,
  UINT_MAX,
  0,
  0xffff,
  16,
  16
};

BMPString::BMPString(const void* info)
: ConstrainedObject(info)
{
}

BMPString::BMPString()
: ConstrainedObject(&theInfo)
{
}

BMPString::BMPString(const base_string& str, const void* info)
 : ConstrainedObject(info), base_string(str)
{
}

BMPString::BMPString(const value_type* str, const void* info)
: ConstrainedObject(info), base_string(str)
{
}

BMPString::BMPString(const BMPString & other)
: ConstrainedObject(other)
, base_string(other)
{
}

AbstractData* BMPString::create(const void* info)
{
  return new BMPString(info);
}

bool BMPString::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool BMPString::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

AbstractData * BMPString::do_clone() const
{
  return new BMPString(*this);
}

INT_TYPE BMPString::do_compare(const AbstractData& other) const
{
  const BMPString& that = dynamic_cast<const BMPString&>(other);
  return base_string::compare(that);
}

///////////////////////////////////////////////////////////////////////
ASN1_API const UTCTime::InfoType UTCTime::theInfo = {
  UTCTime::create,
  UniversalTagClass << 30 | UniversalUTCTime,
  0
};

AbstractData* UTCTime::create(const void* info)
{
  return new UTCTime(info);
}

UTCTime::UTCTime(const void* info)
: AbstractData(info), year(1), month(1), day(1), hour(0), minute(0), second(0), mindiff(0)
, utc(false)
{}

UTCTime::UTCTime()
: AbstractData(&theInfo), year(1), month(1), day(1), hour(0), minute(0), second(0), mindiff(0)
, utc(false)
{}

UTCTime::UTCTime(const char* value)
: AbstractData(&theInfo)
{
  set(value);
}

UTCTime::UTCTime(int yr, int mon, int dy,
    int hr , int mn, int sc,
    int md, bool u )
: AbstractData(&theInfo), year(yr), month(mon), day(dy), hour(hr), minute(mn), second(sc), mindiff(md)
, utc(u)
{
}

UTCTime& UTCTime::operator = (const UTCTime& other )
{
  year = other.year; month = other.month; day = other.day;
  hour = other.hour; minute = other.minute; second = other.second;
  mindiff = other.mindiff; utc = other.utc;
  return *this;
}

UTCTime::UTCTime(const UTCTime& other)
: AbstractData(other), year(other.year), month(other.month), day(other.day)
  , hour(other.hour), minute(other.minute), second(other.second)
  , mindiff(other.mindiff), utc(other.utc)
{}

static int read_dec2(ASN1_STD istream& is)
{
  int ret (0);
  if(is.good() && isdigit(is.peek())) {
    ret = is.get()-'0';
    if(is.good() && isdigit(is.peek())) {
      (ret*=10)+=(is.get()-'0');
      return ret;
    }
  }

  // old iostream has setstate protected
#ifndef HP_aCC_RW
  is.setstate(ASN1_STD ios::failbit);
#endif

  return 0;
}


void UTCTime::set(const char* valueNotion)
{
#if !defined(HP_aCC_RW)
  ASN1_STD istringstream strm(valueNotion);
#else
  ASN1_STD string strng(valueNotion);
  ASN1_STD istrstream strm(strng.data(), strng.size());
#endif

  year = read_dec2(strm);
  month = read_dec2(strm);
  day = read_dec2(strm);
  hour = read_dec2(strm);
  minute = read_dec2(strm);

  if(strm.good() && isdigit(strm.peek())) // seconds are present
    second = read_dec2(strm);

  int c, hd, md;

  switch(c=strm.get()) {
    case 'Z':
      set_utc(true);
      break;

    case '+':
    case '-':
      hd = read_dec2(strm);
      md = read_dec2(strm);

      if(strm.good()) {
        mindiff = hd*60+md;
        if(c=='-')
          mindiff = -mindiff;
      }
      break;

    default:
      // old iostream has setstate protected
#ifndef HP_aCC_RW
      strm.setstate(ASN1_STD ios::failbit);
#endif
      break;
  }

  if(!strm.good()) { // reset
    year = month = day = 1;
    hour = minute = second = mindiff = 0;
    utc = false;
  }
}

ASN1_STD string UTCTime::get() const
{
  DECLARE_STRSTREAM(strm);

  strm << ASN1_STD setfill('0')
       << ASN1_STD setw(2) << year
       << ASN1_STD setw(2) << month
       << ASN1_STD setw(2) << day
       << ASN1_STD setw(2) << hour
       << ASN1_STD setw(2) << minute
       << ASN1_STD setw(2) << second;

  if (utc)
    strm.put('Z');
  else {
    if(mindiff < 0)
      strm << '-'
           << ASN1_STD setw(2) << -mindiff/60
           << ASN1_STD setw(2) << -mindiff%60;
    else
      strm << '+'
           << ASN1_STD setw(2) << mindiff/60
           << ASN1_STD setw(2) << mindiff%60;
  }

#if defined(HP_aCC_RW)
  strm << '\0';
#endif

  return ASN1_STD string(strm.str());
}

bool UTCTime::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool UTCTime::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

void UTCTime::swap(UTCTime& other)
{
  ASN1_STD swap(year, other.year);
  ASN1_STD swap(month, other.month);
  ASN1_STD swap(day, other.day);
  ASN1_STD swap(hour, other.hour);
  ASN1_STD swap(minute, other.minute);
  ASN1_STD swap(second, other.second);
  ASN1_STD swap(mindiff, other.mindiff);
  ASN1_STD swap(utc, other.utc);
}

INT_TYPE UTCTime::do_compare(const AbstractData& other) const
{
  const UTCTime& that = dynamic_cast<const UTCTime&>(other);
  const int* src = &year, *dst = &that.year;
  for (; src != &mindiff; ++src, ++dst)
    if (*src != *dst)
      return *src - *dst;

  return utc - that.utc;
}

time_t UTCTime::get_time_t() const
{
  struct tm t = { 0 }; // UMR on SUN

  // years since 1900; years before 1970 considered as being 20xx
  t.tm_year = year < 70 ? year+100 : year;
  // the number of full calendar months since the beginning of the year (in the range 0 through 11).
  t.tm_mon = month-1;
  t.tm_mday = day;
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = second;
  return mktime(&t);
}

void UTCTime::set_time_t(time_t gmt)
{
  struct tm* t = gmtime(&gmt);
  year = t->tm_year;
  month = t->tm_mon+1;
  day = t->tm_mday;
  hour = t->tm_hour;
  minute = t->tm_min;
  second = t->tm_sec;
}

AbstractData* UTCTime::do_clone() const
{
  return new UTCTime(*this);
}

///////////////////////////////////////////////////////////////////////
ASN1_API const GeneralizedTime::InfoType GeneralizedTime::theInfo = {
  GeneralizedTime::create,
  UniversalTagClass << 30 | UniversalGeneralisedTime,
  0
};

AbstractData* GeneralizedTime::create(const void* info)
{
  return new GeneralizedTime(info);
}

GeneralizedTime::GeneralizedTime(const void* info)
: UTCTime(info), millisec(0)
{}

GeneralizedTime::GeneralizedTime()
: UTCTime(&theInfo),  millisec(0)
{}

GeneralizedTime::GeneralizedTime(const char* value)
: UTCTime(&theInfo)
{
    set(value);
}

GeneralizedTime::GeneralizedTime(int yr, int mon, int dy,
    int hr , int mn, int sc,
    int ms , int md, bool u )
: UTCTime(yr, mon, dy, hr, mn, sc, md, u), millisec(ms)
{
}

GeneralizedTime& GeneralizedTime::operator = (const GeneralizedTime& other )
{
  UTCTime::operator==(other);
  millisec = other.millisec;
  return *this;
}

GeneralizedTime::GeneralizedTime(const GeneralizedTime& other)
: UTCTime(other), millisec(other.millisec)
{}

void GeneralizedTime::set(const char* valueNotion)
{
  sscanf(valueNotion, "%4d%2d%2d%2d%2d%2d",&year, &month, &day, &hour, &minute, &second);
  int len = strlen(valueNotion);
  if (len > 15 && valueNotion[14] == '.')
  {
    float f;
    sscanf(&valueNotion[14], "%f",&f);
    millisec = (int) f*1000;
  }

  int pos = len - 5;
  if (valueNotion[len-1] == 'Z')
    utc = true;
  else if (valueNotion[pos] == '+' || valueNotion[pos] == '-' ) {
    sscanf(&valueNotion[pos], "%d", &mindiff);
    mindiff = mindiff/100*60 + mindiff%100;
  }
}

ASN1_STD string GeneralizedTime::get() const
{
  char buf[128];
  sprintf(buf, "%04d%02d%02d%02d%02d%02d", year, month, day, hour, minute, second);

  int len = 14;
  if (millisec)
  {
    sprintf(&buf[14], ".%03d", millisec);
    len = 18;
  }

  if (utc)
    sprintf(&buf[len], "Z");
  else if(mindiff<0)
    sprintf(&buf[len], "-%02d%02d", -mindiff/60, -mindiff%60);
  else
    sprintf(&buf[len], "+%02d%02d", mindiff/60, mindiff%60);

  return ASN1_STD string(buf);
}

bool GeneralizedTime::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool GeneralizedTime::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

void GeneralizedTime::swap(GeneralizedTime& other)
{
  UTCTime::swap(other);
  ASN1_STD swap(millisec, other.millisec);
}

INT_TYPE GeneralizedTime::do_compare(const AbstractData& other) const
{
  const GeneralizedTime& that = dynamic_cast<const GeneralizedTime&>(other);
  const int* src = &year, *dst = &that.year;
  for (; src != &mindiff; ++src, ++dst)
    if (*src != *dst)
      return *src - *dst;

  return utc - that.utc;
}

time_t GeneralizedTime::get_time_t() const
{
  struct tm t;
  t.tm_year = year-1900;
  t.tm_mon = month-1;
  t.tm_mday = day;
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = second;
  return mktime(&t);
}

void GeneralizedTime::set_time_t(time_t gmt)
{
  struct tm* t = gmtime(&gmt);
  year = t->tm_year+1900;
  month = t->tm_mon+1;
  day = t->tm_mday;
  hour = t->tm_hour;
  minute = t->tm_min;
  second = t->tm_sec;
}

AbstractData* GeneralizedTime::do_clone() const
{
  return new GeneralizedTime(*this);
}

///////////////////////////////////////////////////////////////////////
bool CHOICE::isSelectionExplicit() const
{
  return info()->tags ? isExplicit(info()->tags[choiceID]) : false;
}

unsigned CHOICE::getSelectionInfoTag() const
{
  return info()->selectionInfos
    ? static_cast<const InfoType*>(info()->selectionInfos[choiceID])->tag
    : 0;
}

AbstractData* CHOICE::create(const void* info)
{
  return new CHOICE(info);
}

CHOICE::CHOICE(const void* information, int id, AbstractData* value)
: AbstractData(information), choice(value) , choiceID(id)
{
}

CHOICE::CHOICE(const CHOICE & other)
  : AbstractData(other)
  , choice( other.choice.get() == 0 ? 0 : other.choice->clone())
  , choiceID( other.choiceID)
{}

CHOICE::~CHOICE()
{
}

CHOICE & CHOICE::operator=(const CHOICE & other)
{
  assert(info_ == other.info_);
  choice.reset(other.choice.get() == 0 ? 0 : other.choice->clone());
  choiceID = other.choiceID;
  return *this;
}

AbstractData* CHOICE::setSelection(int id, AbstractData* obj)
{
  choice.reset(obj);
  choiceID = id;
  return obj;
}

bool CHOICE::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool CHOICE::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

void CHOICE::swap(CHOICE& other)
{
  assert(info_ == other.info_);
  ASN1_STD swap(choice, other.choice);
  ASN1_STD swap(choiceID, other.choiceID);
}

INT_TYPE CHOICE::do_compare(const AbstractData& other) const
{
  const CHOICE& that = dynamic_cast<const CHOICE&>(other);
  if (choiceID >= 0 && choiceID == that.choiceID)
    return choice->compare(*that.choice);

  return choiceID - that.choiceID;
}

AbstractData* CHOICE::do_clone() const
{
  return new CHOICE(*this);
}

bool CHOICE::createSelection()
{
  if (choiceID >= 0 && static_cast<unsigned>(choiceID) < info()->totalChoices )
  {
    const AbstractData::InfoType* selectionInfo =
    static_cast<const AbstractData::InfoType*>(info()->selectionInfos[choiceID]);

    if (selectionInfo)
    {
      choice.reset(selectionInfo->create(selectionInfo));
      return true;
    }
  }
  choiceID = unknownSelection_;
  choice.reset();
  return false;
}

///////////////////////////////////////////////////////////////////////
SEQUENCE::FieldVector::~FieldVector()
{
  iterator it = begin(), last = end();
  for (; it != last; ++it)
    delete *it;

  clear();
}

SEQUENCE::FieldVector::FieldVector(const SEQUENCE::FieldVector& other)
{
  const_iterator it = other.begin(), last = other.end();
  reserve(other.size());
  for (; it != last; ++it)
    push_back( *it ? (*it)->clone() : 0);
}

void SEQUENCE::BitMap::resize(unsigned nBits)
{
  bitData.resize((nBits+7)/8, 0);
  totalBits = nBits;
}

bool SEQUENCE::BitMap::operator [] (unsigned bit) const
{
  if (bit < totalBits)
    return (bitData[bit>>3] & (1 << (7 - (bit&7)))) != 0;

  return false;
}

void SEQUENCE::BitMap::set(unsigned bit)
{
  if (bit < totalBits)
    bitData[(unsigned)(bit>>3)] |= 1 << (7 - (bit&7));
}

void SEQUENCE::BitMap::clear(unsigned bit)
{
  if (bit < totalBits)
    bitData[(unsigned)(bit>>3)] &= ~(1 << (7 - (bit&7)));
}

inline void SEQUENCE::BitMap::swap(BitMap& other)
{
  bitData.swap(other.bitData);
  ASN1_STD swap(totalBits, other.totalBits);
}

const unsigned SEQUENCE::defaultTag =0;

AbstractData* SEQUENCE::create(const void* info)
{
  return new SEQUENCE(info);
}

SEQUENCE::SEQUENCE(const void* information)
: AbstractData(information)
{
  unsigned nBaseFields = info()->numFields;
  int nExtensions = info()->knownExtensions;
  unsigned i;
  fields.resize(nBaseFields+nExtensions, 0);
  optionMap.resize(info()->numOptional);
  for (i = 0; i < nBaseFields; ++i)
    if (info()->ids[i]  == -1 )
      fields[i] = AbstractData::create(info()->fieldInfos[i]);

  if (info()->nonOptionalExtensions)
  {
    extensionMap.bitData.assign(info()->nonOptionalExtensions,
          info()->nonOptionalExtensions + (nExtensions+7/8));
    extensionMap.totalBits = nExtensions;
  }

  for (i = 0; i < extensionMap.size(); ++i)
    if (extensionMap[i])
      fields[i+nBaseFields] = AbstractData::create(info()->fieldInfos[i+nBaseFields]);
}

SEQUENCE::SEQUENCE(const SEQUENCE & other)
: AbstractData(other),
    fields(other.fields),
  optionMap(other.optionMap),
  extensionMap(other.extensionMap)
{
}

SEQUENCE::~SEQUENCE()
{
}

SEQUENCE & SEQUENCE::operator=(const SEQUENCE & other)
{
  assert(info_ == other.info_ );

  FieldVector temp_fields(other.fields);
  BitMap temp_optionalMap(other.optionMap);
  BitMap temp_extensionMap(other.extensionMap);

  fields.swap(temp_fields);
  optionMap.swap(temp_optionalMap);
  extensionMap.swap(temp_extensionMap);

  return *this;
}

bool SEQUENCE::hasOptionalField(unsigned opt) const
{
  if (opt < (unsigned)optionMap.size())
    return optionMap[opt];

  return extensionMap[opt - optionMap.size()];
}

void SEQUENCE::includeOptionalField(unsigned opt, unsigned pos)
{
  if (opt < (unsigned)optionMap.size())
    optionMap.set(opt);
  else {
    assert(extendable());
    opt -= optionMap.size();
    if (opt >= (unsigned)extensionMap.size())
      extensionMap.resize(opt+1);
    extensionMap.set(opt);
  }
  if (fields[pos] == 0)
    fields[pos] = AbstractData::create(info()->fieldInfos[pos]);
}

void SEQUENCE::removeOptionalField(unsigned opt)
{
  if (opt < (unsigned)optionMap.size())
    optionMap.clear(opt);
  else {
    assert(extendable());
    opt -= optionMap.size();
    extensionMap.clear(opt);
  }
}

bool SEQUENCE::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool SEQUENCE::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

AbstractData * SEQUENCE::do_clone() const
{
  return new SEQUENCE(*this);
}

void SEQUENCE::swap(SEQUENCE& other)
{
  fields.swap(other.fields);
  optionMap.swap(other.optionMap);
  extensionMap.swap(other.extensionMap);
}

INT_TYPE SEQUENCE::do_compare(const AbstractData& other) const
{
  const SEQUENCE& that = dynamic_cast<const SEQUENCE&>(other);
  assert(info_ == that.info_);

  int lastOptionalId = -1, result = 0;
  unsigned i;
  for (i = 0; i < info()->numFields ; ++i)
  {
    int id = info()->ids[i];
    if (id == mandatory_  || (hasOptionalField(id) && that.hasOptionalField(id)) )
      result = fields[i]->compare(*that.fields[i]);
    else
      result = hasOptionalField(id) - that.hasOptionalField(id);

    if (result != 0) return result;

    lastOptionalId = ( id != mandatory_ ? id : lastOptionalId);
  }

  for (; i < fields.size(); ++i)
  {
    if (hasOptionalField(++lastOptionalId) && that.hasOptionalField(lastOptionalId))
      result = fields[i]->compare(*that.fields[i]);
    else
      result = hasOptionalField(lastOptionalId) - that.hasOptionalField(lastOptionalId);
  }

  return result;

}

///////////////////////////////////////////////////////

SEQUENCE_OF_Base::SEQUENCE_OF_Base(const void* info)
: ConstrainedObject(info)
{
}

SEQUENCE_OF_Base::SEQUENCE_OF_Base(const SEQUENCE_OF_Base & other)
  : ConstrainedObject(other)
{
  Container::const_iterator first = other.container.begin(), last = other.container.end();
  container.reserve(other.container.size());
  for (; first != last; ++ first)
    container.push_back( (*first)->clone());
}

SEQUENCE_OF_Base & SEQUENCE_OF_Base::operator=(const SEQUENCE_OF_Base & other)
{
  Container temp;
  Container::const_iterator first = other.container.begin(), last = other.container.end();
  temp.reserve(other.container.size());
  for (; first != last; ++first)
    temp.push_back( (*first)->clone());

  temp.swap(container);
  return *this;
}

AbstractData* SEQUENCE_OF_Base::do_clone() const
{
  return new SEQUENCE_OF_Base(*this);
}

bool SEQUENCE_OF_Base::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool SEQUENCE_OF_Base::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

void SEQUENCE_OF_Base::clear()
{
  Container::const_iterator first = container.begin(), last = container.end();
  for (;first != last; ++first)
    delete *first;

  container.clear();
}

INT_TYPE SEQUENCE_OF_Base::do_compare(const AbstractData& other) const
{
  const SEQUENCE_OF_Base& that = dynamic_cast<const SEQUENCE_OF_Base&>(other);
  Container::const_iterator first1 = container.begin(), last1 = container.end();
  Container::const_iterator first2 = that.container.begin(), last2 = that.container.end();
  for (; first1 != last1 && first2 != last2; ++first1, ++first2)
  {
    INT_TYPE r = (*first1)->compare(*(*first2));
    if (r != 0)
      return r;
  }
  return container.size() - that.container.size();
}

void SEQUENCE_OF_Base::insert(iterator position, SEQUENCE_OF_Base::Container::size_type n, const AbstractData& x)
{
//  SEQUENCE_OF_Base::Container::difference_type dist = ASN1_STD distance(begin(), position);
  reserve(size()+ n);
  ASN1_STD generate_n(ASN1_STD inserter(container, position), n, create_from0(x));
}

void SEQUENCE_OF_Base::insert(iterator position, const_iterator first, const_iterator last)
{
//  SEQUENCE_OF_Base::Container::difference_type dist = ASN1_STD distance(container.begin(), position);
  reserve(size()+ (last-first));
  ASN1_STD transform(first, last, ASN1_STD inserter(container, position), create_from_ptr());
}

AbstractData* SEQUENCE_OF_Base::create(const void* info)
{
  return new SEQUENCE_OF_Base(info);
}

void SEQUENCE_OF_Base::resize(Container::size_type sz)
{
  if (sz < size())
  {
    iterator i = begin()+sz, last = end();
    for (; i != last; ++i)
      delete *i;

    container.resize(sz);
  }
  else
  {
    container.reserve(sz);
    for (unsigned i = size(); i < sz; ++i)
      container.push_back(createElement());
  }
}

void SEQUENCE_OF_Base::erase(iterator first, iterator last)
{
  iterator f = first;
  for (;f != last; ++f)
    delete *f;

  container.erase(first, last);
}

//////////////////////////////////////////////////////

ASN1_API const OpenData::InfoType OpenData::theInfo = {
  OpenData::create,
  0,
  0
};

OpenData::OpenData(const OpenData& that)
: AbstractData(that)
, data(that.has_data() ? that.get_data().clone() : 0)
, buf(that.has_buf() ? new OpenBuf(that.get_buf()) : 0)
{}

AbstractData* OpenData::create(const void* info)
{
  return new OpenData(info);
}

void OpenData::swap(OpenData& other)
{
  ASN1_STD swap(data, other.data);
  ASN1_STD swap(buf, other.buf);
}

bool OpenData::do_accept(Visitor& visitor)
{
  return visitor.visit(*this);
}

bool OpenData::do_accept(ConstVisitor& visitor) const
{
  return visitor.visit(*this);
}

INT_TYPE OpenData::do_compare(const AbstractData& other) const
{
  const OpenData& that = dynamic_cast<const OpenData&>(other);
  if (has_data() && that.has_data())
    return get_data().compare(that.get_data());

  if (has_buf() && that.has_buf())
    return lexicographic_compare_bytes(&(*get_buf().begin()), get_buf().size(),
                    &(*that.get_buf().begin()), that.get_buf().size());

  if (isEmpty() && that.isEmpty()) return 0;

  return has_data() ? 1 : -1;
}

AbstractData* OpenData::do_clone() const
{
  return new OpenData(*this);
}

/////////////////////////////////////////////////////////

bool TypeConstrainedOpenData::do_accept(Visitor& v)
{
  return v.visit(*this);
}

AbstractData* TypeConstrainedOpenData::create(const void* info)
{
  return new TypeConstrainedOpenData(
        AbstractData::create(static_cast<const InfoType*>(info)->typeInfo), info);
}

/////////////////////////////////////////////////////////

bool Visitor::visit(SEQUENCE& value)
{
  VISIT_SEQ_RESULT result = preVisitExtensionRoots(value);
  if (result <= STOP)
    return result != FAIL;

  bool visitExtension = (result == CONTINUE);

  int lastOptionalId = -1;
  unsigned i;
  for (i = 0 ; i < value.info()->numFields; ++i)
  {
    int optionalId = value.info()->ids[i];
    result = visitExtensionRoot(value, i, optionalId);
    if ( result <= STOP)
      return result != FAIL;

    lastOptionalId = ( optionalId != -1 ? optionalId : lastOptionalId);
  }

  if (!visitExtension)
  {
//      value.extensionMap.resize(0);
      return true;
  }

  result = preVisitExtensions(value);

  if ( result !=  CONTINUE )
  return result != FAIL;

  for (; i < value.fields.size(); ++i)
  {
    result = visitKnownExtension(value, i, ++lastOptionalId);
    if ( result !=  CONTINUE )
      return result != FAIL;
  }

  return visitUnknownExtensions(value);
}

bool ConstVisitor::visit(const SEQUENCE& value)
{
  if (!preVisitExtensionRoots(value))
    return false;

  unsigned i;
  int lastOptionalId = -1;
  for (i = 0; i < value.info()->numFields ; ++i)
  {
    int optionalId = value.info()->ids[i];
    if (optionalId == -1 || value.hasOptionalField(optionalId))
    {
      if (value.fields[i] == NULL || !visitExtensionRoot(value, i))
        return false;
    }
    lastOptionalId = ( optionalId != -1 ? optionalId : lastOptionalId);
  }

  if (value.extensionMap.size())
  {
    assert(value.extendable());
    if (!preVisitExtensions(value))
      return false;

    for (; i < value.fields.size(); ++i)
    {
      if (!value.hasOptionalField(++lastOptionalId))
        continue;

      if (value.fields[i] == NULL || !visitKnownExtension(value, i))
        return false;
    }
  }
  return afterVisitSequence(value);
}

} // namespace ASN1

// <stubs>
// f9063 � 141 B � unmatched � placement: link order � name unknown
// void f9063(i32, i32) { ... }

// f8967 � 12 B � unmatched � placement: link order � name unknown
// ? f8967(i32) { ... }
// </stubs>
