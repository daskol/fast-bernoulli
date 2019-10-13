/**
 * \file executor_jit.h
 */

#pragma once

#include "executor.h"

#include <llvm/ExecutionEngine/Orc/LLJIT.h>

namespace NFastBernoulli {

class TJitExecutor : public IExecutor {
private:
    typedef void (*TJitFunction)(const void *, void *) noexcept;

    struct TJitState {
        std::unique_ptr<llvm::orc::LLJIT> LLJIT_;
    };

    using TJitStatePtr = std::unique_ptr<TJitState>;

public:
    TJitExecutor(const TExecutionPlan &plan,
                 TJitFunction func,
                 TJitStatePtr state) noexcept
        : IExecutor(plan)
        , JitFunction_{func}
        , JitState_{std::move(state)}
    {}

    virtual ~TJitExecutor(void) = default;
    virtual void Execute(const void *src, void *dst, size_t noblocks) override;

    static TExecutorPtr Create(const TExecutionPlan &plan,
                               EInstructionSet ise) noexcept;

private:
    TJitFunction JitFunction_ = nullptr;
    TJitStatePtr JitState_;
};

TExecutorPtr CreateJitExecutor(const TExecutionPlan &plan,
                               EInstructionSet ise) noexcept;

} // namespace NFastBernoulli
