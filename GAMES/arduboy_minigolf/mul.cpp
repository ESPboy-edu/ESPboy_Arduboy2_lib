#include "game.hpp"

int16_t mul_f8_s16(int16_t a, uint8_t b)
{
#if USE_AVR_INLINE_ASM
    /*
               A1 A0
                  B0
               =====
               A0*B0
            A1*B0
         ===========
            R1 R0
    */
    int16_t r;
    asm volatile(
        "mulsu %B[a], %[b]       \n\t"
        "movw  %A[r], r0         \n\t" // r = A1*B0
        "mul   %A[a], %[b]       \n\t"
        "add   %A[r], r1         \n\t" // R0 += hi(A0*B0)
        "clr   r1                \n\t"
        "adc   %B[r], r1         \n\t" // R1 += C
        : [r] "=&r" (r)
        : [a] "a"   (a),
          [b] "a"   (b)
        :
    );
    return r;
#else
    return int16_t(u24(s24(a) * b) >> 8);
#endif
}

int16_t mul_f8_s16(int16_t a, uint16_t b)
{
#if USE_AVR_INLINE_ASM
    /*
               A1 A0
               B1 B0
               =====
               A0*B0
            A1*B0
            A0*B1
         A1*B1
         ===========
            R1 R0
    */
    int16_t r;
    asm volatile(
        "mulsu %B[a], %A[b]      \n\t"
        "movw  %A[r], r0         \n\t" // r = A1*B0
        "mul   %B[b], %A[a]      \n\t"
        "add   %A[r], r0         \n\t"
        "adc   %B[r], r1         \n\t" // r += A0*B1
        "mul   %B[a], %B[b]      \n\t"
        "add   %B[r], r0         \n\t" // R1 += lo(A1*B1)
        "mul   %A[a], %A[b]      \n\t"
        "add   %A[r], r1         \n\t" // R0 += hi(A0*B0)
        "clr   r1                \n\t"
        "adc   %B[r], r1         \n\t" // R1 += C
        : [r] "=&r" (r)
        : [a] "a"   (a),
          [b] "a"   (b)
        :
    );
    return r;
#else
    return int16_t(u24(s24(a) * b) >> 8);
#endif
}

int16_t mul_f8_s16(int16_t a, int16_t b)
{
#if USE_AVR_INLINE_ASM
    /*
               A1 A0
               B1 B0
               =====
               A0*B0
            A1*B0
            A0*B1
         A1*B1
         ===========
            R1 R0
    */
    int16_t r;
    asm volatile(
        "mulsu %B[a], %A[b]      \n\t"
        "movw  %A[r], r0         \n\t" // r = A1*B0
        "mulsu %B[b], %A[a]      \n\t"
        "add   %A[r], r0         \n\t"
        "adc   %B[r], r1         \n\t" // r += A0*B1
        "mul   %B[a], %B[b]      \n\t"
        "add   %B[r], r0         \n\t" // R1 += lo(A1*B1)
        "mul   %A[a], %A[b]      \n\t"
        "add   %A[r], r1         \n\t" // R0 += hi(A0*B0)
        "clr   r1                \n\t"
        "adc   %B[r], r1         \n\t" // R1 += C
        : [r] "=&r" (r)
        : [a] "a"   (a),
          [b] "a"   (b)
        :
    );
    return r;
#else
    return int16_t(u24(s24(a) * b) >> 8);
#endif
}

int16_t mul_f16_s16(int16_t a, int16_t b)
{
    return int16_t(uint32_t(int32_t(a) * b) >> 16);
}

uint16_t mul_f8_u16(uint16_t a, uint8_t b)
{
#if USE_AVR_INLINE_ASM
    /*
               A1 A0
                  B0
            ========
               A0*B0
            A1*B0
         ===========
            R1 R0
    */
    uint16_t r;
    asm volatile(
        "mul  %B[a], %A[b]      \n\t"
        "movw %A[r], r0         \n\t" // r = A1*B0
        "mul  %A[a], %A[b]      \n\t"
        "add  %A[r], r1         \n\t" // R0 += hi(A0*B0)
        "clr  r1                \n\t"
        "adc  %B[r], r1         \n\t" // R1 += C
        : [r] "=&r" (r)
        : [a] "r"   (a),
          [b] "r"   (b)
        :
    );
    return r;
#else
    return uint16_t((u24(a) * b) >> 8);
#endif
}

