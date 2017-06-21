#include "image.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define filter(x) ((x)>255? 255: (x)<0? 0: (x))
int16_t Image::YUV[3][512][512];
uint8_t Image::BGR[512*512*3];

Image::Image(){}

Image::~Image(){}

void Image::InputYUV(const int& bid, const int& mb_row, const int& mb_col, const int block[8][8]){
	int row = mb_row * 16;
	int col = mb_col * 16;
	// Y
	if (bid < 4){
		row += (bid / 2) * 8;
		col += (bid % 2) * 8;
		for (int i=0; i<8; i++){
			for (int j=0; j<8; j++){
				YUV[0][row+i][col+j] = block[i][j];
			}
		}
	}
	// Cb, Cr
	else {
		int _id = bid-3;
		for (int i=0; i<16; i+=2){
			for (int j=0; j<16; j+=2){
				YUV[_id][row+i+0][col+j+0] = block[i/2][j/2];
				YUV[_id][row+i+0][col+j+1] = block[i/2][j/2];
				YUV[_id][row+i+1][col+j+0] = block[i/2][j/2];
				YUV[_id][row+i+1][col+j+1] = block[i/2][j/2];
			}
		}
	}
}

void Image::OutputBMP(const int& height, const int& width, const char* fout){
	for (int i=0; i<height; i++){
		for (int j=0; j<width; j++){
			int _B = (i*512+j)*3 + 0;
			int _G = (i*512+j)*3 + 1;
			int _R = (i*512+j)*3 + 2;
			int16_t tmp[3];
			tmp[0] = YUV[0][i][j] + 1.772*(YUV[1][i][j] - 128);
			tmp[1] = YUV[0][i][j] - 0.34414*(YUV[1][i][j] - 128) - 0.71414*(YUV[2][i][j]-128);
			tmp[2] = YUV[0][i][j] + 1.402*(YUV[2][i][j] - 128);

			BGR[_B] = filter(tmp[0]);
			BGR[_G] = filter(tmp[1]);
			BGR[_R] = filter(tmp[2]);
		}
	}

	/* write to .bmp */
	uint8_t *buff = (uint8_t *)malloc(sizeof(uint8_t)*512*512*3);
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
        if (size + l > 512 * 512 * 3){
            puts("output file is too large.");
			exit(1);
        }
        memcpy(ptr, &BGR[y*512*3], l);
        ptr += l, size += l;
    }

    FILE *fptr = fopen(fout, "wb");
    if (!fptr) exit(1);
    fwrite(buff, sizeof(uint8_t), size, fptr);
    fclose(fptr);	
}
