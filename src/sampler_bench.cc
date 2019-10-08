/**
 * \file sampler.cc
 */

#include <benchmark/benchmark.h>

#include "sampler.h"

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
        bernoulli.Sample(rng, ptr, size);
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
        bernoulli.Sample(rng, ptr, size);
    }
}

BENCHMARK(BM_DummyBernoulli);

static void BM_BaseSampler(benchmark::State &state) {
    NFastBernoulli::TRng rng;
    auto sampler = NFastBernoulli::CreateSampler(0.5);
    auto size = sampler.Get()->GetBufferSize();
    auto buf = std::aligned_alloc(32, size);
    for (auto _ : state) {
        sampler(rng, buf, size);
    }
    std::free(buf);
}

BENCHMARK(BM_BaseSampler);
