#ifndef _H_LIBPBM
#define _H_LIBPBM

#include <stdio.h>
#include <stdint.h>

#define FMT_BWBIN 0
#define FMT_BW8 1
#define FMT_GRAY8 2
#define FMT_R2G3B3 3
#define FMT_R5G6B5 4
#define FMT_RGB24 5
#define FMT_RGB32 6

extern int FMT_SIZES[];

uint32_t pullBPMFromData(int type, int index, void* data, int format);
void writePBM(int type, int width, int height, void* data, int format, FILE* file);

#endif
