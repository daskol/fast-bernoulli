/**
 * \file executor_jit.cc
 */

#include "executor_jit.h"

#include <x86intrin.h>

#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>

namespace NFastBernoulli {

bool GenerateCode(llvm::LLVMContext &context, llvm::Module &module,
                  const TExecutionPlan &plan) {
    llvm::IRBuilder<> builder(context);
    llvm::Type *int64 = llvm::Type::getInt64Ty(context);
    llvm::Type *ymm = llvm::VectorType::get(int64, 4);
    llvm::Type *ymmPtr = llvm::PointerType::get(ymm, 0);
    llvm::Type *retVoid = llvm::Type::getVoidTy(context);

    auto *funcType = llvm::FunctionType::get(retVoid, {ymmPtr, ymmPtr}, false);
    auto *func = llvm::Function::Create(funcType,
                                        llvm::Function::ExternalLinkage,
                                        "sample", module);

    auto attrAlignment = llvm::Attribute::AttrKind::Alignment;
    auto attr = llvm::Attribute::get(context, attrAlignment, 32);
    std::vector<llvm::Value *> args;

    for (auto &arg : func->args()) {
        args.push_back(&arg);
        arg.addAttr(attr);
        if (args.size() == 1) {
            arg.addAttr(llvm::Attribute::AttrKind::ReadOnly);
        }
    }

    llvm::APInt incr = llvm::APInt(64, 32);
    auto *ptrdiff = llvm::Constant::getIntegerValue(int64, incr);
    auto *basicBlock = llvm::BasicBlock::Create(context, "entry", func);

    builder.SetInsertPoint(basicBlock);

    auto *ptr = args[0];
    llvm::Value *acc = builder.CreateAlignedLoad(ptr, 32);

    for (size_t ip = 0; ip != plan.NoOps_; ++ip) {
        switch (plan.Ops_[ip]) {
        case EOp::Not: {
            acc = builder.CreateNot(acc);
            } break;
        case EOp::And: {
            // Advance pointer to source blocks.
            auto *ptrval = builder.CreatePtrToInt(ptr, int64);
            auto *ptrinc = builder.CreateAdd(ptrval, ptrdiff);
            ptr = builder.CreateIntToPtr(ptrinc, ymmPtr);
            // Load block to register and apply binary operator.
            auto *val = builder.CreateAlignedLoad(ptr, 32);
            acc = builder.CreateAnd(acc, val);
            } break;
        case EOp::Or: {
            // Advance pointer to source blocks.
            auto *ptrval = builder.CreatePtrToInt(ptr, int64);
            auto *ptrinc = builder.CreateAdd(ptrval, ptrdiff);
            ptr = builder.CreateIntToPtr(ptrinc, ymmPtr);
            // Load block to register and apply binary operator.
            auto *val = builder.CreateAlignedLoad(ptr, 32);
            acc = builder.CreateAdd(acc, val);
            } break;
        default:
            return false;
        }
    }

    builder.CreateAlignedStore(acc, args[1], 32);
    builder.CreateRetVoid();

    llvm::outs() << module;
    llvm::verifyFunction(*func, &llvm::errs());
    return !llvm::verifyFunction(*func);
}

void TJitExecutor::Execute(const void *src, void *dst, size_t noblocks) {
    auto *begin = static_cast<const __m256i *>(src);
    auto *end = begin + noblocks * Plan_.NoSrcBlocks_;
    auto *res = static_cast<__m256i *>(dst);
    for (auto it = begin; it != end; it += Plan_.NoSrcBlocks_, ++res) {
        JitFunction_(it, res);
    }
}

TExecutorPtr TJitExecutor::Create(const TExecutionPlan &plan,
                                  EInstructionSet ise) noexcept {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    auto jtmb = llvm::orc::JITTargetMachineBuilder::detectHost();

    if (!jtmb) {
        llvm::errs() << "failed to detect host: " << jtmb.takeError() << '\n';
        return nullptr;
    }

    // Add specific CPU features if required.
    switch (ise) {
    case EInstructionSet::MMX:
        jtmb->getFeatures().AddFeature("mmx");
        break;
    case EInstructionSet::SSE:
        jtmb->getFeatures().AddFeature("sse2");
        break;
    case EInstructionSet::AVX:
        jtmb->getFeatures().AddFeature("avx2");
        break;
    default:
        break;
    }

    auto dl = jtmb->getDefaultDataLayoutForTarget();

    if (!dl) {
        llvm::errs() << dl.takeError() << '\n';
        return nullptr;
    }

    auto vm = llvm::orc::LLJIT::Create(*jtmb, *dl, 1);

    if (!vm) {
        llvm::errs() << "failed to create LLJIT: " << vm.takeError() << '\n';
        return nullptr;
    }

    auto context = std::make_unique<llvm::LLVMContext>();
    auto module = std::make_unique<llvm::Module>("unnamed", *context.get());

    if (!GenerateCode(*context.get(), *module.get(), plan)) {
        llvm::errs() << "failed to generate ir code\n";
        return nullptr;
    }

    auto err = (*vm)->addIRModule({std::move(module), std::move(context)});

    if (err) {
        llvm::errs() << err << '\n';
        return nullptr;
    }

    auto foo = (*vm)->lookup("sample");

    if (!foo) {
        llvm::errs() << foo.takeError() << "\n";
    }

    auto func = reinterpret_cast<TJitFunction>(foo->getAddress());
    auto state = std::make_unique<TJitState>();
    state->LLJIT_ = std::move(*vm);
    return std::make_unique<TJitExecutor>(plan, func, std::move(state));
}

TExecutorPtr CreateJitExecutor(const TExecutionPlan &plan,
                               EInstructionSet ise) noexcept {
    return std::move(TJitExecutor::Create(plan, ise));
}

} // namespace NFastBernoulli
