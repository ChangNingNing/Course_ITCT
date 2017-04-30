typedef struct{
//	char identifier[2];
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
