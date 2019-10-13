/**
 * \file executor_bench.cc
 */

#include <benchmark/benchmark.h>
#include <array>
#include "executor.h"

template <typename ExecutorType>
static void BM_Executor(benchmark::State &state) {
    using NFastBernoulli::EOp;

    ExecutorType exe({
        .Ops_         = {
            EOp::Not, EOp::And, EOp::Not, EOp::And, EOp::And, EOp::Not,
            EOp::And, EOp::And, EOp::Not, EOp::And, EOp::And, EOp::Not,
            EOp::And, EOp::And, EOp::Not, EOp::And, EOp::And, EOp::Not,
            EOp::And, EOp::And, EOp::Not, EOp::And, EOp::And,
        },
        .NoOps_       = 23,
        .NoSrcBlocks_ = 16,
    });

    alignas(32) static std::array<uint64_t, 4> dst;
    alignas(32) static std::array<uint64_t, 64> src;

    for (auto _ : state) {
        exe.Execute(src.data(), dst.data(), 1);
    }
}

BENCHMARK_TEMPLATE(BM_Executor, NFastBernoulli::TGeneralExecutor);
BENCHMARK_TEMPLATE(BM_Executor, NFastBernoulli::TAvxExecutor);
