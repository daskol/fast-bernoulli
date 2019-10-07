/**
 * \file cpuid.h
 */

#pragma once

namespace NFastBernoulli {

bool IsAVXSupported(void) noexcept;
bool IsMMXSupported(void) noexcept;
bool IsSSESupported(void) noexcept;

} // namespace NFastBernoulli
