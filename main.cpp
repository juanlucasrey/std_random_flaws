#include "philox_engine.hpp"
#include "subtract_with_carry_engine.hpp"

#include <cassert>
#include <cstdint>
#include <limits>
#include <random>

// #include <iostream>

namespace detail {
template <std::size_t N, class T> inline constexpr auto pow(T base) -> T {
  if constexpr (N == 1) {
    return base;
  } else if constexpr (N % 2 == 0) {
    T power = pow<N / 2>(base);
    return power * power;

  } else {
    T power = pow<N / 2>(base);
    return base * power * power;
  }
};
} // namespace detail

template <class RNG>
constexpr auto period(RNG const & /* rng */) -> unsigned long long {
  constexpr auto w = RNG::word_size;
  constexpr auto r = RNG::long_lag;
  constexpr auto s = RNG::short_lag;
  return detail::pow<r>(w) - detail::pow<s>(w);
}

// as defined in https://en.wikipedia.org/wiki/Linear_congruential_generator
template <class UIntType>
using randq1 =
    std::linear_congruential_engine<UIntType, 1664525, 1013904223, 0>;

int main() {
  int result = 0;

  // issue 1
  {
    std::subtract_with_carry_engine<std::uint_fast32_t, 16, 2, 4> rng1;
    std::subtract_with_carry_engine<std::uint_fast32_t, 16, 2, 4> rng2;

    constexpr unsigned long long full_cycle = period(rng1); // 65280

    rng2.discard(full_cycle);

    assert(rng1 != rng2);     // this should be equal!!
    assert(rng1() == rng2()); // first simulation is equal
    assert(rng1 == rng2);     // then state is the same

    // proposed fix
    stdfix::subtract_with_carry_engine<std::uint_fast32_t, 16, 2, 4> rng1_fix;
    stdfix::subtract_with_carry_engine<std::uint_fast32_t, 16, 2, 4> rng2_fix;
    rng2_fix.discard(full_cycle);
    assert(rng1_fix == rng2_fix);
  }

  // issue 2
  {
    using rand_32 = randq1<std::uint32_t>;
    using rand_64 = randq1<std::uint64_t>;
    using rand_32_fast = randq1<std::uint_fast32_t>;

    // uint32 and uint64 have different max values, so distribution generators
    // will behave differently
    {
      static_assert(rand_32::max() != rand_64::max());

      rand_32 rng32;
      rand_64 rng64;

      std::uniform_real_distribution<> distrib;
      for (std::size_t j = 0; j < 10; j++) {
        // different float values generated
        assert(distrib(rng32) != distrib(rng64));
      }
    }

    // std::numeric_limits<std::uint_fast32_t>::digits
    // is implementation dependent!!

    if constexpr (std::numeric_limits<std::uint_fast32_t>::digits == 32) {

      // uint_fast32_t behaves like uint32_t
      assert(rand_32::max() ==
             rand_32_fast::max()); // we don't use static_assert because
                                   // compilation will fail

      rand_32 rng32;
      rand_32_fast rng32_fast;

      std::uniform_real_distribution<> distrib;
      for (std::size_t j = 0; j < 10; j++) {
        // same float values generated
        assert(distrib(rng32) == distrib(rng32_fast));
      }

    } else if constexpr (std::numeric_limits<std::uint_fast32_t>::digits ==
                         64) {

      // uint_fast32_t behaves like uint64_t
      assert(rand_64::max() ==
             rand_32_fast::max()); // we don't use static_assert because
                                   // compilation will fail

      rand_64 rng64;
      rand_32_fast rng32_fast;

      std::uniform_real_distribution<> distrib;
      for (std::size_t j = 0; j < 10; j++) {
        // same float values generated
        assert(distrib(rng64) == distrib(rng32_fast));
      }
    }

    // issue 3
    {
      stdmock::philox4x32 rng1;
      stdmock::philox4x64 rng2;
      // this version of discard follows strictly what std describes
      rng1.discard(10000 - 1);
      rng2.discard(10000 - 1);
      // and gives different numbers!
      assert(rng1() != 1955073260U);
      assert(rng2() != 3409172418970261260U);

      stdmock::philox4x32 rng1_fix;
      stdmock::philox4x64 rng2_fix;
      // this version of discard fixes the implementation as to reproduce the
      // desired number
      rng1_fix.discard<true>(10000 - 1);
      rng2_fix.discard<true>(10000 - 1);
      assert(rng1_fix() == 1955073260U);
      assert(rng2_fix() == 3409172418970261260U);
    }
  }

  return result;
}
