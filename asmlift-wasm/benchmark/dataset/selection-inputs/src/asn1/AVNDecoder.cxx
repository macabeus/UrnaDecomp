/*
 * avndecoder.cxx
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

namespace ASN1 {

#if defined(HP_aCC_RW)
// restricted version of stringstream for HP
class asn_strstream : public ASN1_STD strstream
{
public:
  typedef open_mode openmode;

  asn_strstream(const ASN1_STD string& s, openmode mode =ios::in) : strstream(const_cast<char*>(s.data()), s.size(), mode) {}
};
#else
typedef ASN1_STD stringstream asn_strstream;
#endif

bool AbstractData::setFromValueNotation(const ASN1_STD string& valueString)
{
  asn_strstream strm(valueString, ASN1_STD ios::in);
  AVNDecoder decoder(strm);
  return accept(decoder);
}

iostate AbstractData::get_from(ASN1_STD istream & strm)
{
  AVNDecoder decoder(strm);
  if (accept(decoder)) return ASN1_STD ios::goodbit;

  return ASN1_STD ios::failbit;
}

ASN1_STD istream & operator >>(ASN1_STD istream &is, AbstractData & arg)
{
  if (!is.good()) return is;

  return g_extractor(is, arg);
}

inline bool input_success(ASN1_STD istream& strm)
{
  return !(strm.rdstate() & (ASN1_STD ios::badbit | ASN1_STD ios::failbit));
}

bool IntegerWithNamedNumber::setFromName(const ASN1_STD string& str)
{
  const NameEntry* begin = info()->nameEntries, *i = begin;
  for (; i != begin+info()->entryCount; ++i)
  {
    if (strcmp(i->name, str.c_str()) != 0) continue;

    value = i->value;
    return true;
  }

  return false;
}

bool ENUMERATED::setFromName(const ASN1_STD string& str)
{
  const char** pos = info()->names;
  const char** end = pos + getMaximum()+1;
  for (; pos != end ; ++pos)
    if (strcmp(str.c_str(), *pos) == 0)
      break;
  value = pos - info()->names;

  if (value <= getMaximum())
    return true;
  else
    return false;
}

bool AVNDecoder::do_visit(Null& value)
{
  ASN1_STD string tmp;
  char c;
  while (strm >> c)
  {
    if (isalpha(c))
      tmp += c;
    else
    {
      strm.putback(c);
      break;
    }
  }

  if (tmp != "NULL")
    return false;

  return true;
}

bool AVNDecoder::do_visit(BOOLEAN& value)
{
  ASN1_STD string tmp;
  char c;
  while (strm >> c)
  {
    if (isalpha(c))
      tmp += c;
    else
    {
      strm.putback(c);
      break;
    }
  }

  if (tmp == "TRUE")
    value = true;
  else if (tmp == "FALSE")
    value = false;
  else
    return false;

  return true;
}

bool AVNDecoder::do_visit(INTEGER& value)
{
  if (!value.constrained() || value.getLowerLimit() < 0)
  {
    INT_TYPE tmp;
    strm >> tmp;
    value = tmp;
  }
  else
  {
    UINT_TYPE tmp;
    strm >> tmp;
    value = tmp;
  }
  return input_success(strm);
}

bool AVNDecoder::do_visit(IntegerWithNamedNumber& value)
{
  char c;
  if (strm >> c)
  {
    strm.putback(c);
    if (isdigit(c) || c == '-')
      return visit(static_cast<INTEGER &>(value));

    ASN1_STD string tmp;
    if (strm >> tmp)
      return value.setFromName(tmp);
  }
  return false;
}

bool AVNDecoder::do_visit(ENUMERATED& value)
{
  ASN1_STD string tmp;
  if (strm >> tmp)
    return value.setFromName(tmp);

  return false;
}

bool AVNDecoder::do_visit(OBJECT_IDENTIFIER& value)
{
  char c;
  if (!(strm >> c)) return false;

  if (c == '{')
  {
    ASN1_STD string subString;
    if (ASN1_STD getline(strm, subString, '}'))
    {
      asn_strstream subIs(subString);
      while (subIs >> subString)
      {
        // process ObjIdComponent , either in the form "itu(0)"(NameAndNumberForm)
        // or a number such as "2250" (NumberForm)
        asn_strstream s(subString);
        unsigned v;
        if (isdigit(subString[0])) // parse NumberForm
        {
          if (s >> v)
            value.append(v);
          else
            return false;
        }
        else // parse NameAndNumberForm
        {
          if (ASN1_STD getline(s, subString, '(') && s >> v)
          {
            value.append(v);
            continue;
          }

          if (!(subIs >> c)) return false;

          if (c == '(' && s >> v)
          {
            value.append(v);
            if (s >> c && c == ')')
              continue;
          }
        }
      }
      return true;
    }
  }
  return false;
}

struct is_space_or_newline
{
  bool operator()(char c) { return c == ' ' || c == '\n'; }
};

bool get_from_string(ASN1_STD string& str0, ASN1_STD vector<char>& value, unsigned int& totalBits, int base)
{
  assert(base == 16 || base == 2);

  int nCharAByte = (base == 16 ) ? 2 : 8;
  ASN1_STD string str;
  ASN1_STD remove_copy_if(str0.begin(), str0.end(), ASN1_STD inserter(str, str.begin()), is_space_or_newline());

  totalBits = str.size();
  if (str.size() % nCharAByte)
    str.insert(str.end(), nCharAByte - (str.size() % nCharAByte), '0');
  value.reserve(str.size()/nCharAByte);

  ASN1_STD string tmp(nCharAByte, ' ');
  char* stopPos;
  for (ASN1_STD string::iterator it = str.begin(); it != str.end() ; it+=nCharAByte)
  {
    char v;
    ASN1_STD copy(it, it+nCharAByte, tmp.begin());
    const char *tmpArr = &*tmp.begin();
    size_t      tmpSz  = tmp.size();
    v = (char) strtol(tmpArr, &stopPos, base);
    if ((stopPos-tmpArr) == tmpSz)
      value.push_back(v);
    else
      return false;
  }
  return true;
}

inline bool get_from_bstring(ASN1_STD string& str, ASN1_STD vector<char>& value, unsigned int& totalBits)
{
  return get_from_string(str, value, totalBits, 2);
}

inline bool get_from_hstring(ASN1_STD string& str, ASN1_STD vector<char>& value, unsigned int& totalBits)
{
  return get_from_string(str, value, totalBits, 16);
}

bool get_value_from(ASN1_STD istream& strm, ASN1_STD vector<char>& value, unsigned int& totalBits)
{
  char c;
  if (strm >> c)
  {
    if (c != '\'') return false;

    ASN1_STD string str;
    if (!ASN1_STD getline(strm, str, '\'')) return false;

    if (strm >> c)
    {
      switch (c)
      {
        case 'B':
          return get_from_bstring(str, value, totalBits);

        case 'H':
          return get_from_hstring(str, value, totalBits);

        default:
          return false;
      }
    }
  }
  return false;
}

bool AVNDecoder::do_visit(BIT_STRING& value)
{
  return get_value_from(strm, value.bitData, value.totalBits);
}

bool AVNDecoder::do_visit(OCTET_STRING& value)
{
  unsigned totalBits;
  return get_value_from(strm, value, totalBits);
}

bool get_string(ASN1_STD istream& strm, ASN1_STD string& str)
{
  char c;
  if (strm >> c)
  {
    if (c == '\"')
      return !!ASN1_STD getline(strm, str , '\"');
  }
  return false;
}

bool AVNDecoder::do_visit(AbstractString& value)
{
  return get_string(strm, value);
}

bool AVNDecoder::do_visit(BMPString& value)
{
  ASN1_STD string str;
  if (!get_string(strm, str)) return false;

  ASN1_STD vector<wchar_t> tmp;
  tmp.resize(str.size()+1, 0);
  int len = mbstowcs(&*tmp.begin(), str.c_str(), str.size());
  if (len == -1) return false;

  value.assign(&*tmp.begin(), len);
  return true;
}

bool get_id(const char** names, unsigned size, const ASN1_STD string& name, int& id)
{
  for (unsigned i = 0; i < size; ++i)
  {
    if (name != names[i]) continue;

    id = i;
    return true;
  }
  return false;
}

bool AVNDecoder::do_visit(CHOICE& value)
{
  ASN1_STD string identifier;
  char c;
  if (strm >> identifier >> c)
  {
    int choiceID;
    if (c ==':' && get_id(value.info()->names, value.info()->totalChoices, identifier, choiceID) &&
        value.select(choiceID))
      return value.getSelection()->accept(*this);
  }
  return false;
}

bool AVNDecoder::do_visit(SEQUENCE_OF_Base& value)
{
  char c;
  if (strm >> c)
  {
    if (c != '{' || (strm >> c, !strm)) return false;

    value.clear();

    if (c != '}')
      strm.putback(c);
    else
      return true; // no entry, return and indicate success

    do {
      asn_ptr<AbstractData> entry(value.createElement());
      if (entry.get() && entry->accept(*this) && strm >> c)
        value.push_back(entry.release());
      else
        return false;
    } while (c == ',');

    if (c == '}') return true;
  }
  return false;
}

bool AVNDecoder::do_visit(OpenData& value)
{
  // not implemented
  return false;
}

bool AVNDecoder::do_revisit(OpenData& value)
{
  // not implemented
  return false;
}

bool AVNDecoder::do_visit(TypeConstrainedOpenData& value)
{
  assert(value.has_data());
  return value.get_data().accept(*this);
}

bool AVNDecoder::do_visit(UTCTime& value)
{
  ASN1_STD string str;
  if (get_string(strm, str))
  {
    value.set(str.c_str());
    return true;
  }
  return false;
}

bool AVNDecoder::do_visit(GeneralizedTime& value)
{
  ASN1_STD string str;
  if (get_string(strm, str))
  {
    value.set(str.c_str());
    return true;
  }
  return false;
}

Visitor::VISIT_SEQ_RESULT AVNDecoder::preVisitExtensionRoots(SEQUENCE& value)
{
  char c;
  if (strm >> c && c == '{')
  {
    if (strm >> c)
    {
      if (c == '}') return STOP;

      strm.putback(c);
      ASN1_STD string id;
      if (strm >> id)
      {
        identifiers.push_back(id);
        return CONTINUE;
      }

      return FAIL;
    }
  }
  return FAIL;
}

Visitor::VISIT_SEQ_RESULT AVNDecoder::visitExtensionRoot(SEQUENCE& value, int index, int optional_id)
{
  if (identifiers.back() == value.getFieldName(index))
  {
    if (optional_id != -1)
      value.includeOptionalField(optional_id, index);

    AbstractData* field = value.getField(index);
    if (field && field->accept(*this))
    {
      char c;
      if (strm >> c)
      {
        if (c == ',' && (strm >> identifiers.back() ) )
          return CONTINUE;

        if (c == '}')
          return STOP;
      }
    }
    return FAIL;
  }

  return (optional_id != -1) ? CONTINUE : FAIL ; // return true only it is not a mandatory component
}

Visitor::VISIT_SEQ_RESULT AVNDecoder::visitKnownExtension(SEQUENCE& value, int index, int optional_id)
{
  return visitExtensionRoot(value, index, optional_id);
}

bool AVNDecoder::visitUnknownExtensions(SEQUENCE& value)
{
  if (identifiers.back() != "}") return false;

  identifiers.pop_back();
  return true;
}

}
