/*
  IO helpers for a compiler that does not have default [in,out]put operators for 'long long'
*/
#include "asn1.h"
#include "ios_helper.h"

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

namespace ASN1 {

#if defined(ASN_USE_LONG) && defined(_WIN32)

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
ASN1_STD ostream& operator<<(ASN1_STD ostream &os, const INT_TYPE& _val)
{
  char buf[24]; // full size of int64 is 20 bytes (including sign)
#ifdef output_hex
  sprintf(buf, "%I64x", _val);
#else
  sprintf(buf, "%I64d", _val);
#endif
  return os << buf;
}

ASN1_STD ostream& operator<<(ASN1_STD ostream &os, const UINT_TYPE& _val)
{
  char buf[24]; // full size of unsigned is 20 bytes
#ifdef output_hex
  sprintf(buf, "%I64x", _val);
#else
  sprintf(buf, "%I64u", _val);
#endif
  return os << buf;
}

//using namespace std;

ASN1_STD istream& operator>>(ASN1_STD istream &is, INT_TYPE& _val)
{
  // skip whitespaces
  try {
    int c;
    while((c = is.peek()) == ' ' || c == '\t') is.get();

    bool neg = false;
    if((c = is.get()) == '-' && isdigit(is.peek()))
      neg = true;
    else if(!isdigit(c))
    {
      is.setf(ASN1_STD ios::failbit);
      is.putback(c);
      return is;
    }
    else
      is.putback(c);

    _val = 0;
    for(int k = 0; k < 19; k++)
    {
      if(!isdigit((c = is.get())))
      {
        is.putback(c);
        break;
      }

      if(UINT_TYPE(_val*10 + c-'0') < 0x7fffffffffffffff)
        _val = _val*10 + c-'0';
    }

    if(neg) _val *= -1;
  }
  catch(...) {
    is.setf(ASN1_STD ios::failbit);
  }

  return is;
}

ASN1_STD istream& operator>>(ASN1_STD istream &is, UINT_TYPE& _val)
{
  try {
    // skip whitespaces
    int c;
    while((c = is.peek()) == ' ' || c == '\t') is.get();

    if(!isdigit(c = is.peek()))
    {
      is.setf(ASN1_STD ios::failbit);
      return is;
    }

    _val = 0;
    for(int k = 0; k < 19; k++)
    {
      if(!isdigit((c = is.get())))
      {
        is.putback(c);
        break;
      }

      if(UINT_TYPE(_val*10 + c-'0') < 0xffffffffffffffff)
        _val = _val*10 + c-'0';
    }
  }
  catch(...) {
    is.setf(ASN1_STD ios::failbit);
  }

  return is;
}
#endif

#endif

}
