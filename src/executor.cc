/**
 * \file executor.cc
 */

#include "executor.h"

#include <algorithm>
#include <exception>
#include <x86intrin.h>

#ifdef USE_JIT_EXECUTOR
#include "cpuid.h"
#include "executor_jit.h"
#else
#include "cpuid.h"
#endif

namespace NFastBernoulli {

std::optional<TExecutionPlan> Quantize(double prob, double tol) noexcept {
    TExecutionPlan plan{.NoSrcBlocks_ = 1};
    double lhs = 0.0, rhs = 1.0;
    for (auto it = plan.Ops_.begin(); it + 1 < plan.Ops_.end(); ++it) {
        if (prob - lhs <= tol && rhs - prob <= tol) {
            std::reverse(plan.Ops_.begin(), plan.Ops_.begin() + plan.NoOps_);
            return plan;
        }

        double mid = lhs + 0.5 * (rhs - lhs);

        if (mid == prob) {
            std::reverse(plan.Ops_.begin(), plan.Ops_.begin() + plan.NoOps_);
            return plan;
        } else if (prob > mid) {
            *(it++) = EOp::Not;
            ++plan.NoOps_;
            prob = 1 - prob;
            lhs = 1 - rhs;
            rhs = 1 - mid;
        } else if (prob < mid) {
            rhs = mid;
        }

        // In any case append AND operation.
        *it = EOp::And;
        ++plan.NoOps_;
        ++plan.NoSrcBlocks_;
    }
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
            switch (Plan_.Ops_[ip]) {
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
        EInstructionSet ise = opts.Isa_;

        // Select optimal executor with respect to instruction excentions.
        if (ise == EInstructionSet::Auto) {
            if (IsAVXSupported()) {
                ise = EInstructionSet::AVX;
            } else if (IsSSESupported()) {
                ise = EInstructionSet::SSE;
            } else if (IsMMXSupported()) {
                ise = EInstructionSet::MMX;
            }
        }

        if (opts.UseJit_) {
#ifdef USE_JIT_EXECUTOR
            return std::move(CreateJitExecutor(std::move(*plan), ise));
#else
            return nullptr;
#endif
        }

        std::unique_ptr<IExecutor> ptr;
        switch (ise) {
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
