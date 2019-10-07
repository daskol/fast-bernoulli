/**
 * \file executor.cc
 */

#include "executor.h"

#include <exception>
#include <x86intrin.h>

namespace NFastBernoulli {

std::optional<TExecutionPlan> Quantize(double prob, double tol) noexcept {
    return std::nullopt;
}

void TGeneralExecutor::Execute(const void *src, void *dst, size_t noblocks) {
    constexpr size_t BlockSize = IExecutor::BlockSize / sizeof(uint64_t);
    auto *begin = static_cast<const uint64_t *>(src);
    auto *end = begin + noblocks * Plan_.NoSrcBlocks_ * BlockSize;
    auto *res = static_cast<uint64_t *>(dst);
    for (auto it = begin; it != end; ++it, ++res) {
        uint64_t value = *it;
        for (size_t ip = 0; ip != Plan_.NoOps_; ++ip) {
            switch (Plan_.Ops_[Plan_.NoOps_ - ip - 1]) {
            case EOp::Not:
                value = ~value;
                break;
            case EOp::And:
                value &= *(++it);
                break;
            case EOp::Or:
                value |= *(++it);
                break;
            default:
                std::terminate();
            }
        }
        *res = value;
    }
}

void TAvxExecutor::Execute(const void *src, void *dst, size_t noblocks) {
    auto *begin = static_cast<const __m256i *>(src);
    auto *end = begin + noblocks * Plan_.NoSrcBlocks_;
    auto *res = static_cast<__m256i *>(dst);
    __m256i ymm00, ymm01;
    for (auto it = begin; it != end; ++it, ++res) {
        ymm00 = _mm256_load_si256(it);
        for (size_t ip = 0; ip != Plan_.NoOps_; ++ip) {
            switch (Plan_.Ops_[Plan_.NoOps_ - ip - 1]) {
            case EOp::Not:
                ymm01 = _mm256_set1_epi64x(-1l);
                ymm00 = _mm256_xor_si256(ymm00, ymm01);
                break;
            case EOp::And:
                ymm01 = _mm256_load_si256(++it);
                ymm00 = _mm256_and_si256(ymm00, ymm01);
                break;
            case EOp::Or:
                ymm01 = _mm256_load_si256(++it);
                ymm00 = _mm256_or_si256(ymm00, ymm01);
                break;
            default:
                std::terminate();
            }
        }
        _mm256_store_si256(res, ymm00);
    }
}

TExecutorPtr CreateExecutor(double prob, double tol) noexcept {
    return CreateExecutor({prob, tol, EInstructionSet::Auto});
}

TExecutorPtr CreateExecutor(const TExecutorOpts &opts) noexcept {
    if (auto plan = Quantize(opts.Probability_, opts.Tolerance_); !plan) {
        return nullptr;
    } else {
        std::unique_ptr<IExecutor> ptr;
        switch (opts.Isa_) {
        case EInstructionSet::Auto:
        case EInstructionSet::General:
            ptr.reset(new TGeneralExecutor(std::move(*plan)));
            break;
        case EInstructionSet::AVX:
            ptr.reset(new TAvxExecutor(std::move(*plan)));
            break;
        case EInstructionSet::MMX: // Not implemented.
        case EInstructionSet::SSE: // Not implemented.
        default:
            break;
        }
        return ptr;
    }
}

} // namespace NFastBernoulli
