/*
 * BufferSeeder.h — TSE's in-memory ISeeder (reconstructed). Header-only: it throws, so it is
 * kept out of asn1.h, whose translation units are compiled without exceptions.
 */
#ifndef _ASN1_BUFFERSEEDER_H
#define _ASN1_BUFFERSEEDER_H

#include <stdexcept>
#include "asn1.h"

namespace ASN1 {

class ASN1_API BufferSeeder : public ISeeder
{
public:
  BufferSeeder(const char* buffer, unsigned size) : m_begin(buffer), m_end(buffer + size), m_current(buffer) {}

  bool eof() const { return m_current >= m_end; }

  unsigned char read()
  {
    if (eof())
      throw ASN1_STD runtime_error("BufferSeed: no more buffer");
    unsigned char value = *m_current;
    ++m_current;
    return value;
  }

  unsigned read(char* buffer, unsigned size)
  {
    const char* end = m_end;
    const char* current = m_current;
    memcpy(buffer, current, size);
    if (current + size > end)
      size = end - current;
    m_current = current + size;
    return size;
  }

  void seek(unsigned position)
  {
    const char* p = m_begin + position;
    if (p > m_end)
      throw ASN1_STD runtime_error("BufferSeed: seek out of buffer");
    m_current = p;
  }

  void skip(unsigned size)
  {
    const char* p = m_current + size;
    if (p > m_end)
      throw ASN1_STD runtime_error("BufferSeed: skip out of buffer");
    m_current = p;
  }

  unsigned size() const { return m_end - m_begin; }
  unsigned remaining() const { return m_end - m_current; }
  unsigned tell() const { return m_current - m_begin; }
  void resize(unsigned size) { m_end = m_begin + size; }

private:
  const char* m_begin;
  const char* m_end;
  const char* m_current;
};

} // namespace ASN1

#endif
