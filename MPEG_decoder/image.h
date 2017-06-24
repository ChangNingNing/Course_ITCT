#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#define MAXL 1024
#define MAXF 120

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

typedef struct {
	int16_t y[MAXL][MAXL];
	int16_t cb[MAXL][MAXL];
	int16_t cr[MAXL][MAXL];
} Frame;


class Image {
	public:
		Image();
		~Image();
		void inputYUV(const int& cur_addr, int bid, const int& mb_row, const int& mb_col, const int block[8][8]);
		void outputFrame(int pid, const int& height, const int& width);
		void outputBMP(int pid, const int& height, const int& width, const char* fout);

		static int16_t image_buf[3][3][MAXL][MAXL];
		static Frame frame[MAXF];
		uint32_t frameNum;
	private:
		static uint8_t BGR[MAXL*MAXL*3];
};

#endif
