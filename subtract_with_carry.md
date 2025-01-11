---
title: "Subtract With Carry Engine Correction Proposal"
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

# Subtract With Carry

The subtract with carry engine has the particularity that the states can be degenerated. More concretely, this means that there can be 2 different internal states that are actually different but provide the same numbers. This could be interpreted as the fact that these internal states are over-parametrised. This is problematic since in some cases, 2 subtract with carry instances (rng1 and rng2) can return the same numbers while having (rng1 != rng2).\

We could consider that the "correct" internal state could be the one that the engine has after a full cycle has been generated. And example of this problem is shown in [this test](https://github.com/juanlucasrey/std_random_flaws/blob/main/main.cpp#L51), where 2 subtract with carry isntances are not equal despite generating the same numbers.



