/**
 * \file executor_test.cc
 */

#include <gtest/gtest.h>
#include "executor_testdata.inc"
#include "executor.h"

using NFastBernoulli::EOp;
using NFastBernoulli::TAvxExecutor;
using NFastBernoulli::TGeneralExecutor;

TEST(GeneralExecutor, NoOps) {
    TGeneralExecutor({}).Execute(nullptr, nullptr, 0xffff);
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
            EOp::Not, EOp::And, EOp::Not, EOp::And, EOp::And, EOp::Not,
            EOp::And, EOp::And, EOp::Not, EOp::And, EOp::And, EOp::Not,
            EOp::And, EOp::And, EOp::Not, EOp::And, EOp::And, EOp::Not,
            EOp::And, EOp::And, EOp::Not, EOp::And, EOp::And,
        },
        .NoOps_       = 23,
        .NoSrcBlocks_ = 16,
    }).Execute(src.data(), dst.data(), 1);

    for (size_t i = 0; i != res.size(); ++i) {
        EXPECT_EQ(dst[i], res[i]);
    }
}

TEST(AvxExecutor, NoOps) {
    TAvxExecutor({}).Execute(nullptr, nullptr, 0xffff);
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
