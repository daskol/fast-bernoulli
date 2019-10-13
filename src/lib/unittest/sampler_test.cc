/**
 * \file sampler-test.cc
 */

#include <gtest/gtest.h>
#include <array>
#include <cmath>
#include "sampler.h"

using NFastBernoulli::CreateSampler;
using NFastBernoulli::EInstructionSet;
using NFastBernoulli::TRng;
using aligned_ptr = std::unique_ptr<void, decltype(&std::free)>;

TEST(AlignedPtr, SimpleExpansion) {
    auto ptr = NFastBernoulli::MakeAligned(1);
    ptr.Get<uint8_t>()[0] = 0b10101010;
    auto res = NFastBernoulli::Expand(ptr);

    ASSERT_EQ(res.size(), 8);

    for (size_t i = 0; i != 8; ++i) {
        EXPECT_EQ(res[i], i % 2);
    }
}

TEST(SamplerCreation, Base) {
    TRng rng;
    alignas(64) std::array<uint8_t, 32> buf;
    auto sampler = CreateSampler(0.5);
    ASSERT_TRUE(sampler);
    sampler(rng, buf.data(), buf.size());
}

template <typename IteratorType>
auto EstimateStatistics(IteratorType begin, IteratorType end) {
    using elemType = typename std::remove_reference<decltype(*begin)>::type;
    size_t elemSize = sizeof(elemType);
    size_t nobits = 8 * elemSize;
    size_t nnz = 0;
    size_t length = 0;

    for (auto it = begin; it != end; ++it) {
        for (size_t bit = 0; bit != nobits; ++bit, ++length) {
            nnz += (*it) & static_cast<elemType>(1);
            *it = (*it) >> 1;
        }
    }

    double mean = static_cast<double>(nnz) / length;
    double std = 0;

    for (auto it = begin; it != end; ++it) {
        for (size_t bit = 0; bit != nobits; ++bit) {
            std = std::pow(((*it) & static_cast<elemType>(1)) - mean, 2);
            *it = (*it) >> 1;
        }
    }

    return std::make_tuple(mean, std::sqrt(std / nobits));
}

void BootstrapStatistics(EInstructionSet ise, bool useJit, bool useStdLib) {
    TRng rng;
    auto prob = 0.6;
    auto sampler = CreateSampler({
        .Probability_   = prob,
        .Tolerance_     = 1e-5,
        .Ise_           = EInstructionSet::General,
        .UseJit_        = useJit,
        .UseStdSampler_ = useStdLib,
    });

    constexpr size_t nobits = 256 * 40000; // 10240000 bits = 1250 kiBytes
    size_t outsize = nobits / 8;
    size_t bufsize = sampler.Get()->GetBufferSize(nobits);

    aligned_ptr buf{std::aligned_alloc(32, bufsize), &std::free};
    auto ptr = static_cast<char *>(buf.get());
    bool ok = sampler(rng, ptr, bufsize);

    auto [mean, std] = EstimateStatistics(ptr, ptr + outsize);

    EXPECT_NEAR(mean, prob, 1e-3)
        << "mean = " << mean
        << " std = " << std;
}

TEST(StatCorrectness, StdSampler) {
    SCOPED_TRACE("StdSampler");
    BootstrapStatistics(EInstructionSet::Auto, false, true);
}

TEST(StatCorrectness, GenericSampler) {
    SCOPED_TRACE("GenericSampler");
    BootstrapStatistics(EInstructionSet::General, false, false);
}

TEST(StatCorrectness, AvxSampler) {
    SCOPED_TRACE("AvxSampler");
    BootstrapStatistics(EInstructionSet::AVX, false, false);
}

#ifdef USE_JIT_EXECUTOR

TEST(StatCorrectness, JitGenericSampler) {
    SCOPED_TRACE("JitGenericSampler");
    BootstrapStatistics(EInstructionSet::General, true, false);
}

TEST(StatCorrectness, JitAvxSampler) {
    SCOPED_TRACE("JitAvxSampler");
    BootstrapStatistics(EInstructionSet::AVX, true, false);
}

#endif
