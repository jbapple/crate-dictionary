extern "C" {
#include "cuckoo-filter-bucket.h"
}

#include <cassert>

#include <iostream>

using namespace std;

#include "random.hpp"

void test(int argc, char ** argv) {
  assert(argc > 0);
  assert(argc < 2);
  Generator g(argv[argc - 1]);
  uint64_t bucket = g();
  uint8_t value = g();
  cout << boolalpha << hasvalue8(bucket, value) << endl;
  value = bucket;
  cout << hasvalue8(bucket, value) << endl;
  value = bucket >> 8;
  cout << hasvalue8(bucket, value) << endl;
  value = bucket >> 16;
  cout << hasvalue8(bucket, value) << endl;
  value = bucket >> 24;
  cout << hasvalue8(bucket, value) << endl;
  value = bucket >> 32;
  cout << hasvalue8(bucket, value) << endl;
}

int main() {
  uint64_t result = 0;
  for (uint64_t i = 0; i < 1'000'000'000; ++i) {
    // Prevent overlapping execution by serializing on result
    result += hasvalue8(i, (i >> 32) + result);
  }
  cout << result << endl;
}
