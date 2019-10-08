/**
 * \file sampler.h
 */

#pragma once

#include <random>
#include <memory>

#include "common.h"

namespace NFastBernoulli {

enum EStatus : uint32_t {
    EOk = 0,
    ENotImplemented,
    EWrongPtrAlignment,
    EWrongSizeAlignment,
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
    virtual size_t GetBufferSize(size_t nobits = 256) const noexcept {
        if (size_t rem = nobits % 256; rem != 0) {
            nobits += 256 - rem;
        }
        return nobits / 8; // bytes
    }
};

class TSamplerPtr {
public:
    TSamplerPtr(void) noexcept = default;
    TSamplerPtr(std::unique_ptr<ISampler> &&sampler) noexcept
        : Sampler_{std::move(sampler)}
    {}

    explicit inline operator bool(void) const noexcept {
        return static_cast<bool>(Sampler_);
    }

    inline bool operator()(TRng &rng, void *ptr, size_t size) noexcept {
        return Sampler_->Sample(rng, ptr, size);
    }

    inline bool operator()(void *ptr, size_t size) noexcept {
        return Sampler_->Sample(ptr, size);
    }

    inline ISampler *Get(void) noexcept {
        return Sampler_.get();
    }

private:
    std::unique_ptr<ISampler> Sampler_ = {};
};

struct TSamplerOpts {
    double Probability_;
    double Tolerance_;
    EInstructionSet Ise_ = EInstructionSet::Auto;
    bool UseStdSampler_ = false;
};

TSamplerPtr CreateSampler(const TSamplerOpts &opts);
TSamplerPtr CreateSampler(double prob, double tol = 1e-3);

} // namespace NFastBernoulli
