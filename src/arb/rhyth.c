#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "rhyth.h"

static int errcode = 0;

rmp_t* rmp_new(uint32_t len, uint32_t pnt){
    if(pnt>len){
        errcode |= RMP_INIT_INVALID;
        return NULL;
    }
    rmp_t* ret = malloc(sizeof(rmp_t));
    ret->len=len;
    ret->pnt=pnt;
    ret->bin = calloc(len,sizeof(uint32_t));
    return ret;
}

void rmp_free(rmp_t* val){
    free(val->bin);
    free(val);
}

void rmp_trim(rmp_t* val){
    uint32_t i;
    for(i=0; i<val->len; i++){
        if(i>=val->len-val->pnt)break;
        if(val->bin[i])break;
    }
    if(i==0)return;
    uint32_t nlen = val->len-i;
    uint32_t* nmem = malloc(sizeof(uint32_t)*nlen);
    memcpy(nmem,val->bin+i,nlen*sizeof(uint32_t));
    free(val->bin);
    val->bin=nmem;
    val->len = nlen;
}

void rmp_trimp2(rmp_t* val){
    uint32_t i;
    for(i=0; i<val->len; i++){
        if(i>=val->len-val->pnt)break;
        if(val->bin[i])break;
    }
    uint32_t nlen = val->len-i;
    uint32_t alen = 1;
    while(alen<nlen){
        alen<<=1;
    }
    uint32_t* nmem = malloc(sizeof(uint32_t)*alen);
    memcpy(nmem,val->bin+val->len-alen,alen*sizeof(uint32_t));
    free(val->bin);
    val->bin=nmem;
    val->len = alen;
}

void rmp_to(rmp_t* val, double d){
    int neg=0;
    if(d<0){
        neg=1;
        d=-d;
    }
    int i,j;
    for(i=0; i<sizeof(uint32_t)*(val->pnt-1); i++)
        d /= 1L<<8;
    for(i=val->len-1; i>=0; i--){
        uint32_t ival = (uint32_t)d;
        val->bin[i]=ival;
        for(j=0;j<sizeof(uint32_t);j++)
            d *= 1L<<8;
    }
    if(neg){
        uint32_t carry = 1;
        for(i=0; i<val->len; i++){
            uint32_t inv = ~val->bin[i];
            val->bin[i] = inv+carry;
            if(val->bin[i]<inv)carry=1;
            else carry = 0;
        }
    }
}

double rmp_from(rmp_t* val){
    double ret = 0;
    int i,j;
    for(i=val->len-1; i>=0; i--){
        for(j=0;j<sizeof(uint32_t);j++)
            ret *= 1L<<8;
        ret += val->bin[i];
    }
    for(i=0; i<sizeof(uint32_t)*(val->len-val->pnt); i++){
        ret /= 1L<<8;
    }
    if((int)val->bin[val->len-1]<0){
        double base = 1;
        for(j=0;j<sizeof(uint32_t)*(val->pnt);j++)
            base *= 1L<<8;
        ret-=base;
    } 
    return ret;
}

void rmp_zero(rmp_t* val){
    memset(val->bin,0,val->len);
}

void rmp_match(rmp_t* dst, uint32_t l){
    if(dst->len==l)return;
    uint32_t* nmem = malloc(sizeof(uint32_t)*l);
    int i;
    for(i=0; i<l; i++){
        if(i<dst->len)nmem[l-1-i]=dst->bin[dst->len-1-i];
        else nmem[l-1-i]=0;
    }
    free(dst->bin);
    dst->bin=nmem;
    dst->len=l;
}

void rmp_copy(rmp_t* dst, rmp_t* src){
    if(dst->pnt!=src->pnt){
        errcode |= RMP_OP_INCOMP;
        return;
    }
    if(dst->len<src->len){
        dst->bin = realloc(dst->bin,src->len*sizeof(uint32_t));
        dst->len=src->len;
    }
    memcpy(dst->bin,src->bin,src->len);
    if(dst->len>src->len)
        memset(dst->bin+src->len,0,dst->len-src->len);
}

void rmp_negate(rmp_t* dst, rmp_t* src){
    if(dst->pnt!=src->pnt){
        errcode |= RMP_OP_INCOMP;
        return;
    }
    rmp_match(dst,src->len);
    int i;
    uint32_t carry = 1;
    for(i=0; i<dst->len; i++){
        uint32_t val = ~(src->bin[i]);
        dst->bin[i] = val+carry;
        if(dst->bin[i]<val)carry=1;
        else carry=0;
    }
}

