/**
 * \file executor.h
 */

#pragma once

#include <array>
#include <memory>
#include <optional>

namespace NFastBernoulli {

constexpr size_t MaxPrecision = 64;

enum EOp : uint8_t {
    Not = 0,
    And,
    Or,
};

struct TExecutionPlan {
    std::array<EOp, MaxPrecision> Ops_;
    size_t NoOps_;
    size_t NoSrcBlocks_;
};

std::optional<TExecutionPlan> Quantize(double prob, double tol) noexcept;

class IExecutor {
public:
    static constexpr size_t BlockSize = 32; // bytes

public:
    constexpr IExecutor(const TExecutionPlan &plan) noexcept
        : Plan_{plan}
    {}

    virtual ~IExecutor(void) = default;
    virtual void Execute(const void *src, void *dst, size_t noblocks) = 0;

protected:
    TExecutionPlan Plan_;
};

class TGeneralExecutor : public IExecutor {
public:
    constexpr TGeneralExecutor(const TExecutionPlan &plan) noexcept
        : IExecutor(plan)
    {}

    virtual ~TGeneralExecutor(void) = default;
    virtual void Execute(const void *src, void *dst, size_t noblocks) override;
};

class TAvxExecutor : public IExecutor {
public:
    constexpr TAvxExecutor(const TExecutionPlan &plan) noexcept
        : IExecutor(plan)
    {}

    virtual ~TAvxExecutor(void) = default;
    virtual void Execute(const void *src, void *dst, size_t noblocks) override;
};

enum EInstructionSet : uint8_t {
    Auto = 0,
    General,
    AVX,
    MMX,
    SSE,
};

struct TExecutorOpts {
    double Probability_;
    double Tolerance_;
    EInstructionSet Isa_ = Auto;
};

using TExecutorPtr = std::unique_ptr<IExecutor>;

TExecutorPtr CreateExecutor(double prob, double tol) noexcept;
TExecutorPtr CreateExecutor(const TExecutorOpts &opts) noexcept;

} // namespace NFastBernoulli
