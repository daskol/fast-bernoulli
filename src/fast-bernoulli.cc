/**
 * \file fast-bernoulli.cc
 */

#include "fast-bernoulli.h"

#include <cpuid.h>
#include <x86intrin.h>

namespace NFastBernoulli {

bool IsAVXSupported(void) {
    uint32_t eax, ebx, ecx, edx;
    __get_cpuid(1, &eax, &ebx, &ecx, &edx);
    return static_cast<bool>(ecx & bit_AVX);
}

bool IsMMXSupported(void) {
    uint32_t eax, ebx, ecx, edx;
    __get_cpuid(1, &eax, &ebx, &ecx, &edx);
    return static_cast<bool>(edx & bit_MMX);
}

bool IsSSESupported(void) {
    uint32_t eax, ebx, ecx, edx;
    __get_cpuid(1, &eax, &ebx, &ecx, &edx);
    return static_cast<bool>(edx & bit_SSE);
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

EStatus TStdBernoulli::Generate(TRng &rng, void *ptr, size_t size) noexcept {
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

EStatus TStdBernoulli::Generate(void *ptr, size_t size) noexcept {
    if (auto status = Validate(ptr, size); status) {
        return status;
    }

    return EOk;
}

inline void GenerateAVX(void *ptr, size_t size) noexcept {
    __m256i ymm00 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x00);
    __m256i ymm01 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x01);
    __m256i ymm02 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x02);
    __m256i ymm03 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x03);
    __m256i ymm04 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x04);
    __m256i ymm05 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x05);
    __m256i ymm06 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x06);
    __m256i ymm07 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x07);
    __m256i ymm08 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x08);
    __m256i ymm09 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x09);
    __m256i ymm10 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x0a);
    __m256i ymm11 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x0b);
    __m256i ymm12 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x0c);
    __m256i ymm13 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x0d);
    __m256i ymm14 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x0e);
    __m256i ymm15 = _mm256_load_si256(static_cast<__m256i *>(ptr) + 0x0f);

    ymm00 = _mm256_and_si256(ymm00, ymm01);
    ymm00 = _mm256_and_si256(ymm00, ymm02);
    ymm00 = _mm256_and_si256(ymm00, ymm03);
    ymm00 = _mm256_and_si256(ymm00, ymm04);
    ymm00 = _mm256_and_si256(ymm00, ymm05);
    ymm00 = _mm256_and_si256(ymm00, ymm06);
    ymm00 = _mm256_and_si256(ymm00, ymm07);
    ymm00 = _mm256_and_si256(ymm00, ymm08);
    ymm00 = _mm256_and_si256(ymm00, ymm09);
    ymm00 = _mm256_and_si256(ymm00, ymm10);
    ymm00 = _mm256_and_si256(ymm00, ymm11);
    ymm00 = _mm256_and_si256(ymm00, ymm12);
    ymm00 = _mm256_and_si256(ymm00, ymm13);
    ymm00 = _mm256_and_si256(ymm00, ymm14);
    ymm00 = _mm256_and_si256(ymm00, ymm15);

    _mm256_store_si256(static_cast<__m256i *>(ptr), ymm00);
}

EStatus TDummyBernoulli::Generate(TRng &rng, void *ptr, size_t size) noexcept {
    if (auto status = Validate(ptr, size); status) {
        return status;
    }

    uint64_t *begin = static_cast<uint64_t *>(ptr);
    uint64_t *end = begin + size / sizeof(uint64_t);

    for (auto it = begin; it != end; ++it) {
        *it = rng();
    }

    GenerateAVX(ptr, size);
    return EOk;
}

EStatus TDummyBernoulli::Generate(void *ptr, size_t size) noexcept {
    if (auto status = Validate(ptr, size); status) {
        return status;
    }

    GenerateAVX(ptr, size);
    return EOk;
}

TSamplerPtr CreateBernoulliGenerator(double proba) {
    std::unique_ptr<IBernoulli> ptr;
    ptr.reset(new TDummyBernoulli(proba));
    return std::move(ptr);
}

} // namespace NFastBernoulli