void rmp_add(rmp_t* dst, rmp_t* a, rmp_t* b){
    if(a->pnt!=b->pnt||dst->pnt!=a->pnt){
        errcode |= RMP_OP_INCOMP;
        return;
    }
    uint32_t len = a->len;
    if(b->len>len)len=b->len;
    rmp_match(dst,len);
    uint32_t carry = 0;
    int i;
    for(i=0; i<len; i++){
        uint32_t ad = 0, bd=0;
        if(i<a->len)ad=a->bin[i];
        if(i<b->len)bd=b->bin[i];
        uint32_t tmp = ad+bd;
        dst->bin[i]=tmp+carry;
        if(dst->bin[i]<tmp||tmp<ad)carry=1;
        else carry=0;
    }
}

void rmp_sub(rmp_t* dst, rmp_t* a, rmp_t* b){
    if(a->pnt!=b->pnt||dst->pnt!=a->pnt){
        errcode |= RMP_OP_INCOMP;
        return;
    }
    uint32_t len = a->len;
    if(b->len>len)len=b->len;
    rmp_match(dst,len);
    uint32_t carry = 1;
    int i;
    for(i=0; i<len; i++){
        uint32_t ad = 0, bd=0;
        if(i<a->len)ad=a->bin[i];
        if(i<b->len)bd=b->bin[i];
        bd = ~bd;
        uint32_t tmp = bd+carry;
        dst->bin[i] = tmp+ad;
        if(dst->bin[i]<ad||tmp<bd)carry=1;
        else carry=0;
    }
}

int rmp_err(){
    return errcode;
}

void rmp_err_clear(){
    errcode=0;
}

void arrpt(uint32_t* arr, int n){
    printf("0x");
    int i;
    for(i=0;i<n;i++){
        printf("%08x",arr[i]);
        if(i<n-1)printf(",");
    }puts("");
}

void arrneg(uint32_t* arr, int n){
    uint32_t carry = 1;
    int i;
    for(i=0; i<n; i++){
       uint32_t tmp = ~arr[i];
       arr[i]=tmp+carry;
       if(arr[i]<tmp)carry=1;
       else carry=0;
    }
}

int arradd(uint32_t* dst, uint32_t* a, uint32_t* b, uint32_t n, uint32_t fn){
    uint32_t carry = 0;
    int i;
    for(i=0; i<fn; i++){
        uint32_t at=a[i],bt=0;
        if(i<n){
            bt=b[i];
        }
        uint32_t tmp = at+bt;
        dst[i] = tmp+carry;
        if(dst[i]<tmp||tmp<at)carry=1;
        else carry=0;
    }
    return carry;
}

int arrsub(uint32_t* dst, uint32_t* a, uint32_t* b, uint32_t n, uint32_t fn){
     uint32_t carry = 1;
    int i;
    for(i=0; i<fn; i++){
        uint32_t at=a[i], bt=0;
        if(i<n)
            bt=b[i];
        bt = ~bt;
        uint32_t tmp = bt+carry;
        dst[i] = tmp+at;
        if(dst[i]<at||tmp<bt)carry=1;
        else carry=0;
    }
    return carry;

}

void arrsignext(uint32_t* dst, int n){
    int i;
    for(i=n-1;i>=0;i--){
        if(dst[i])break;
    }
    if(i==0)return;
    for(i=n-1;i>=0;i--){
        if(dst[i]==0)dst[i]=-1;
        else{
            uint32_t op = dst[i];
            uint32_t mask = -1;
            while(op){
                op>>=1;
                mask<<=1;
            }
            dst[i]|=mask;
            break;
        }
    }
}

void arrmuln(uint32_t* dst, uint32_t* a, uint32_t* b, int n){
    int i,j,k;
    memset(dst,0,sizeof(uint32_t)*2*n);
    for(i=0;i<n;i++){
        uint32_t af = a[i];
        for(j=0;j<n;j++){
            uint32_t bf = b[j];
            uint64_t prod = (uint64_t)af*(uint64_t)bf;
            uint32_t carry = (uint32_t)prod;
            for(k=i+j;k<2*n;k++){
                uint32_t tmp = dst[k];
                dst[k] = tmp+carry;
                if(dst[k]<tmp)carry=1;
                else carry=0;
            }
            carry = prod>>32;
            for(k=i+j+1;k<2*n;k++){
                uint32_t tmp = dst[k];
                dst[k] = tmp+carry;
                if(dst[k]<tmp)carry=1;
                else carry=0;
            }
        }
    }
}

