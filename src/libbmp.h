#ifndef _H_LIBBMP
#define _H_LIBBMP

#include <stdint.h>

#define BI_RGB 0
#define BI_RLE8 1
#define BI_RLE4 2
#define BI_BITFIELDS 3
#define BI_ALPHABITFIELDS 6

#define BITMAPCOREHEADER 12
#define BITMAPINFOHEADER 40
#define BITMAPINFOHEADERV2 52
#define BITMAPINFOHEADERV3 56

typedef struct __attribute__((__packed__)) _bmp_header{
    char magic[2];
    uint32_t size;
    char res0[2];
    char res1[2];
    uint32_t offset;
} bmp_header;

typedef struct __attribute__((__packed__)) _dibh_core{
    uint32_t header_size;
    uint16_t width;
    uint16_t height;
    uint16_t planes;
    uint16_t bpp;
} dibh_core;

typedef struct __attribute__((__packed__)) _dibh_info{
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compression;
    uint32_t img_size;
    int32_t res_x;
    int32_t res_y;
    uint32_t palette;
    uint32_t important;
} dibh_info;

typedef struct __attribute__((__packed__)) _dibh_info_v2{
    dibh_info v1;
    uint32_t red;
    uint32_t green;
    uint32_t blue;
} dibh_info_v2;

typedef struct __attribute__((__packed__)) _dibh_info_v3{
    dibh_info_v2 v2;
    uint32_t alpha;
}

int readBMPHeader(FILE*, bmp_header*, dibh_core**);
void* readBMPData(FILE*, bmp_header*, dibh_core*);
uint32_t* readBMPTable(FILE*, bmp_header*, dibh_core*);
uint32_t* getBMPRGBA(dibh_core*,void*,uint32_t*);
void* putBMPRGBA(dibh_core*,uint32_t*,uint32_t*);

int writeBMPFile(FILE*, bmp_header*, dibh_core*, uint32_t*, void*);

#endif
