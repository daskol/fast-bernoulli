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

using NFastBernoulli::EInstructionSet;

static void BM_Sampler(benchmark::State &state, EInstructionSet ise) {
    NFastBernoulli::TRng rng;
    auto sampler = NFastBernoulli::CreateSampler({
        .Probability_ = 0.6,
        .Tolerance_   = 1e-3,
        .Ise_         = ise,
    });
    auto size = sampler.Get()->GetBufferSize();
    auto nobytes = size * state.range(0);
    auto buf = std::aligned_alloc(32, nobytes);
    for (auto _ : state) {
        sampler(rng, buf, nobytes);
    }
    std::free(buf);
    state.SetBytesProcessed(state.range(0) * state.iterations());
}

static void BM_GenericSampler(benchmark::State &state) {
    BM_Sampler(state, EInstructionSet::General);
}

BENCHMARK(BM_GenericSampler)
        -> RangeMultiplier(2)
        -> Range(1, 128);

static void BM_AvxSampler(benchmark::State &state) {
    BM_Sampler(state, NFastBernoulli::EInstructionSet::AVX);
}

BENCHMARK(BM_AvxSampler)
        -> RangeMultiplier(2)
        -> Range(1, 128);
