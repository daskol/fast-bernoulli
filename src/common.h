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

} // namespace NFastBernoulli
