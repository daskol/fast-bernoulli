/**
 * \file sampler-test.cc
 */

#include <gtest/gtest.h>
#include <array>
#include "sampler.h"

using NFastBernoulli::CreateSampler;
using NFastBernoulli::TRng;

TEST(SamplerCreation, Base) {
    TRng rng;
    alignas(64) std::array<uint8_t, 32> buf;
    auto sampler = CreateSampler(0.5);
    ASSERT_TRUE(sampler);
    sampler(rng, buf.data(), buf.size());
}
