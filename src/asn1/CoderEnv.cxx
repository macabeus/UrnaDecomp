/*
 * CoderEnv.cxx
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
 * Contributor(s): Huang-Ming Huang
 *
 * 2001/06/26 Huang-Ming Huang
 * Version 2.1 Reimplemented to minimize the code size.
 *
 * The code is modified by Genesys Telecommunications Labs UK, 2003-2011
 * Contributors: 
 *    Arunas Ruksnaitis <arunas.ruksnaitis@genesyslab.com>
 *    Rustam Mirzaev <rustam.mirzaev@genesyslab.com>
 */


#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "asn1.h"

namespace ASN1 {

bool CoderEnv::decode(const char* first, size_t size, AbstractData& val, bool defered)
{
  if (get_encodingRule() == per_Basic_Aligned)
  {
    PERDecoder decoder(first, size, defered ? NULL : this);
    return val.accept(decoder);
  }
  if (get_encodingRule() == ber)
  {
    BERDecoder decoder(first, size, defered ? NULL : this);
    return val.accept(decoder);
  }

  if (get_encodingRule() == avn)
  {
#  if defined(HP_aCC_RW)
    ASN1_STD istrstream strm(first, size);
#else
    ASN1_STD istringstream strm(ASN1_STD string(first, first+size));
#endif

    AVNDecoder decoder(strm);
    return val.accept(decoder);
  }
  return false;
}

} //namespace
