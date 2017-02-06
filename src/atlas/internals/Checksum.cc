
#include <stdint.h>
#include <cstddef>
#include "atlas/internals/Checksum.h"

namespace atlas {
namespace internals {
namespace { // anonymous

// Inefficient implementation of Fletcher's checksum algorithm
static uint16_t fletcher16( const uint8_t* data, int size ) {
  uint16_t s1(0), s2(0);
  for (int i=0; i<size; ++i) {
    s1 = (s1 + data[i]) % 255;
    s2 = (s2 + s1     ) % 255;
  }
  s2 <<= 8;
  s2 |=  s1;
  return s2;
}

}

static checksum_t checksum(const char* data, size_t size)
{
  // return fletcher64( reinterpret_cast<const uint32_t*>(data), size/sizeof(uint32_t) );
  // return fletcher32( reinterpret_cast<const uint16_t*>(data), size/sizeof(uint16_t) );
  return fletcher16( reinterpret_cast<const uint8_t*>(data), size/sizeof(uint8_t) );
}

checksum_t checksum(const int values[], size_t size)
{
  return checksum(reinterpret_cast<const char*>(&values[0]),size*sizeof(int)/sizeof(char));
}

checksum_t checksum(const long values[], size_t size)
{
  return checksum(reinterpret_cast<const char*>(&values[0]),size*sizeof(long)/sizeof(char));
}

checksum_t checksum(const float values[], size_t size)
{
  return checksum(reinterpret_cast<const char*>(&values[0]),size*sizeof(float)/sizeof(char));
}

checksum_t checksum(const double values[], size_t size)
{
  return checksum(reinterpret_cast<const char*>(&values[0]),size*sizeof(double)/sizeof(char));
}

checksum_t checksum(const checksum_t values[], size_t size)
{
  return checksum(reinterpret_cast<const char*>(&values[0]),size*sizeof(checksum_t)/sizeof(char));
}

} // namespace internals
} // namespace atlas
