/**
 * \file executor_test.cc
 */

#include <gtest/gtest.h>
#include "executor_testdata.inc"
#include "executor.h"

using NFastBernoulli::EOp;
using NFastBernoulli::TAvxExecutor;
using NFastBernoulli::TGeneralExecutor;
using NFastBernoulli::Quantize;

TEST(Quantize, Zero) {
    auto plan = Quantize(0.0, 1e-6);
    ASSERT_TRUE(plan);
}

TEST(Quantize, One) {
    auto plan = Quantize(1.0, 1e-6);
    ASSERT_TRUE(plan);
}

TEST(Quantize, Half) {
    auto plan = Quantize(0.5, 1e-6);
    ASSERT_TRUE(plan);
    EXPECT_EQ(plan->NoSrcBlocks_, 1);
    EXPECT_EQ(plan->NoOps_, 0);
}

TEST(Quantize, Quater) {
    auto plan = Quantize(0.25, 1e-6);
    ASSERT_TRUE(plan);
    EXPECT_EQ(plan->NoSrcBlocks_, 2);
    EXPECT_EQ(plan->NoOps_, 1);
    EXPECT_EQ(plan->Ops_[0], EOp::And);
}

TEST(Quantize, ThreeQuaters) {
    auto plan = Quantize(0.75, 1e-6);
    ASSERT_TRUE(plan);
    EXPECT_EQ(plan->NoSrcBlocks_, 2);
    EXPECT_EQ(plan->NoOps_, 2);
    EXPECT_EQ(plan->Ops_[0], EOp::And);
    EXPECT_EQ(plan->Ops_[1], EOp::Not);
}

TEST(Quantize, p625) {
    auto plan = Quantize(0.625, 1e-6);
    ASSERT_TRUE(plan);
    EXPECT_EQ(plan->NoSrcBlocks_, 3);
    EXPECT_EQ(plan->NoOps_, 4);
    EXPECT_EQ(plan->Ops_[0], EOp::And);
    EXPECT_EQ(plan->Ops_[1], EOp::Not);
    EXPECT_EQ(plan->Ops_[2], EOp::And);
    EXPECT_EQ(plan->Ops_[3], EOp::Not);
}

TEST(Quantize, p875) {
    auto plan = Quantize(0.875, 1e-6);
    ASSERT_TRUE(plan);
    EXPECT_EQ(plan->NoSrcBlocks_, 3);
    EXPECT_EQ(plan->NoOps_, 3);
    EXPECT_EQ(plan->Ops_[0], EOp::And);
    EXPECT_EQ(plan->Ops_[1], EOp::And);
    EXPECT_EQ(plan->Ops_[2], EOp::Not);
}

TEST(Quantize, TooPrecise) {
    auto plan = Quantize(1e-30, 1e-70);
    ASSERT_FALSE(plan);
}

TEST(GeneralExecutor, NoOps) {
    std::array<uint64_t, 4> dst;
    std::array<uint64_t, 4> src{0ul, 1ul, 2ul, 3ul};
    std::array<uint64_t, 4> res{0ul, 1ul, 2ul, 3ul};

    TGeneralExecutor({.NoSrcBlocks_ = 1}).Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}

TEST(GeneralExecutor, SingleNot) {
    std::array<uint64_t, 4> src{0ul, 1ul, 2ul, 3ul};
    std::array<uint64_t, 4> dst;
    std::array<uint64_t, 4> res{~0ul, ~1ul, ~2ul, ~3ul};

    TGeneralExecutor({
        .Ops_         = {EOp::Not},
        .NoOps_       = 1,
        .NoSrcBlocks_ = 1,
    }).Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}

TEST(GeneralExecutor, SingleAnd) {
    std::array<uint64_t, 8> src{0ul, 1ul, 2ul, 3ul, 4ul, 5ul, 6ul, 7ul};
    std::array<uint64_t, 4> dst;
    std::array<uint64_t, 4> res{0ul, 2ul, 4ul, 6ul};

    TGeneralExecutor({
        .Ops_         = {EOp::And},
        .NoOps_       = 1,
        .NoSrcBlocks_ = 2,
    }).Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}

TEST(GeneralExecutor, MultipleOps) {
    TGeneralExecutor({
        .Ops_         = {
            EOp::And, EOp::And, EOp::Not, EOp::And, EOp::And, EOp::Not,
            EOp::And, EOp::And, EOp::Not, EOp::And, EOp::And, EOp::Not,
            EOp::And, EOp::And, EOp::Not, EOp::And, EOp::And, EOp::Not,
            EOp::And, EOp::And, EOp::Not, EOp::And, EOp::Not,
        },
        .NoOps_       = 23,
        .NoSrcBlocks_ = 16,
    }).Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}

TEST(AvxExecutor, NoOps) {
    alignas(64) std::array<uint64_t, 4> dst;
    alignas(64) std::array<uint64_t, 4> src{0ul, 1ul, 2ul, 3ul};
    alignas(64) std::array<uint64_t, 4> res{0ul, 1ul, 2ul, 3ul};

    TAvxExecutor({.NoSrcBlocks_ = 1}).Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}

TEST(AvxExecutor, SingleNot) {
    alignas(32) std::array<uint64_t, 4> src{0ul, 1ul, 2ul, 3ul};
    alignas(32) std::array<uint64_t, 4> dst;
    alignas(32) std::array<uint64_t, 4> res{~0ul, ~1ul, ~2ul, ~3ul};

    TAvxExecutor({
        .Ops_         = {EOp::Not},
        .NoOps_       = 1,
        .NoSrcBlocks_ = 1,
    }).Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}

TEST(AvxExecutor, SingleAnd) {
    alignas(32) std::array<uint64_t, 4> dst;
    alignas(32) std::array<uint64_t, 4> res{0ul, 1ul, 2ul, 3ul};
    alignas(32) std::array<uint64_t, 8> src{
        0ul, 1ul, 2ul, 3ul, 4ul, 5ul, 6ul, 7ul,
    };

    TAvxExecutor({
        .Ops_         = {EOp::And},
        .NoOps_       = 1,
        .NoSrcBlocks_ = 2,
    }).Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}

TEST(ExecutorCreation, GeneralExecutor) {
    alignas(64) std::array<uint64_t, 4> dst;
    alignas(64) std::array<uint64_t, 4> src{0ul, 1ul, 2ul, 3ul};
    alignas(64) std::array<uint64_t, 4> res{0ul, 1ul, 2ul, 3ul};
    auto exe = NFastBernoulli::CreateExecutor(0.5, 1e-3);

    ASSERT_TRUE(exe);

    exe->Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}

TEST(ExecutorCreation, AvxExecutor) {
    alignas(64) std::array<uint64_t, 4> dst;
    alignas(64) std::array<uint64_t, 4> src{0ul, 1ul, 2ul, 3ul};
    alignas(64) std::array<uint64_t, 4> res{0ul, 1ul, 2ul, 3ul};
    auto exe = NFastBernoulli::CreateExecutor({
        .Probability_ = 0.5,
        .Tolerance_ = 1e-3,
        .Isa_ = NFastBernoulli::EInstructionSet::AVX,
    });

    ASSERT_TRUE(exe);

    exe->Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}
