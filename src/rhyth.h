#ifndef _H_RHTYH
#define _H_RHYTH

typedef struct _rmp_t{
    unsigned int len;
    unsigned int pnt;
    unsigned int* bin;
} rmp_t;

rmp_t* rmp_t_new(unsigned int,unsigned int);
void rmp_t_free(rmp_t*);
void rmp_t_trim(rmp_t*);
void rmp_t_to(rmp_t*,double);
double rmp_t_from(rmp_t*);
void rmp_t_zero(rmp_t*);
void rmp_t_copy(rmp_t*,rmp_t*);

void rmp_t_negate(rmp_t*,rmp_t*);
void rmp_t_add(rmp_t*,rmp_t*,rmp_t*);
void rmp_t_sub(rmp_t*,rmp_t*,rmp_t*);

#endif
