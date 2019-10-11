/**
 * \file sampler.cc
 */

#include <benchmark/benchmark.h>

#include "sampler.h"

using NFastBernoulli::EInstructionSet;
using aligned_ptr = std::unique_ptr<void, decltype(&std::free)>;

inline void EstimateThroughput(benchmark::State &state) {
    constexpr size_t blockSize = 32; // bytes
    state.SetBytesProcessed(blockSize * state.range(0) * state.iterations());
}

/**
 * In benchmark BM_RandomNumberGeneration maximal rate of random bit generation
 * is estimated. The measured value is a baseline for any other improvement.
 */
static void BM_RandomNumberGeneration(benchmark::State& state) {
    NFastBernoulli::TRng rng;
    size_t blockSize = 32; // bytes
    size_t noblocks = state.range(0);
    size_t nobitsPerCall = sizeof(NFastBernoulli::TRng::result_type);
    size_t nocalls = noblocks * blockSize / nobitsPerCall;

    for (auto _ : state) {
        for (size_t i = 0; i != nocalls; ++i) {
            benchmark::DoNotOptimize(rng());
        }
    }

    EstimateThroughput(state);
}

BENCHMARK(BM_RandomNumberGeneration)
    ->RangeMultiplier(2)
    ->Range(1, 128);

/**
 * Benchmark BM_Sampler estimates how efficiently samples from Bernoulli
 * distribution could be obtained with standard library subject to every bit of
 * output corresponds to every instance of a random variable.
 */
static void BM_StdSampler(benchmark::State& state) {
    NFastBernoulli::TRng rng;
    auto sampler = NFastBernoulli::CreateSampler({
        .Probability_   = 0.6,
        .UseStdSampler_ = true,
    });

    size_t noblocks =  state.range();
    size_t size = 16 * noblocks * sampler.Get()->GetBufferSize();
    aligned_ptr buf(std::aligned_alloc(32, size), &std::free);
    auto *ptr = buf.get();

    for (auto _ : state) {
        sampler(rng, ptr, size);
    }

    EstimateThroughput(state);
}

BENCHMARK(BM_StdSampler)
    ->RangeMultiplier(2)
    ->Range(1, 128);

static void BM_Sampler(benchmark::State &state, EInstructionSet ise) {
    NFastBernoulli::TRng rng;
    auto sampler = NFastBernoulli::CreateSampler({
        .Probability_ = 0.6,
        .Tolerance_   = 1e-3,
        .Ise_         = ise,
    });

    auto size = sampler.Get()->GetBufferSize();
    auto nobytes = size * state.range(0);
    aligned_ptr buf(std::aligned_alloc(32, nobytes), &std::free);
    auto *ptr = buf.get();

    for (auto _ : state) {
        sampler(rng, ptr, nobytes);
    }

    EstimateThroughput(state);
}

/**
 * Benchmark BM_GenericSampler estimates performance of implementation of the
 * algorithm with generic instruction set without extensions.
 */
static void BM_GenericSampler(benchmark::State &state) {
    BM_Sampler(state, EInstructionSet::General);
}

BENCHMARK(BM_GenericSampler)
        -> RangeMultiplier(2)
        -> Range(1, 128);

/**
 * Benchmark BM_AvxSampler estimates performance of implementation of the
 * algorithm with AVX2 vector instruction extensions.
 */
static void BM_AvxSampler(benchmark::State &state) {
    BM_Sampler(state, NFastBernoulli::EInstructionSet::AVX);
}

BENCHMARK(BM_AvxSampler)
        -> RangeMultiplier(2)
        -> Range(1, 128);

static void BM_JitSampler(benchmark::State &state, EInstructionSet ise) {
    NFastBernoulli::TRng rng;
    auto sampler = NFastBernoulli::CreateSampler({
        .Probability_ = 0.6,
        .Tolerance_   = 1e-3,
        .Ise_         = ise,
        .UseJit_      = true,
    });

    auto size = sampler.Get()->GetBufferSize();
    auto nobytes = size * state.range(0);
    aligned_ptr buf(std::aligned_alloc(32, nobytes), &std::free);
    auto *ptr = buf.get();

    for (auto _ : state) {
        sampler(rng, ptr, nobytes);
    }

    EstimateThroughput(state);
}

/**
 * Benchmark BM_JitGenericSampler estimates performance of implementation of
 * the algorithm with generic instruction set without extensions which was
 * compiled with JIT in run-time.
 */
static void BM_JitGenericSampler(benchmark::State &state) {
    BM_Sampler(state, EInstructionSet::General);
}

BENCHMARK(BM_JitGenericSampler)
        -> RangeMultiplier(2)
        -> Range(1, 128);

/**
 * Benchmark BM_JitAvxSampler estimates performance of implementation of the
 * algorithm with AVX2 vector instruction extensions which was compiled with
 * JIT in run-time.
 */
static void BM_JitAvxSampler(benchmark::State &state) {
    BM_JitSampler(state, NFastBernoulli::EInstructionSet::AVX);
}

BENCHMARK(BM_JitAvxSampler)
        -> RangeMultiplier(2)
        -> Range(1, 128);
