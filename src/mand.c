#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fix/fix.h"
#include "libpbm.h"

void mand(u32* dst, u32* x, u32* y, u32* max){
    u32 *re = fix_new(), *im=fix_new(), *rr=fix_new(), *ii=fix_new(), *ri=fix_new();
    u32 len = get_fix();
    memset(dst,0,len*sizeof(u32));
    while(fix_cmp(dst,max)<0){
        fix_mul(rr,re,re);
        fix_mul(ii,im,im);
        fix_mul(ri,re,im);
        fix_sub(re,rr,ii);
        fix_add(im,ri,ri);
        fix_add(re,re,x);
        fix_add(im,im,y);
        fix_add(ri,rr,ii);
        if(((i32)ri[len-1])>4){
            break;
        }
        big_inc(dst,dst);
    }
    free(re);
    free(im);
    free(rr);
    free(ii);
    free(ri);
}

int main(){
    init_fix(4,0);
    u32 max[] = {255,0,0,0};
    u32 dst[4];
    u32 x[4];
    u32 y[] = {0,0,0,1};
    fix_neg(y,y);
    u32 xinc[] = {0,0,0x10000000,0};
    u32 yinc[] = {0,0,0x10000000,0};
    int width = 8, height = 8;
    u32 res[256];
    int i,j;
    for(i=0; i<height; i++){
        x[0]=x[1]=x[2]=0;
        x[3] = 1;
        fix_neg(x,x);
        for(j=0; j<width; j++){
            mand(dst,x,y,max);
            arr_print(x,4);
            printf("+i");arr_print(y,4);
            printf("->");arr_print(dst,4);
            res[i*width+j]=dst[0]<<8;
            fix_add(x,x,xinc);
        }
        fix_add(y,y,yinc);
    }
    tear_fix();
    FILE* out = fopen("test.ppm","wb");
    writePBM(6,width,height,res,FMT_RGB32,out);
    return 0;
}
