#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fix.h"

static u32 LENGTH=0, POINT=0;
static u32 *PAD = NULL, *ONE = NULL, *TWO = NULL;

u32* fix_new(){
    return calloc(sizeof(u32),LENGTH);
}

void fix_seti(u32* a, u64 b){
    a[LENGTH-1-POINT]=b;
    if(POINT>0){
        a[LENGTH-POINT]=b>>32;
    }
}

void arr_print(u32* arr, u32 len){
    int i;
    for(i=0; i<len; i++){
        printf("%08x",arr[i]);
        if(i<len-1)printf(",");
    }puts("");
}

void init_fix(u32 l, u32 p){
    if(l!=LENGTH){
        tear_fix();
        LENGTH = l;
        PAD = malloc(sizeof(u32)*10*LENGTH);
        ONE = calloc(sizeof(u32),LENGTH);
        TWO = calloc(sizeof(u32),LENGTH);
        ONE[LENGTH-1-POINT]=1;
        TWO[LENGTH-1-POINT]=2;
    }
    POINT = p;
}

u64 get_fix(){
    return (((u64)POINT)<<32)|LENGTH;
}

void tear_fix(){
    if(PAD != NULL)
        free(PAD);
}

void arr_neg(u32* dst, u32* src, u32 len, u32 dlen){
    u32 carry = 1;
    int i;
    for(i=0; i<len; i++){
        u32 tmp = carry+~src[i];
        if(tmp<~src[i])
            carry = 1;
        else
            carry = 0;
        dst[i] = tmp;
    }
    for(; i<dlen; i++){
        u32 tmp = carry - 1;
        if(tmp<carry)
            carry = 1;
        else
            carry = 0;
        dst[i] = tmp;
    }
}

void fix_neg(u32* dst, u32* src){
    arr_neg(dst,src,LENGTH,LENGTH);
}

u32 arr_add(u32* dst, u32* a, u32* b, u32 len, u32 dlen){
    u32 carry = 0;
    int i;
    for(i=0; i<len; i++){
        u32 tmp = a[i]+b[i];
        u32 res = tmp+carry;
        if(tmp<a[i] || res<tmp)
            carry = 1;
        else
            carry = 0;
        dst[i] = res;
    }
    for(; i<dlen; i++){
        u32 tmp = a[i] + carry;
        if(tmp<a[i])
            carry = 1;
        else
            carry = 0;
        dst[i] = tmp;
    }
    return carry;
}

u32 fix_add(u32* dst, u32* a, u32* b){
    return arr_add(dst,a,b,LENGTH,LENGTH);
}

u32 big_inc(u32* dst, u32* src){
    u32 one = 1;
    return arr_add(dst,dst,&one,1,LENGTH);
}

u32 arr_sub(u32* dst, u32* a, u32* b, u32 len, u32 dlen){
    u32 carry = 1;
    int i;
    for(i=0; i<len; i++){
        u32 bt = ~b[i];
        u32 tmp = a[i]+bt;
        u32 res = tmp+carry;
        if(tmp<a[i] || res<tmp)
            carry = 1;
        else carry = 0;
        dst[i] = res;
    }
    for(; i<dlen; i++){
        u32 tmp = a[i] - 1;
        u32 res = tmp + carry;
        if(tmp<a[i] || res < tmp)
            carry = 1;
        else
            carry = 0;
        dst[i] = res;
    }
    return carry;
}

u32 fix_sub(u32* dst, u32* a, u32* b){
    return arr_sub(dst,a,b,LENGTH,LENGTH);
}

