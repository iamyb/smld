#ifndef _SMD_ATOMIC_X86_64_H_
#define _SMD_ATOMIC_X86_64_H_
#ifdef __cplusplus
extern "C"{
#endif

#include<stdint.h>

typedef struct{
    volatile int64_t cnt;
}smld_atomic64_t;

static inline int 
smld_atomic64_cmpset(volatile uint64_t *dst, uint64_t exp, uint64_t src)
{
    uint8_t res;

    asm volatile(
            "lock;"
            "cmpxchgq %[src], %[dst];"
            "sete %[res];"
            :[res] "=a" (res),
            [dst] "=m" (*dst)
            :[src] "r" (src),
            "a" (exp),
            "m" (dst)
            :"memory");

    return res;
}

static inline void
smld_atomic64_init(smld_atomic64_t *v)
{
    v->cnt = 0;
}

static inline int64_t
smld_atomic64_read(smld_atomic64_t *v)
{
    return v->cnt;
}

static inline void
smld_atomic64_set(smld_atomic64_t*v, int64_t new_value)
{
    v->cnt = new_value;
}

static inline void
smld_atomic64_add(smld_atomic64_t *v, int64_t inc)
{
    asm volatile(
            "lock;"
            "addq %[inc], %[cnt]"
            :[cnt] "=m" (v->cnt) // output
            :[inc] "ir" (inc),   // input
            "m" (v->cnt)
            );
}

static inline void 
smld_atomic64_sub(smld_atomic64_t *v, int64_t dec)
{
    asm volatile(
            "lock;"
            "subq %[dec], %[cnt]"
            :[cnt] "=m" (v->cnt)
            :[dec] "ir" (dec),
            "m" (v->cnt)
            );
}

static inline void
smld_atomic64_inc(smld_atomic64_t *v)
{
    asm volatile(
            "lock;"
            "incq %[cnt]"
            :[cnt] "=m" (v->cnt)
            : "m" (v->cnt)
            );
}

static inline void
smld_atomic64_dec(smld_atomic64_t *v)
{
    asm volatile(
            "lock;"
            "decq %[cnt]"
            :[cnt] "=m" (v->cnt)
            : "m" (v->cnt)
            );
}

static inline int64_t
smld_atomic64_add_return(smld_atomic64_t *v, int64_t inc)
{
    int64_t prev = inc;

    asm volatile(
            "lock;"
            "xaddq %[prev],%[cnt]"
            :[prev] "+r" (prev),
            [cnt] "=m" (v->cnt)
            :"m"(v->cnt)
            );

    return prev+inc;
}

static inline int64_t 
smld_atomic64_sub_return(smld_atomic64_t *v, int64_t dec)
{
    return smld_atomic64_add_return(v, -dec);
}

static inline int 
smld_atomic64_test_and_set(smld_atomic64_t *v)
{
    return smld_atomic64_cmpset((volatile uint64_t*)&v->cnt,0,1);
}

static inline void
smld_atomic64_clear(smld_atomic64_t *v)
{
    v->cnt = 0;
}
#ifdef __cplusplus
}
#endif
#endif
