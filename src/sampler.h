/**
 * \file sampler.h
 */

#pragma once

#include <random>
#include <memory>

namespace NFastBernoulli {

enum EStatus : uint32_t {
    EOk = 0,
    EWrongPtrAlignment = 1,
    EWrongSizeAlignment = 2,
};

using TRng = std::mt19937_64;

/**
 * Class ISampler defines a common interface to any block generator of
 * Bernoulli random variables (RVs).
 */
class ISampler {
public:
    virtual ~ISampler(void) = default;
    virtual EStatus Sample(TRng &rng, void *begin, size_t size) noexcept = 0;
    virtual EStatus Sample(void *begin, size_t size) noexcept = 0;
};

/**
 * Class TStdBernoulli implements block generator based on built-in sampler
 * from Bernoulli distribution which is based on sampling from uniform
 * distribution and comparision to threshold probability.
 */
class TStdBernoulli : public ISampler {
public:
    TStdBernoulli(double proba) noexcept
        : Dist_{proba}
    {}

    virtual EStatus Sample(TRng &rng, void *begin, size_t size) noexcept override;
    virtual EStatus Sample(void *begin, size_t size) noexcept override;

private:
    std::bernoulli_distribution Dist_;
};

/**
 * Class TDummyBernoulli is a implementation from scratch which is based on the
 * original idea and uses AVX2 instrution set.
 */
class TDummyBernoulli : public ISampler {
public:
    constexpr TDummyBernoulli(double proba) noexcept
        : Proba_{proba}
    {}

    virtual ~TDummyBernoulli(void) = default;
    virtual EStatus Sample(TRng &rng, void *begin, size_t size) noexcept override;
    virtual EStatus Sample(void *begin, size_t size) noexcept override;

private:
    double Proba_;
};

class TSamplerPtr {
public:
    TSamplerPtr(void) noexcept = default;
    TSamplerPtr(std::unique_ptr<ISampler> &&sampler) noexcept
        : Sampler_{std::move(sampler)}
    {}

    inline bool operator()(TRng &rng, void *ptr, size_t size) noexcept {
        return Sampler_->Sample(rng, ptr, size);
    }

    inline ISampler *get(void) noexcept {
        return Sampler_.get();
    }

private:
    std::unique_ptr<ISampler> Sampler_ = {};
};

TSamplerPtr CreateBernoulliSampler(double proba);

} // namespace NFastBernoulli
