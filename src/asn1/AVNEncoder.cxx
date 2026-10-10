/*
 * avnencoder.cxx
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
 * The Original Code is III ASN.1 Tool
 *
 * The Initial Developer of the Original Code is Institute for Information Industry.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * The code is modified by Genesys Telecommunications Labs UK, 2003-2011
 * Contributors: 
 *    Arunas Ruksnaitis <arunas.ruksnaitis@genesyslab.com>
 *    Rustam Mirzaev <rustam.mirzaev@genesyslab.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ios_helper.h"

#include <vector>

#if defined(HP_aCC_RW)
# include <iomanip.h>
#else
# include <iomanip>
#endif

namespace ASN1 {

ASN1_STD string AbstractData::asValueNotation() const
{
  DECLARE_STRSTREAM(strm);

  AVNEncoder encoder(strm);
  accept(encoder);
  return strm.str();
}

iostate AbstractData::print_on(ASN1_STD ostream & strm) const
{
  AVNEncoder encoder(strm);
  if (accept(encoder))
    return ASN1_STD ios::goodbit;

  return ASN1_STD ios::failbit;
}

ASN1_STD ostream & operator<<(ASN1_STD ostream & os, const AbstractData & arg)
{
  if (!os.good()) return os;

  return g_inserter(os, arg);
}

void AbstractData::Print(PrintHelper& helper) const
{
    AVNPrinter printer(helper);
    accept(printer);
}

bool IntegerWithNamedNumber::getName(ASN1_STD string& str) const
{
  const NameEntry* begin = info()->nameEntries;
  const NameEntry* end   = begin+ info()->entryCount;
  const NameEntry* i = ASN1_STD lower_bound(begin, end, value, NameEntryCmp());

  if (i != end && i->value == value)
  {
    str = i->name;
    return true;
  }
  return false;
}

const char* ENUMERATED::getName() const
{
  if (value >=0 && value <= getMaximum())
    return info()->names[value];

  return 0;
}

bool AVNEncoder::do_visit(const Null& value)
{
  strm << "NULL";
  return strm.good();
}

bool AVNEncoder::do_visit(const BOOLEAN& value)
{
  if (value)
    strm << "TRUE";
  else
    strm << "FALSE";
  return strm.good();
}

bool AVNEncoder::do_visit(const INTEGER& value)
{
  if (!value.constrained() || value.getLowerLimit() < 0)
    strm << (INT_TYPE)value.getValue();
  else
    strm << (UINT_TYPE)value.getValue();
  return strm.good();
}

bool AVNEncoder::do_visit(const IntegerWithNamedNumber& value)
{
  ASN1_STD string str;
  if (value.getName(str))
    strm << str;
  else
    return visit(static_cast<const INTEGER&>(value));
  return strm.good();
}

bool AVNEncoder::do_visit(const ENUMERATED& value)
{
  const char* name = value.getName();
  if (name != 0)
    strm << name;
  else
    strm << value.asInt();
  return strm.good();
}

bool AVNEncoder::do_visit(const OBJECT_IDENTIFIER& value)
{
  strm << "{ ";
  for (unsigned i = 0;  i < value.levels() && strm.good(); ++i)
    strm << value[i] << ' ';
  strm << "}";
  return strm.good();
}

bool AVNEncoder::do_visit(const BIT_STRING& value)
{
  if(value.hasNames()) { // IdentifierList notation
    strm << '{';
    char sep = ' ';
    const char * name;

    for (unsigned i = 0; i < value.size() && strm.good(); ++i) {
      if (!value[i]) continue;

      strm << sep;
      name = value.getName(i);
      if(name)
        strm << name;
      else
        strm << i;

      sep = ',';
    }
    strm << ' ' << '}';
  }
  else { // bstring notation
    strm << '\'';
    for (unsigned i = 0; i < value.size() && strm.good(); ++i)
    {
      if (value[i])
        strm << '1';
      else
        strm << '0';
    }
    strm << "\'B";
  }
  return strm.good();
}

bool AVNEncoder::do_visit(const OCTET_STRING& value)
{
#ifdef HP_aCC_RW
  long flags = strm.flags();
#else
  ASN1_STD ios::fmtflags flags = strm.flags();
#endif
  strm << '\'';

  for (unsigned i = 0; i < value.size() && strm.good(); ++i)
  {
    strm << ASN1_STD hex << ASN1_STD setw(2) << ASN1_STD setfill('0') << (0xFF & value[i]);
    if (i != value.size()-1)
      strm << ' ';
  }

  if (strm.good())
    strm << "\'H";
  strm.setf(flags);
  strm << ASN1_STD setfill(' ') ;
  return strm.good();
}

bool AVNEncoder::do_visit(const AbstractString& value)
{
  strm << '\"' << static_cast<const ASN1_STD string&>(value) << '\"';
  return strm.good();
}

bool AVNEncoder::do_visit(const BMPString& value)
{
  ASN1_STD vector<char> tmp;
  tmp.resize(value.size()*2+1, 0);
  int len = wcstombs(&*tmp.begin(), &*value.begin(), value.size());
  if (len != -1)
  {
    tmp[len] = 0;
    strm << '\"' << &(*tmp.begin()) << '\"';
  }
  else // output Quadruple form
  {
    strm << '{';
    for (unsigned i = 0; i < value.size() && strm.good(); ++i)
    {
      strm << "{ 0, 0, " << (int) (value[i] >> 8) << ", " << (int) value[i]  << '}';
      if (strm.good() && i != value.size()-1)
        strm << ", ";
    }
    if (strm.good())
      strm << '}';
  }
  return strm.good();
}

bool AVNEncoder::do_visit(const CHOICE& value)
{
  if (value.currentSelection() >= 0)
  {
    strm << value.getSelectionName() << " : ";
    return value.getSelection()->accept(*this);
  }
  return false;
}

bool AVNEncoder::do_visit(const OpenData& value)
{
  if (value.has_data())
  {
    return value.get_data().accept(*this);
  }
  else if (value.has_buf())
  {
    OCTET_STRING ostr(value.get_buf());
    return ostr.accept(*this);
  }
  return false;
}

bool AVNEncoder::do_visit(const UTCTime& value)
{
  strm << '\"' << value.get() << '\"';
  return strm.good();
}

bool AVNEncoder::do_visit(const GeneralizedTime& value)
{
  strm << '\"' << value.get() << '\"';
  return strm.good();
}

bool AVNEncoder::do_visit(const SEQUENCE_OF_Base& value)
{
  strm << "{\n";
  SEQUENCE_OF_Base::const_iterator first = value.begin(), last = value.end();

  indent +=2;

  for (; first != last && strm.good(); ++first)
  {
    strm << ASN1_STD setw(indent) << " ";
    if (!(*first)->accept(*this))
      return false;

    if (first != last-1)
      strm << ",\n";
  }

  indent -=2;
  if (value.size() && strm.good())
    strm << '\n';
  strm << ASN1_STD setw(indent+1) << "}";
  return strm.good();
}

bool AVNEncoder::preVisitExtensionRoots(const SEQUENCE& value)
{
  outputSeparators.push_back(false);
  strm << "{\n";
  return strm.good();
}

bool AVNEncoder::visitExtensionRoot(const SEQUENCE& value, int index)
{
  if (outputSeparators.back())
    strm << ",\n";

  strm << ASN1_STD setw(strlen(value.getFieldName(index)) + indent +2)
       << value.getFieldName(index) << ' ';
  indent +=2;

  if (!value.getField(index)->accept(*this))
    return false;

  indent -=2;
  outputSeparators.back() = true;
  return true;
}

bool AVNEncoder::visitKnownExtension(const SEQUENCE& value, int index)
{
  return visitExtensionRoot(value, index);
}

bool AVNEncoder::afterVisitSequence(const SEQUENCE& value)
{
  if (outputSeparators.back())
    strm  << '\n';

  outputSeparators.pop_back();
  strm << ASN1_STD setw(indent + 1) << "}";
  return strm.good();
}

/////////////////////////////////////
// AVNPrinter
/////////////////////////////////////

bool AVNPrinter::do_visit(const Null& value)
{
  helper.Print("NULL");
  return true;
}

bool AVNPrinter::do_visit(const BOOLEAN& value)
{
  if (value)
    helper.Print("TRUE");
  else
    helper.Print("FALSE");
  return true;
}

bool AVNPrinter::do_visit(const INTEGER& value)
{
  if (!value.constrained() || value.getLowerLimit() < 0)
    helper.Print("%d", (int)value.getValue());
  else
    helper.Print("%u", (unsigned)value.getValue());
  return true;
}

bool AVNPrinter::do_visit(const IntegerWithNamedNumber& value)
{
  ASN1_STD string str;
  if (value.getName(str))
    helper.Print("%s", str.c_str());
  else
    return visit(static_cast<const INTEGER&>(value));
  return true;
}

bool AVNPrinter::do_visit(const ENUMERATED& value)
{
  const char* name = value.getName();
  if (name != 0)
    helper.Print("%s", name);
  else
    helper.Print("%d", value.asInt());
  return true;
}

bool AVNPrinter::do_visit(const OBJECT_IDENTIFIER& value)
{
  helper.Print("{ ");
  for (unsigned i = 0;  i < value.levels(); ++i)
    helper.Print("%u ", value[i]);
  helper.Print("}");
  return true;
}

bool AVNPrinter::do_visit(const BIT_STRING& value)
{
  if(value.hasNames()) { //IdentifierList notation
    helper.Print("{");
    char sep = ' ';
    const char * name;

    for (unsigned i = 0; i < value.size(); ++i) {
      if (value[i]) {
        helper.Print("%c", sep);
        name = value.getName(i);
        if(name)
          helper.Print("%s", name);
        else
          helper.Print("%d", i);

        sep = ',';
      }
    }
    helper.Print(" }");
  }
  else { //bstring notation
    helper.Print("\'");
    for (unsigned i = 0; i < value.size() && true; ++i)
    {
      if (value[i])
        helper.Print("1");
      else
        helper.Print("0");
    }
    helper.Print("\'B");
  }
  return true;
}

bool AVNPrinter::do_visit(const OCTET_STRING& value)
{
  helper.Print("\'");

  for (unsigned i = 0; i < value.size() && true; ++i)
  {
    helper.Print("%02x", (0xFF & value[i]));
    if (i != value.size()-1)
      helper.Print(" ");
  }

  helper.Print("\'H");
  return true;
}

bool AVNPrinter::do_visit(const AbstractString& value)
{
  helper.Print("\"%s\"", static_cast<const ASN1_STD string&>(value).c_str());
  return true;
}

bool AVNPrinter::do_visit(const BMPString& value)
{
  ASN1_STD vector<char> tmp;
  tmp.resize(value.size()*2+1, 0);
  int len = wcstombs(&*tmp.begin(), &*value.begin(), value.size());
  if (len != -1)
  {
    tmp[len] = 0;
    helper.Print("\"%s\"", &(*tmp.begin()));
  }
  else // output Quadruple form
  {
    helper.Print("{");
    for (unsigned i = 0; i < value.size() && true; ++i)
    {
      helper.Print("{ 0, 0, %d, %d }", (int) (value[i] >> 8), (int) value[i]);
      if (i != value.size()-1)
        helper.Print(", ");
    }
    helper.Print("}");
  }
  return true;
}

bool AVNPrinter::do_visit(const CHOICE& value)
{
  if (value.currentSelection() >= 0)
  {
    helper.Print("%s : ", value.getSelectionName());
    return value.getSelection()->accept(*this);
  }
  return false;
}

bool AVNPrinter::do_visit(const OpenData& value)
{
  if (value.has_data())
  {
    return value.get_data().accept(*this);
  }
  else if (value.has_buf())
  {
    OCTET_STRING ostr(value.get_buf());
    return ostr.accept(*this);
  }
  return false;
}

bool AVNPrinter::do_visit(const UTCTime& value)
{
  helper.Print("\"%s\"", value.get().c_str());
  return true;
}

bool AVNPrinter::do_visit(const GeneralizedTime& value)
{
  helper.Print("\"%s\"", value.get().c_str());
  return true;
}

bool AVNPrinter::do_visit(const SEQUENCE_OF_Base& value)
{
  helper.Print("{\n");
    SEQUENCE_OF_Base::const_iterator first = value.begin(), last = value.end();
    indent +=2;
  for (; first != last && true; ++first)
  {
    helper.Print("%*s", indent, "");
    if (!(*first)->accept(*this))
      return false;
    if (first != last-1)
      helper.Print(",\n");
  }
    indent -=2;
  if (value.size())
    helper.Print("\n");
  helper.Print("%*s}", indent, "");
  return true;
}

bool AVNPrinter::preVisitExtensionRoots(const SEQUENCE& value)
{
  outputSeparators.push_back(false);
  helper.Print("{\n");
  return true;
}

bool AVNPrinter::visitExtensionRoot(const SEQUENCE& value, int index)
{
  if (outputSeparators.back())
    helper.Print(",\n");
  helper.Print("%*s ", strlen(value.getFieldName(index)) + indent +2, value.getFieldName(index));
    indent +=2;
  if (!value.getField(index)->accept(*this))
    return false;
    indent -=2;
  outputSeparators.back() = true;
  return true;
}

bool AVNPrinter::visitKnownExtension(const SEQUENCE& value, int index)
{
  return visitExtensionRoot(value, index);
}

bool AVNPrinter::afterVisitSequence(const SEQUENCE& value)
{
  if (outputSeparators.back())
    helper.Print("\n");
  outputSeparators.pop_back();
  if (indent)
    helper.Print("%*s}", indent, "");
  else
    helper.Print("}");
  return true;
}

}
