---
title: "Linear Congruent Engine Correction Proposal"
subtitle: "Visual inspection of various features of the framework"
document: D0000R1
date: today
audience:
  - Library Evolution
  - Library
revises: D0000R0
author:
  - name: Author Juan Lucas Rey
    email: <juanlucasrey@gmail.com>
toc: false
toc-depth: 1
---

# Linear Congruent Engine Corrections

The Linear congruent Engine does not set on the template parameters the number of bits the geenrated numbers are supposed to have. This in contrast with Mersenne Twister, substract with carry and philox engines, which all have std::size_t w as template parameter. This can lead to issues since the definition of a linear_congruential_engine with std::uint_fast32_t (as it is suggested in the cppreference page) now returns numbers that are implementation depedendent. This will not affect the actual numbers generated, but it will affect the distributinos where these numbers are used. This is because the numbers being returned by, for example, std::uniform_real_distribution, depends on the value of std::linear_congruential_engine<>::max().

To exemplify this problem, I have written [the following test](https://github.com/juanlucasrey/std_random_flaws/blob/main/main.cpp#L86), where a linear congruential engine generates different unifrm real distributed numbers, according to an implementation.
