#ifndef _H_FIX
#define _H_FIX

#include <stdint.h>

typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

u32* fix_new();
void fix_setu(u32*,u64);

void init_fix(u32,u32);
u64 get_fix();
void tear_fix();

void arr_neg(u32*,u32*,u32,u32);
void fix_neg(u32*,u32*);

u32 arr_add(u32*,u32*,u32*,u32,u32);
u32 arr_sub(u32*,u32*,u32*,u32,u32);

u32 fix_add(u32*,u32*,u32*);
u32 big_inc(u32*,u32*);
u32 fix_sub(u32*,u32*,u32*);

void fix_mul(u32*,u32*,u32*);
void big_mul(u32*,u32*,u32*);
void arr_mulkt(u32*,u32*,u32*,u32*,u32);

i32 arr_cmp(u32*,u32*,u32);
i32 fix_cmp(u32*,u32*);
i32 fix_cmpu(u32*,u32);
u32 arr_iszero(u32*,u32);
u32 fix_iszero(u32*);
void arr_shift(u32*,u32*,i32,u32);
void fix_shift(u32*,u32*,i32);
void arr_divmod(u32*,u32*,u32*,u32*,u32*,u32);
void big_divmod(u32*,u32*,u32*,u32*);

void fix_recip(u32*,u32*);
void fix_div(u32*,u32*,u32*);

#endif
