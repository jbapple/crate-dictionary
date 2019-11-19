#include <stdint.h>

// returns a non-zero result if and only if one of the low bytes in x is 0
inline uint32_t haszero8(uint32_t x) {
  return (x - UINT32_C(0x01010101)) & (~x) & UINT32_C(0x80808080);
}

// Finds an eight-bit value n in the low 32 bits of x
inline bool hasvalue8(uint32_t x, uint8_t n) {
  return haszero8(x ^ (UINT32_C(0x01010101) * n));
}
