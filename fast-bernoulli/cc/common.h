/**
 * \file common.h
 */

#pragma once

namespace NFastBernoulli {

enum class EInstructionSet : uint8_t {
    Auto = 0,
    General,
    AVX,
    MMX,
    SSE,
};

enum EStatus : uint32_t {
    EOk = 0,
    ENotImplemented,
    EWrongPtrAlignment,
    EWrongSizeAlignment,
};

} // namespace NFastBernoulli
