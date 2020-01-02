#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "immintrin.h"

// returns the position (starting from 0) of the jth set bit of x.
inline uint64_t select64(uint64_t x, int64_t j) {
  const uint64_t y = _pdep_u64(UINT64_C(1) << j, x);
  return _tzcnt_u64(y);
}

// returns the number of zeros before the jth (counting from 0) set bit of x
inline uint64_t nth64(uint64_t x, int64_t j) {
  const uint64_t y = select64(x, j);
  assert(y < 64);
  const uint64_t z = x & ((UINT64_C(1) << y) - 1);
  return y - _mm_popcnt_u64(z);
}

// find an 8-bit value in a pocket dictionary with quotients in [0,64) and 49 values
inline bool pd_find_64(int64_t quot, char rem, const __m512i* pd) {
  assert(quot < 64);
  // The header has size 64 + 49
  uint64_t header[2];
  // We need to copy (64+49) bits, but we copy slightly more and mask out the ones we
  // don't care about.
  //
  // memcpy is the only defined punning operation
  memcpy(header, pd, 16);
  header[1] = header[1] & ((UINT64_C(1) << 49) - 1);
  assert(_mm_popcnt_u64(header[0]) + _mm_popcnt_u64(header[1]) == 64);
  // [begin,end) is the indexes of the values that have quot as their quotient. the range
  // is exclusive and end is at most 49
  const int64_t p = _mm_popcnt_u64(header[0]);
  uint64_t begin = 0;
  if (quot > 0) {
    begin = ((quot <= p) ? nth64(header[0], quot - 1)
                         : ((64 - p) + nth64(header[1], quot - 1 - p)));
  }
  const uint64_t end =
      (quot < p) ? nth64(header[0], quot) : ((64 - p) + nth64(header[1], quot - p));
  assert(begin <= end);
  assert(end <= 49);
  const __m512i target = _mm512_set1_epi8(rem);
  uint64_t v = _mm512_cmpeq_epu8_mask(target, *pd);
  // round up to remove the header
  v = v >> ((64 + 49 + CHAR_BIT - 1) / CHAR_BIT);
  return (v & ((UINT64_C(1) << end) - 1)) >> begin;
}

// find an 8-bit value in a pocket dictionary with quotients in [0,50) and 51 values
inline bool pd_find_50(int64_t quot, char rem, const __m512i* pd) {
  assert(quot < 50);
  // The header has size 50 + 51
  uint64_t header[2];
  // We need to copy (50+51) bits, but we copy slightly more and mask out the ones we
  // don't care about.
  //
  // memcpy is the only defined punning operation
  memcpy(header, pd, 13);
  // Requires little-endianness
  header[1] = header[1] & ((UINT64_C(1) << (50 + 51 - 64)) - 1);
  assert(_mm_popcnt_u64(header[0]) + _mm_popcnt_u64(header[1]) == 50);
  // [begin,end) is the indexes of the values that have quot as their quotient. the range
  // is exclusive and end is at most 51
  const int64_t p = _mm_popcnt_u64(header[0]);
  uint64_t begin = 0;
  if (quot > 0) {
    begin = ((quot <= p) ? nth64(header[0], quot - 1)
                         : ((50 - p) + nth64(header[1], quot - 1 - p)));
  }
  const uint64_t end =
      (quot < p) ? nth64(header[0], quot) : ((50 - p) + nth64(header[1], quot - p));
  assert(begin <= end);
  assert(end <= 51);
  const __m512i target = _mm512_set1_epi8(rem);
  uint64_t v = _mm512_cmpeq_epu8_mask(target, *pd);
  // round up to remove the header
  v = v >> ((51 + 50 + CHAR_BIT - 1) / CHAR_BIT);
  return (v & ((UINT64_C(1) << end) - 1)) >> begin;
}
