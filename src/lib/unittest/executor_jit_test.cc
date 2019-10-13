/**
 * \file executor_jit.cc
 */

#include <gtest/gtest.h>
#include <array>
#include "executor_jit.h"

using NFastBernoulli::CreateJitExecutor;
using NFastBernoulli::EInstructionSet;
using NFastBernoulli::EOp;

TEST(JitExecutor, NoOps) {
    alignas(64) std::array<uint64_t, 4> dst;
    alignas(64) std::array<uint64_t, 4> src{0ul, 1ul, 2ul, 3ul};
    alignas(64) std::array<uint64_t, 4> res{0ul, 1ul, 2ul, 3ul};

    auto exe = CreateJitExecutor({.NoSrcBlocks_ = 1}, EInstructionSet::AVX);

    ASSERT_TRUE(exe);

    exe->Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}

TEST(JitExecutor, SingleNot) {
    alignas(64) std::array<uint64_t, 4> src{0ul, 1ul, 2ul, 3ul};
    alignas(64) std::array<uint64_t, 4> dst;
    alignas(64) std::array<uint64_t, 4> res{~0ul, ~1ul, ~2ul, ~3ul};

    auto exe = CreateJitExecutor({
        .Ops_         = {EOp::Not},
        .NoOps_       = 1,
        .NoSrcBlocks_ = 1,
    }, EInstructionSet::AVX);

    ASSERT_TRUE(exe);

    exe->Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}

TEST(JitExecutor, SingleAnd) {
    alignas(64) std::array<uint64_t, 4> dst;
    alignas(64) std::array<uint64_t, 4> res{0ul, 1ul, 2ul, 3ul};
    alignas(64) std::array<uint64_t, 8> src{
        0ul, 1ul, 2ul, 3ul, 4ul, 5ul, 6ul, 7ul,
    };

    auto exe = CreateJitExecutor({
        .Ops_         = {EOp::And},
        .NoOps_       = 1,
        .NoSrcBlocks_ = 2,
    }, EInstructionSet::AVX);

    ASSERT_TRUE(exe);

    exe->Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}
