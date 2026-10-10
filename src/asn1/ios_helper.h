/*
 * Copyright (c) 2001 Institute for Information Industry, Taiwan, Republic of China
 * (http://www.iii.org.tw/iiia/ewelcome.htm)
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 * The code is modified by Genesys Telecommunications Labs UK
*/
#ifndef IOS_HELPER_H
#define IOS_HELPER_H

#include "asn1.h"

#if defined(HP_aCC_RW)
#  include <iostream.h>
#else
#  include <ios>
#endif

template <class Argument>
ASN1_STD istream& g_extractor (ASN1_STD istream& is, Argument& arg)
{
  if (is.good())
  {
    arg.get_from(is);
  }
  return is;
}

template <class Argument>
ASN1_STD ostream& g_inserter (ASN1_STD ostream& os, const Argument& arg)
{
  if (os.good())
  {
    arg.print_on(os);
  }
  return os;
}

namespace ASN1 {

#if defined(ASN_USE_LONG) && defined(_WIN32)

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
ASN1_STD ostream& operator<<(ASN1_STD ostream&, const INT_TYPE& _val);
ASN1_STD ostream& operator<<(ASN1_STD ostream&, const UINT_TYPE& _val);
ASN1_STD istream& operator>>(ASN1_STD istream&, INT_TYPE& _val);
ASN1_STD istream& operator>>(ASN1_STD istream&, UINT_TYPE& _val);
#endif
#endif

}

#endif
