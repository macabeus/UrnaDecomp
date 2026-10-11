/*
 * DataFree.cxx
 *
 * TSE's DataFree visitor (reconstructed from vota_web_wasm.wasm): empties the data it visits.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "asn1.h"

namespace ASN1 {

bool DataFree::do_visit(Null& value)
{
  return true;
}

bool DataFree::do_visit(BOOLEAN& value)
{
  return true;
}

bool DataFree::do_visit(INTEGER& value)
{
  value = value.getLowerLimit();
  return true;
}

bool DataFree::do_visit(IntegerWithNamedNumber& value)
{
  return true;
}

bool DataFree::do_visit(ENUMERATED& value)
{
  return true;
}

bool DataFree::do_visit(OBJECT_IDENTIFIER& value)
{
  return true;
}

bool DataFree::do_visit(BIT_STRING& value)
{
  value.resize(0);
  return true;
}

bool DataFree::do_visit(OCTET_STRING& value)
{
  value.clear();
  return true;
}

bool DataFree::do_visit(AbstractString& value)
{
  value.clear();
  return true;
}

bool DataFree::do_visit(BMPString& value)
{
  value.clear();
  return true;
}

bool DataFree::do_visit(CHOICE& value)
{
  if (value.currentSelection() >= 0)
    value.getSelection()->accept(*this);
  return true;
}

bool DataFree::do_visit(SEQUENCE_OF_Base& value)
{
  SEQUENCE_OF_Base::iterator it = value.begin(), last = value.end();
  for (; it != last; ++it)
    (*it)->accept(*this);
  value.clear();
  return true;
}

bool DataFree::do_visit(OpenData& value)
{
  AbstractData* data = value.release_data();
  OpenBuf* buf = value.release_buf();
  delete data;
  delete buf;
  return true;
}

bool DataFree::do_revisit(OpenData& value)
{
  return false;
}

bool DataFree::do_visit(TypeConstrainedOpenData& value)
{
  AbstractData* data = value.release_data();
  OpenBuf* buf = value.release_buf();
  delete data;
  delete buf;
  return true;
}

bool DataFree::do_visit(UTCTime& value)
{
  value = UTCTime("");
  return true;
}

bool DataFree::do_visit(GeneralizedTime& value)
{
  value = GeneralizedTime("");
  return true;
}

}//namespace

// <stubs>
// f2182 · 683 B · unmatched · placement: link order · name unknown
// void f2182(i32, i32, i32, i32, i32, i32, i32) { ... }
// </stubs>
