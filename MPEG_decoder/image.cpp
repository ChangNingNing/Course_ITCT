#include "image.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define filter(x) ((x)>255? 255: (x)<0? 0: (x))
int16_t Image::image_buf[3][3][MAXL][MAXL];
uint8_t Image::BGR[MAXL*MAXL*3];

Image::Image(){}

Image::~Image(){}

void Image::inputYUV(	const int& cur_addr, int bid,
						const int& mb_row, const int& mb_col, const int block[8][8]){
	int row, col, tid;
	// Y
	if (bid < 4){
		row = (mb_row << 4) + ((bid >> 1) << 3);
		col = (mb_col << 4) + ((bid &  1) << 3);
		tid = 0;
	}
	// Cb, Cr
	else {
		row = mb_row << 3;
		col = mb_col << 3;
		tid = bid-3;
	}

	for (int i=0; i<8; i++)
		for (int j=0; j<8; j++)
			image_buf[cur_addr][tid][row+i][col+j] = block[i][j];
}

void Image::outputBMP(int pid, const int& height, const int& width, const char* fout){
	for (int i=0; i<height; i++){
		for (int j=0; j<width; j++){
			int _B = (i*MAXL+j)*3 + 0;
			int _G = (i*MAXL+j)*3 + 1;
			int _R = (i*MAXL+j)*3 + 2;
			float tmp[3];
			float y  = image_buf[pid][0][i][j];
			float cb = image_buf[pid][1][i/2][j/2]-128.0;
			float cr = image_buf[pid][2][i/2][j/2]-128.0;

			tmp[0] = y + 1.772*cb;
			tmp[1] = y - 0.34414*cb - 0.71414*cr;
			tmp[2] = y + 1.402*cr;

			BGR[_B] = filter((int)tmp[0]);
			BGR[_G] = filter((int)tmp[1]);
			BGR[_R] = filter((int)tmp[2]);
		}
	}

	/* write to .bmp */
	uint8_t *buff = (uint8_t *)malloc(sizeof(uint8_t)*MAXL*MAXL*3);
	uint8_t *ptr = buff;
	int size = 0;

	int Wmax = (3*width + 3) / 4 * 4;
	int Hmax = height;

	BmpHeader header;
	BmpImageInfo info;

    memcpy(ptr, "BM", 2*sizeof(char));
    ptr += 2, size += 2;

    header.fileSize = Hmax * Wmax + 54;
    header.reserved = 0;
    header.offset = 54;
    memcpy(ptr, &header, sizeof(BmpHeader));
    ptr += sizeof(BmpHeader), size += sizeof(BmpHeader);

    info.headerSize = 40;
    info.width = width;
    info.height = height;
    info.planes = 1;
    info.bitsPerPixels = 24;
    info.compression = 0;
    info.bitmapDataSize = Wmax * Hmax;
    info.HResolution = 0x0B12;
    info.VResolution = 0x0B12;
    info.usedColors = 0;
    info.importantColors = 0;
    memcpy(ptr, &info, sizeof(BmpImageInfo));
    ptr += sizeof(BmpImageInfo), size += sizeof(BmpImageInfo);

    int l = sizeof(uint8_t) * Wmax;
    for (int y = Hmax - 1; y>=0; y--){
        if (size + l > MAXL*MAXL * 3){
            puts("output file is too large.");
			exit(1);
        }
        memcpy(ptr, &BGR[y*MAXL*3], l);
        ptr += l, size += l;
    }

    FILE *fptr = fopen(fout, "wb");
    if (!fptr) exit(1);
    fwrite(buff, sizeof(uint8_t), size, fptr);
    fclose(fptr);	
	free(buff);
}