void arrmulk(uint32_t* dst, uint32_t* a, uint32_t* b, uint32_t* pad, uint32_t n){
    if(n<=4){
        arrmuln(dst,a,b,n);
        return;
    }
    //printf("A:");arrpt(a,n);
    //printf("B:");arrpt(b,n);
    memset(dst,0,n*2*sizeof(uint32_t));
    uint32_t *a0=a, *a1=a+n/2, *b0=b, *b1=b+n/2;
    int lcar = arradd(pad,a0,a1,n/2,n/2);
    int hcar = arradd(pad+n/2,b0,b1,n/2,n/2);
    //printf("A0+A1:");arrpt(pad,n/2);
    //printf("Carry:%d\n",lcar);
    //printf("B0+B1:");arrpt(pad+n/2,n/2);
    //printf("Carry:%d\n",hcar);
    uint32_t carry = lcar&hcar;
    arrmulk(pad+n,pad,pad+n/2,pad+2*n,n/2);
    arrmulk(dst,a0,b0,pad+2*n,n/2);
    arrmulk(dst+n,a1,b1,pad+2*n,n/2);
    //printf("A0*B0:");arrpt(dst,n);
    //printf("A1*B1:");arrpt(dst+n,n);
    //printf("A2*B2:");arrpt(pad+n,n);
    memset(pad+2*n,0,2*n*sizeof(uint32_t));
    if(lcar){
        arradd(pad+n+n/2,pad+n+n/2,pad+n/2,n/2,n+n/2);
    }
    if(hcar){
        arradd(pad+n+n/2,pad+n+n/2,pad,n/2,n+n/2);
    }
    if(lcar&hcar){
        uint32_t one = 1;
        arradd(pad+n+n,pad+n+n,&one,1,n);
    }
    //printf("C2:");arrpt(pad+n,2*n);
    arrsub(pad+n,pad+n,dst,n,2*n);
    arrsub(pad+n,pad+n,dst+n,n,2*n);
    //printf("A2*B2-C1-C2:");arrpt(pad+n,2*n);
    arradd(dst+n/2,dst+n/2,pad+n,n,n+n/2);
    //printf("A*B:");arrpt(dst,n*2);
}

void arrmulkh(uint32_t* dst, uint32_t* a, uint32_t* b, uint32_t* pad, int n){
    int aneg=0, bneg=0;
    if(a[n-1]&~(((uint32_t)-1)>>1)){
        aneg=1;
        arrneg(a,n);
    }
    if(b[n-1]&~(((uint32_t)-1)>>1)){
        bneg=1;
        arrneg(b,n);
    }
    arrmulk(dst,a,b,pad,n);
    if(aneg)
        arrneg(a,n);
    if(bneg)
        arrneg(b,n);
    if(aneg^bneg)
        arrneg(dst,2*n);
}

void rmp_mul(rmp_t* dst, rmp_t* a, rmp_t* b){
    static uint32_t pad[1024];
    if(a->pnt!=b->pnt||a->pnt!=dst->pnt){
        errcode |= RMP_OP_INCOMP;
        return;
    }
    rmp_trimp2(a);
    rmp_trimp2(b);
    int len = a->len;
    if(b->len>len)len=b->len;
    rmp_match(a,len);
    rmp_match(b,len);
    rmp_match(dst,len*2);
    arrmulkh(dst->bin,a->bin,b->bin,pad,len);
    dst->bin = realloc(dst->bin,sizeof(uint32_t)*len*2-dst->pnt);
    dst->len=len*2-dst->pnt;
    rmp_trim(dst);
}

int xmain(int argc, char** argv){
    double da = 5, db = 2.4;
    if(argc>2){
        da = atof(argv[1]);
        db = atof(argv[2]);
    }
    rmp_t* a = rmp_new(4,1), *b = rmp_new(4,1), *c = rmp_new(4,1);
    rmp_to(a,da);
    rmp_to(b,db);
    arrpt(a->bin,a->len);
    arrpt(b->bin,b->len);
    rmp_mul(c,a,b);
    arrpt(c->bin,c->len);
    printf("%d,%d->%lf\n",c->len,c->pnt,rmp_from(c));
    return 0;
}
