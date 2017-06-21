#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

typedef struct{
//  char identifier[2];
    unsigned int fileSize;
    unsigned int reserved;
    unsigned int offset;
} BmpHeader;

typedef struct{
    unsigned int headerSize;
    unsigned int width;
    unsigned int height;
    unsigned short planes;
    unsigned short bitsPerPixels;
    unsigned int compression;
    unsigned int bitmapDataSize;
    unsigned int HResolution;
    unsigned int VResolution;
    unsigned int usedColors;
    unsigned int importantColors;

} BmpImageInfo;


class Image {
	public:
		Image();
		~Image();
		void InputYUV(const int& bid, const int& mb_row, const int& mb_col, const int block[8][8]);
		void OutputBMP(const int& height, const int& weight, const char* fout);
	private:
		static int16_t YUV[3][512][512];
		static uint8_t BGR[512*512*3];
};

#endif