uint16_t mul_f8_u16(uint16_t a, uint16_t b)
{
#if USE_AVR_INLINE_ASM
    /*
               A1 A0
               B1 B0
            ========
               A0*B0
            A1*B0
            A0*B1
         A1*B1
         ===========
            R1 R0
    */
    uint16_t r;
    asm volatile(
        "mul  %B[a], %A[b]      \n\t"
        "movw %A[r], r0         \n\t" // r = A1*B0
        "mul  %A[a], %B[b]      \n\t"
        "add  %A[r], r0         \n\t"
        "adc  %B[r], r1         \n\t" // r += A0*B1
        "mul  %B[a], %B[b]      \n\t"
        "add  %B[r], r0         \n\t" // R1 += lo(A1*B1)
        "mul  %A[a], %A[b]      \n\t"
        "add  %A[r], r1         \n\t" // R0 += hi(A0*B0)
        "clr  r1                \n\t"
        "adc  %B[r], r1         \n\t" // R1 += C
        : [r] "=&r" (r)
        : [a] "r"   (a),
          [b] "r"   (b)
        :
    );
    return r;
#else
    return uint16_t((u24(a) * b) >> 8);
#endif
}

int16_t mul_f7_s16(int16_t a, int8_t b)
{
#if USE_AVR_INLINE_ASM
    /*
               A1 A0
                  B0
               =====
               A0*B0
            A1*B0
         ===========
            R1 R0
    */
    int16_t r;
    asm volatile(
        "fmuls  %B[a], %[b]       \n\t"
        "movw   %A[r], r0         \n\t" // r = (A1*B0) << 1
#if AVOID_FMULSU
        "mulsu  %[b] , %A[a]      \n\t"
        "add    r0   , r0         \n\t"
        "adc    r1   , r1         \n\t"
#else
        "fmulsu %[b] , %A[a]      \n\t"
#endif
        "clr    r0                \n\t"
        "sbc    %B[r], r0         \n\t" // R1 -= bit15(A0*B0)
        "add    %A[r], r1         \n\t" // R0 += hi((A0*B0) << 1)
        "clr    r1                \n\t"
        "adc    %B[r], r1         \n\t" // R1 += C
        : [r] "=&r" (r)
        : [a] "a"   (a),
          [b] "a"   (b)
        :
    );
    return r;
#else
    return int16_t(u24(s24(a) * b) >> 7);
#endif
}

int16_t mul_f15_s16(int16_t a, int16_t  b)
{
#if USE_AVR_INLINE_ASM
    /*
               A1 A0
               B1 B0
            ========
               A0*B0
            A1*B0
            A0*B1
         A1*B1
         ===========
         R1 R0
    */
    int16_t r;
    int8_t t;
    int8_t z = 0;
    asm volatile(
        "fmuls  %B[a], %B[b]      \n\t"
        "movw   %A[r], r0         \n\t"
        "fmul   %A[a], %A[b]      \n\t"
        "adc    %A[r], %[z]       \n\t"
        "mov    %[t] , r1         \n\t"
#if AVOID_FMULSU
        "mulsu  %B[a], %A[b]      \n\t"
        "add    r0   , r0         \n\t"
        "adc    r1   , r1         \n\t"
#else
        "fmulsu %B[a], %A[b]      \n\t"
#endif
        "sbc    %B[r], %[z]       \n\t"
        "add    %[t] , r0         \n\t"
        "adc    %A[r], r1         \n\t"
        "adc    %B[r], %[z]       \n\t"
#if AVOID_FMULSU
        "mulsu  %B[b], %A[a]      \n\t"
        "add    r0   , r0         \n\t"
        "adc    r1   , r1         \n\t"
#else
        "fmulsu %B[b], %A[a]      \n\t"
#endif
        "sbc    %B[r], %[z]       \n\t"
        "add    %[t] , r0         \n\t"
        "adc    %A[r], r1         \n\t"
        "adc    %B[r], %[z]       \n\t"
        "clr    r1                \n\t"
        : [r] "=&r" (r),
          [t] "=&r" (t)
        : [a] "a"   (a),
          [b] "a"   (b),
          [z] "r"   (z)
        :
    );
    return r;
#else
    return int16_t(uint32_t(int32_t(a) * b) >> 15);
#endif
}
