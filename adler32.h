#ifndef _ALDER32_
#define _ALDER32_

#include <stdint.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#define ADLER32_MODULO (65521)

inline 
uint32_t
Adler32ComputeChecksum(
    const uint8_t* const   datastream,
    const size_t           size
) {
    uint64_t s1 = 1;
    uint64_t s2 = 0;
#if defined(_MSC_VER) && (defined(_WIN64) || (defined(WIN32) && defined(_M_IX86_FP) && (_M_IX86_FP >= 2)))
    size_t quotient = size >> 3;
    size_t remainder = size & 7;
    for (size_t i = 0; i < quotient; i++)
    {
        __m128i xmm0 = _mm_setzero_si128();
        __m128i xmm1 = _mm_loadu_si128((const __m128i*)datastream + i);
        __m128i xmm2 = _mm_add_epi16(_mm_unpacklo_epi8(xmm1, xmm0), _mm_unpackhi_epi8(xmm1, xmm0));
        __m128i xmm3 = _mm_add_epi32(_mm_unpacklo_epi16(xmm2, xmm0), _mm_unpackhi_epi16(xmm2,xmm0));
        __m128i xmm4 = _mm_add_epi64(_mm_unpacklo_epi32(xmm3, xmm0), _mm_unpackhi_epi32(xmm3, xmm0));
        uint64_t sum = _mm_cvtsi128_si64x(xmm4) + _mm_cvtsi128_si64x(_mm_shuffle_epi32(xmm4, _MM_SHUFFLE(1, 0, 3, 2)));
        s1 = (s1 + sum) % ADLER32_MODULO;
        s2 = (s2 + s1) % ADLER32_MODULO;
    }
    for (size_t i = quotient << 3; i < remainder << 3; i++)
    {
        s1 = (s1 + datastream[i]) % ADLER32_MODULO;
        s2 = (s2 + s1) % ADLER32_MODULO;
    }
#else
    for (size_t i = 0; i < size; i++)
    {
        s1 = (s1 + datastream[i]) % ADLER32_MODULO;
        s2 = (s2 + s1) % ADLER32_MODULO;
    }
#endif
    return (((s2 & 0xFFFF) << 16) | (s1 & 0xFFFF));
}


#endif