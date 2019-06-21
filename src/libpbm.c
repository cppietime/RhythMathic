#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "libpbm.h"

int FMT_SIZES[7] = {1,8,8,8,16,24,32};

uint32_t pullPBMDataFrom(int type, int index, void* data, int format){
    int offset = index*FMT_SIZES[format]/8;
    uint32_t rgb;
    switch(format){
        case FMT_BWBIN:
        {
            int mask = 8-index%8;
            uint8_t* bins = (uint8_t*)data;
            rgb = 1&(bins[offset]>>mask);
        } break;
        case FMT_BW8:
        {
            uint8_t* bins = (uint8_t*)data;
            rgb = !!bins[offset];
        }break;
        case FMT_GRAY8:
        {
            uint8_t* bins = (uint8_t*)data;
            rgb = bins[offset];
        }break;
        case FMT_R2G3B3:
        {
            uint8_t* bins = (uint8_t*)data;
            uint8_t samp = bins[offset];
            int red = samp>>6;
            int green = (samp>>3)&7;
            int blue = samp&7;
            rgb = (red<<16)|(green<<8)|(blue);
        }break;
        case FMT_R5G6B5:
        {
            uint16_t* bins = (uint16_t*)data;
            uint16_t samp = bins[offset];
            int red = samp>>11;
            int green = (samp>>5)&63;
            int blue = samp&31;
            rgb = (red<<16)|(green<<8)|blue;
        }break;
        case FMT_RGB24:
        {
            uint8_t* bins = (uint8_t*)data;
            int red = bins[offset*3];
            int green = bins[offset*3+1];
            int blue = bins[offset*3+2];
            rgb = (red<<16)|(green<<8)|blue;
        }break;
        case FMT_RGB32:
        {
            uint32_t* bins = (uint32_t*)data;
            rgb = bins[offset/4]>>8;
        }break;
    }
    return rgb;
}

void writePBM(int type, int width, int height, void* data, int format, FILE* out){
    fprintf(out,"P%d\n%d %d\n",type,width,height);
    if(type!=1&&type!=4)fprintf(out,"255\n");
    int size = width*height;
    int i;
    for(i=0;i<size;i++){
        uint32_t rgb = pullPBMDataFrom(type,i,data,format);
        switch(type){
            case 1:
            case 2:
                fprintf(out,"%d ",rgb);
                break;
            case 3:
            {
                int red = rgb>>16;
                int green = (rgb>>8)&0xff;
                int blue = rgb&0xff;
                fprintf(out,"%d %d %d ",red,green,blue);
            }break;
            case 4:
            case 5:
                fputc(rgb,out);
                break;
            case 6:{
                int red = rgb>>16;
                int green = (rgb>>8)&0xff;
                int blue = rgb&0xff;
                fputc(red,out);
                fputc(green,out);
                fputc(blue,out);
            }break;
        }
    }
}
