/**
 * \file fast-bernoulli_bench.cc
 */

#include <benchmark/benchmark.h>

#include "fast-bernoulli.h"

static void BM_StdBernoulli(benchmark::State& state) {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::size_t size = 32 * 16;
    std::unique_ptr<void, decltype(&std::free)> data{
        std::aligned_alloc(32, size),
        &std::free,
    };

    auto ptr = data.get();
    auto bernoulli = NFastBernoulli::TStdBernoulli(0.6);

    for (auto _ : state) {
        bernoulli.Generate(rng, ptr, size);
    }
}

BENCHMARK(BM_StdBernoulli);

static void BM_DummyBernoulli(benchmark::State& state) {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::size_t size = 32 * 16;
    std::unique_ptr<void, decltype(&std::free)> data{
        std::aligned_alloc(32, size),
        &std::free,
    };

    auto ptr = data.get();
    auto bernoulli = NFastBernoulli::TDummyBernoulli(0.6);

    for (auto _ : state) {
        bernoulli.Generate(rng, ptr, size);
    }
}

BENCHMARK(BM_DummyBernoulli);
