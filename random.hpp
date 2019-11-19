#include <cstring>

#include <algorithm>
#include <random>

class Generator {
  std::mt19937_64 engine_;

 public:
  Generator(const char* seed) : engine_() {
    decltype(engine_)::result_type v;
    std::memcpy(&v, seed, std::min(sizeof(v), std::strlen(seed)));
    engine_.seed(v);
  }
  decltype(engine_)::result_type operator()() { return engine_(); }
};
