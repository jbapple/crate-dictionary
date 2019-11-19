extern "C" {
#include "pd512.h"
}

#include <iostream>

using namespace std;

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

int main(){
  __m512i x = {INT64_C(0x5555'5555'5555'5555), ((INT64_C(1) << 32) - 1),
               0x0123'4567'89ab'cdef,          0x0123'4567'89ab'cdef,
               0x0123'4567'89ab'cdef,          0x0123'4567'89ab'cdef,
               0x0123'4567'89ab'cdef,          0x0123'4567'89ab'cdef};

  uint64_t result = 0;
  for (uint64_t i = 0; i < 1'000'000'000; ++i) {
    // Prevent overlapping execution by serializing on result
    result += pd_find_64(i & 63, (i >> 6) + result, &x);
  }
  cout << result << endl;
}
