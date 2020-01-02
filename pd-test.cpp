#if defined(NDEBUG)
#error This file depends on assert. Please compile without the NDEBUG macro defined
#endif

extern "C" {
#include "pd512.h"
}

#include <iostream>
using namespace std;

#if 0
template<auto MAX_QUOTIENT, auto CAPACITY>
// find an 8-bit value in a pocket dictionary with quotients in [0,64) and 49 values
inline bool pd_find(int64_t quot, char rem, const __m512i* pd) {
  assert(quot < MAX_QUOTIENT);
  static_assert(MAX_QUOTIENT + CAPACITY + CAPACITY * CHAR_BIT <= 512);
  constexpr HEADER_WORDS = (MAX_QUOTIENT + CAPACITY + 63) / 64;
  // The header has size MAX_QUOTIENT + CAPACITY
  uint64_t header[HEADER_WORDS];
  // We need to copy MAX_QUOTIENT + CAPACITY bits, but we copy slightly more and mask out
  // the ones we don't care about.
  //
  // memcpy is the only defined punning operation
  memcpy(header, pd, HEADER_WORDS * sizeof(header[0]));
  header[HEADER_WORDS - 1] =
      header[HEADER_WORDS - 1] & ((UINT64_C(1) << ((MAX_QUOTIENT + CAPACITY) % 64)) - 1);
// STOPPED HERE

// assert(_mm_popcnt_u64(header[0]) + _mm_popcnt_u64(header[1]) == 64);


  
  // [begin,end) is the indexes of the values that have quot as their quotient. the range
  // is exclusive and end is at most 49
  const int64_t p = _mm_popcnt_u64(header[0]);
  uint64_t begin = 0;
  if (quot > 0) {
    begin = 1 + ((quot <= p) ? select64(header[0], quot - 1)
                             : select64(header[1], quot - 1 - p));
  }
  const uint64_t end =
      (quot < p) ? select64(header[0], quot) : select64(header[1], quot - p);
  assert(begin <= end);
  assert(end <= 49);
  const __m512i target = _mm512_set1_epi8(rem);
  uint64_t v = _mm512_cmpeq_epu8_mask(target, *pd);
  // round up to remove the header
  v = v >> ((64 + 49 + CHAR_BIT - 1) / CHAR_BIT);
  return (v & ((UINT64_C(1) << end) - 1)) >> begin;
}
#endif


static void test_select() {
  assert(select64(0,0) == 64);
  assert(select64(1, 0) == 0);
  assert(select64(1 << 10, 0) == 10);
  assert(select64(UINT64_C(1) << 63, 0) == 63);
  assert(select64((UINT64_C(1) << 63) | 1, 0) == 0);
  assert(select64((UINT64_C(1) << 63) | 1, 1) == 63);
}

static void test_pd_find_64() {
  __m512i x = {~INT64_C(0), 0, 0, 0, 0, 0, 0, 0};
  assert(false == pd_find_64(0, 0, &x));
  assert(false == pd_find_64(1, 0, &x));
  assert(false == pd_find_64(0, 1, &x));
  x[0] = ~INT64_C(1);
  x[1] = 1;
  assert(true == pd_find_64(0, 0, &x));
  assert(false == pd_find_64(1, 0, &x));
  assert(false == pd_find_64(0, 1, &x));
  x[0] = ~INT64_C(2);
  assert(false == pd_find_64(0, 0, &x));
  assert(true == pd_find_64(1, 0, &x));
  assert(false == pd_find_64(0, 1, &x));
  x[0] = ~INT64_C(1);
  x[1] = 1 | (INT64_C(1) << 56);
  assert(false == pd_find_64(0, 0, &x));
  assert(false == pd_find_64(1, 0, &x));
  assert(true == pd_find_64(0, 1, &x));
  x[1] = static_cast<int64_t>(1 | (UINT64_C(0xAB) << 56));
  assert(true == pd_find_64(0, static_cast<char>(0xAB), &x));
}

int main() {
  test_select();
  test_pd_find_64();
  cout << "All tests pass\n";
}