void arr_mulkt(u32* dst, u32* a, u32* b, u32* pad, u32 len){
    if(len==1){
        u64 p = (u64)*a * (u64)*b;
        dst[0] = p;
        dst[1] = p>>32;
        return;
    }
    u32 *a0 = a, *a1 = a+len/2, *b0 = b, *b1 = b+len/2, *a2 = pad+len*2, *b2 = pad+len*2+len/2,
        *ab0 = dst, *ab1 = dst+len, *ab2 = pad+len*3, *c = dst+len/2;
    arr_mulkt(ab0,a0,b0,pad,len/2);
    arr_mulkt(ab1,a1,b1,pad,len/2);
    u32 acar = arr_add(a2,a0,a1,len/2,len/2), bcar = arr_add(b2,b0,b1,len/2,len/2);
    arr_mulkt(ab2,a2,b2,pad,len/2);
    memcpy(pad,ab0,sizeof(u32)*len);
    memcpy(pad+len,ab1,sizeof(u32)*len);
    arr_add(c,c,ab2,len,len+len/2);
    arr_sub(c,c,pad,len,len+len/2);
    arr_sub(c,c,pad+len,len,len+len/2);
    if(acar){
        arr_add(ab1,ab1,b2,len/2,len);
        if(bcar){
            u32 one = 1;
            arr_add(dst+len+len/2,dst+len+len/2,&one,1,len/2);
        }
    }
    if(bcar){
        arr_add(ab1,ab1,a2,len/2,len);
    }
}

void big_mul(u32* dst, u32* a, u32* b){
    int aneg = ((i32)a[LENGTH-1])<0,
        bneg = ((i32)b[LENGTH-1])<0;
    if(aneg)
        fix_neg(a,a);
    if(bneg)
        fix_neg(b,b);
    arr_mulkt(PAD,a,b,PAD+LENGTH*2,LENGTH);
    memcpy(dst,PAD,LENGTH*sizeof(u32));
    if(aneg)
        fix_neg(a,a);
    if(bneg)
        fix_neg(b,b);
    if((!aneg)!=(!bneg))
        fix_neg(dst,dst);
}

void fix_mul(u32* dst, u32* a, u32* b){
    int aneg = ((i32)a[LENGTH-1])<0,
        bneg = ((i32)b[LENGTH-1])<0;
    if(aneg)
        arr_neg(a,a,LENGTH,LENGTH);
    if(bneg)
        arr_neg(b,b,LENGTH,LENGTH);
    arr_mulkt(PAD,a,b,PAD+LENGTH*2,LENGTH);
    memset(dst,0,sizeof(u32)*LENGTH);
    i32 lo = (POINT-LENGTH+1)*2;
    int i;
    for(i=0; i<LENGTH*2; i++){
        if(lo+i<(int)POINT-(int)LENGTH+1)continue;
        if(lo+i>(int)POINT)continue;
        dst[lo+i+LENGTH-POINT-1]=PAD[i];
    }
    if(aneg)
        arr_neg(a,a,LENGTH,LENGTH);
    if(bneg)
        arr_neg(b,b,LENGTH,LENGTH);
    if((!aneg)!=(!bneg))
        arr_neg(dst,dst,LENGTH,LENGTH);
}

i32 arr_cmp(u32* a, u32* b, u32 len){
    int i;
    fix_sub(PAD,a,b);
    if((i32)PAD[LENGTH-1]<0)return -1;
    if(arr_iszero(PAD,LENGTH))return 0;
    return 1;
}

i32 fix_cmp(u32* a, u32* b){
    return arr_cmp(a,b,LENGTH);
}

i32 fix_cmpu(u32* a, u32 b){
    int neg = ((i32)a[LENGTH-1])<0;
    if(neg)
        fix_neg(a,a);
    int ret = -1;
    int i;
    for(i=LENGTH-1; i>=0; i--){
        int pow = POINT+1+i-LENGTH;
        if(pow>0&&a[i]){
            ret = 1;
            break;
        }
        if(ret==0&&pow<0&&a[i]){
            ret = 1;
            break;
        }
        if(pow==0){
            if(ret==-1&&a[i]==b){
                ret = 0;
            }
            if(ret!=0)
                break;
        }
    }
    if(neg){
        fix_neg(a,a);
        ret=-ret;
    }
    return ret;
}

void arr_shift(u32* dst, u32* src, i32 op, u32 len){
    int i;
    memcpy(dst,src,sizeof(u32)*len);
    if(op>=0){ //left shift
        while(op>=32){
            op-=32;
            for(i=len-1; i>0; i--)
                dst[i]=dst[i-1];
            dst[0]=0;
        }
        u32 carry = 0;
        for(i=0; i<len; i++){
            u32 sh = carry|(dst[i]<<op);
            carry = dst[i]>>(32-op);
            dst[i] = sh;
        }
    }else{ //right shift
        op=-op;
        while(op>=32){
            op-=32;
            for(i=0; i<len-1; i++){
                dst[i]=dst[i+1];
            }
            dst[len-1]=0;
        }
        u32 carry = 0;
        for(i=len-1; i>=0; i--){
            u32 sh = carry|(dst[i]>>op);
            carry = dst[i]<<(32-op);
            dst[i]=sh;
        }
    }
}

