/**
 * \file sampler.cc
 */

#include "sampler.h"
#include "executor.h"

#include <x86intrin.h>

namespace NFastBernoulli {

TAlignedPtr MakeAligned(size_t bytes) noexcept {
    return TAlignedPtr(std::aligned_alloc(32, bytes), bytes);
}

std::vector<bool> Expand(const TAlignedPtr &ptr, size_t nobits) {
    return std::move(Expand<bool>(ptr, nobits));
}

inline EStatus Validate(void *ptr, size_t size) {
    //  Pointer is not properly aligned.
    if (reinterpret_cast<size_t>(ptr) & 0b1111) {
        return EWrongPtrAlignment;
    }

    // Memory block is not properly aligned.
    if (size & 0b1111) {
        return EWrongSizeAlignment;
    }

    return EOk;
}

/**
 * Class TStdSampler implements block generator based on built-in sampler from
 * Bernoulli distribution which is based on sampling from uniform distribution
 * and comparision to threshold probability.
 */

class TStdSampler : public ISampler {
public:
    TStdSampler(double proba) noexcept
        : Dist_{proba}
    {}

    virtual EStatus Sample(TRng &rng, void *begin, size_t size) noexcept override;
    virtual EStatus Sample(void *begin, size_t size) noexcept override;

private:
    std::bernoulli_distribution Dist_;
};

EStatus TStdSampler::Sample(TRng &rng, void *ptr, size_t size) noexcept {
    if (auto status = Validate(ptr, size); status) {
        return status;
    }

    uint8_t *begin = static_cast<uint8_t *>(ptr);
    uint8_t *end = begin + size;

    for (auto it = begin; it != end; ++it) {
        for (size_t jt = 0; jt != 8; ++jt) {
            *it |= Dist_(rng) << jt;
        }
    }

    return EOk;
}

EStatus TStdSampler::Sample(void *ptr, size_t size) noexcept {
    if (auto status = Validate(ptr, size); status) {
        return status;
    }

    return EOk;
}

class TBaseSampler : public ISampler {
public:
    TBaseSampler(TExecutorPtr &&executor) noexcept
        : Executor_{std::move(executor)}
    {}

    virtual size_t GetBufferSize(size_t nobits) const noexcept override;
    size_t GetBlockSize(void) const noexcept;

    virtual EStatus Sample(TRng &rng, void *ptr, size_t size) noexcept override;
    virtual EStatus Sample(void *ptr, size_t size) noexcept override;

private:
    TExecutorPtr Executor_;
};

size_t TBaseSampler::GetBlockSize(void) const noexcept {
    return 32;
}

size_t TBaseSampler::GetBufferSize(size_t nobits) const noexcept {
    size_t blockSize = ISampler::GetBufferSize(nobits);
    size_t noblocks = Executor_->Plan().NoSrcBlocks_;
    return blockSize * noblocks;
}

EStatus TBaseSampler::Sample(TRng &rng, void *ptr, size_t size) noexcept {
    if (auto status = Validate(ptr, size); status) {
        return status;
    }

    size_t noblocks = size / Executor_->Plan().NoSrcBlocks_ / GetBlockSize();
    uint64_t *begin = static_cast<uint64_t *>(ptr);
    uint64_t *end = begin + size / sizeof(uint64_t);

    for (auto it = begin; it != end; ++it) {
        *it = rng();
    }

    Executor_->Execute(ptr, ptr, noblocks);
    return EOk;
}

EStatus TBaseSampler::Sample(void *ptr, size_t size) noexcept {
    return ENotImplemented;
}

TSamplerPtr CreateSampler(const TSamplerOpts &opts) {
    if (opts.UseStdSampler_) {
        std::unique_ptr<ISampler> ptr;
        ptr.reset(new TStdSampler(opts.Probability_));
        return ptr;
    }

    TExecutorOpts exeOpts{
        .Probability_ = opts.Probability_,
        .Tolerance_   = opts.Tolerance_,
        .Isa_         = opts.Ise_,
        .UseJit_      = opts.UseJit_,
    };

    if (auto executor = CreateExecutor(exeOpts); !executor) {
        return {};
    } else {
        std::unique_ptr<ISampler> ptr;
        ptr.reset(new TBaseSampler(std::move(executor)));
        return std::move(ptr);
    }
}

TSamplerPtr CreateSampler(double prob, double tol) {
    auto sampler = CreateSampler({.Probability_ = prob, .Tolerance_ = tol});
    return std::move(sampler);
}

} // namespace NFastBernoulli

