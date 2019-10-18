/**
 * \file sampler.h
 */

#pragma once

#include <random>
#include <memory>
#include <vector>

#include <fast-bernoulli/cc/common.h>

namespace NFastBernoulli {

using TRng = std::mt19937_64;

/**
 * Class TAlignedPtr is semantically similar to std::unique_ptr<>. The only
 * difference that the pointer is assumed to be aligned. An instance of the
 * class should not be created directly. Function MakeAligned() should be used
 * instead.
 */
class TAlignedPtr {
public:
    TAlignedPtr(void) noexcept = default;
    TAlignedPtr(void * ptr, size_t size) noexcept
        : Ptr_{ptr, &std::free}
        , Size_{size}
    {}

    explicit operator bool(void) const noexcept {
        return static_cast<bool>(Ptr_);
    }

    void * Get(void) noexcept {
        return Ptr_.get();
    }

    void * const Get(void) const noexcept {
        return Ptr_.get();
    }

    template <typename T>
    T * Get(void) noexcept {
        return static_cast<T *>(Ptr_.get());
    }

    template <typename T>
    T * const Get(void) const noexcept {
        return static_cast<T * const>(Ptr_.get());
    }

    size_t Size(void) const noexcept {
        return Size_;
    }

private:
    std::unique_ptr<void, decltype(&std::free)> Ptr_ = {nullptr, &std::free};
    std::size_t Size_ = 0;
};

TAlignedPtr MakeAligned(size_t bytes) noexcept;

std::vector<bool> Expand(const TAlignedPtr &ptr, size_t nobits = 256);

template <typename T>
std::vector<T> Expand(const TAlignedPtr &ptr, size_t nobits = 256) {
    std::vector<T> expansion;
    auto begin = static_cast<const uint8_t *>(ptr.Get());
    auto end = begin + nobits / 8;
    for (auto it = begin; it != end; ++it) {
        for (size_t shift = 0; shift != 8; ++shift) {
            T bit = static_cast<T>(((*it) >> shift) & 1u);
            expansion.push_back(bit);
        }
    }
    return expansion;
}

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

    inline bool operator()(TRng &rng, TAlignedPtr &ptr) noexcept {
        return Sampler_->Sample(rng, ptr.Get(), ptr.Size());
    }

    inline bool operator()(TRng &rng, void *ptr, size_t size) noexcept {
        return Sampler_->Sample(rng, ptr, size);
    }

    inline bool operator()(void *ptr, size_t size) noexcept {
        return Sampler_->Sample(ptr, size);
    }

    inline size_t GetBufferSize(size_t nobits) const noexcept {
        return Sampler_->GetBufferSize(nobits);
    }

    inline ISampler *Get(void) noexcept {
        return Sampler_.get();
    }

    inline TAlignedPtr MakeBuffer(size_t nobits) const noexcept {
        return std::move(MakeAligned(GetBufferSize(nobits)));
    }

private:
    std::unique_ptr<ISampler> Sampler_ = {};
};

struct TSamplerOpts {
    double Probability_;
    double Tolerance_;
    EInstructionSet Ise_ = EInstructionSet::Auto;
    bool UseJit_ = false;
    bool UseStdSampler_ = false;
};

TSamplerPtr CreateSampler(const TSamplerOpts &opts);
TSamplerPtr CreateSampler(double prob, double tol = 1e-3);

} // namespace NFastBernoulli
