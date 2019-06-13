#ifndef _H_RHTYH
#define _H_RHYTH

#include <stdint.h>

#define RMP_INIT_INVALID 1
#define RMP_OP_INCOMP 2

typedef struct _rmp_t{
    uint32_t len;
    uint32_t pnt;
    uint32_t* bin;
} rmp_t;

rmp_t* rmp_t_new(uint32_t,uint32_t);
void rmp_free(rmp_t*);
void rmp_trim(rmp_t*);
void rmp_to(rmp_t*,double);
double rmp_from(rmp_t*);
void rmp_zero(rmp_t*);
void rmp_copy(rmp_t*,rmp_t*);
void rmp_match(rmp_t*, uint32_t);

void rmp_negate(rmp_t*,rmp_t*);
void rmp_add(rmp_t*,rmp_t*,rmp_t*);
void rmp_sub(rmp_t*,rmp_t*,rmp_t*);

int rmp_err();
void rmp_err_clear();

void rmp_muln(uint32_t*,uint32_t*,uint32_t*,uint32_t);
void rmp_mulk(uint32_t*,uint32_t*,uint32_t*,uint32_t);


#endif