void fix_shift(u32* dst, u32* src, i32 op){
    arr_shift(dst,src,op,LENGTH);
}

u32 arr_iszero(u32* a, u32 len){
    int i;
    for(i=0; i<len; i++){
        if(a[i])return 0;
    }
    return 1;
}

u32 fix_iszero(u32* a){
    return arr_iszero(a,LENGTH);
}

void arr_divmod(u32* div, u32* mod, u32* a, u32* b, u32* pad, u32 len){
    if(arr_iszero(b,len))return;
    u32 *top = mod, *bot = pad, *add = pad+len;
    memcpy(top,a,sizeof(u32)*len);
    memcpy(bot,b,sizeof(u32)*len);
    memset(add,0,sizeof(u32)*len);
    memset(div,0,sizeof(u32)*len);
    *add = 1;
    while(arr_cmp(top,bot,len)>=0){
        arr_shift(add,add,1,len);
        arr_shift(bot,bot,1,len);
    }
    while(!arr_iszero(add,len)){
        if(arr_cmp(top,bot,len)>=0){
            arr_sub(top,top,bot,len,len);
            arr_add(div,div,add,len,len);
        }
        arr_shift(add,add,-1,len);
        arr_shift(bot,bot,-1,len);
    }
}

void big_divmod(u32* div, u32* mod, u32* a, u32* b){
    int aneg = ((i32)a[LENGTH-1])<0,
        bneg = ((i32)b[LENGTH-1])<0;
    if(aneg)
        arr_neg(a,a,LENGTH,LENGTH);
    if(bneg)
        arr_neg(b,b,LENGTH,LENGTH);
    arr_divmod(div,mod,a,b,PAD,LENGTH);
    i32 shift = 32*(LENGTH+POINT-1);
    arr_shift(div,div,shift,LENGTH);
    if(aneg)
        arr_neg(a,a,LENGTH,LENGTH);
    if(bneg)
        arr_neg(b,b,LENGTH,LENGTH);
    if((!aneg)!=(!bneg)){
        arr_neg(div,div,LENGTH,LENGTH);
    }
}

void fix_recip(u32* dst, u32* src){
    if(arr_iszero(src,LENGTH))return;
    u32 *target = PAD+LENGTH*6, *guess = PAD+LENGTH*7;
    memcpy(target,src,sizeof(u32)*LENGTH);
    int neg = 0;
    if((i32)target[LENGTH-1]<0){
        neg=1;
        arr_neg(target,target,LENGTH,LENGTH);
    }
    memset(dst,0,sizeof(u32)*LENGTH);
    dst[LENGTH-POINT-2] = (((u32)-1)>>1)+1;
    int bits = 0;
    while(arr_cmp(target,dst,LENGTH)<0){
        bits++;
        arr_shift(target,target,1,LENGTH);
    }
    dst[LENGTH-POINT-2]=0;
    dst[LENGTH-POINT-1]=1;
    while(arr_cmp(target,dst,LENGTH)>0){
        bits--;
        arr_shift(target,target,-1,LENGTH);
    }
    dst[LENGTH-POINT-1]=0;
    dst[LENGTH-POINT-2]=(((u32)-1)>>1)+1;
    int count = LENGTH*4-1;
    int i;
    for(i=0; i<count; i++){
        fix_mul(guess,dst,target);
        arr_neg(guess,guess,LENGTH,LENGTH);
        arr_add(guess,guess,TWO,LENGTH,LENGTH);
        fix_mul(PAD+LENGTH*8,dst,guess);
        memcpy(dst,PAD+LENGTH*8,sizeof(u32)*LENGTH);
    }
    arr_shift(dst,dst,bits,LENGTH);
    if(neg){
        arr_neg(dst,dst,LENGTH,LENGTH);
    }
}

void fix_div(u32* dst, u32* a, u32* b){
    fix_recip(PAD+LENGTH*9,b);
    fix_mul(dst,a,PAD+LENGTH*9);
}
