/**
 * \file executor_bench.cc
 */

#include <benchmark/benchmark.h>
#include "executor_testdata.inc"
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

    for (auto _ : state) {
        exe.Execute(::src.data(), ::dst.data(), 1);
    }
}

BENCHMARK_TEMPLATE(BM_Executor, NFastBernoulli::TGeneralExecutor);
BENCHMARK_TEMPLATE(BM_Executor, NFastBernoulli::TAvxExecutor);
