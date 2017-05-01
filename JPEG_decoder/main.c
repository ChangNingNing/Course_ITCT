#include <stdio.h>

/* decode.c */
#include <stdint.h>
#include <string.h>
#include "bmp.h"
//#include <math.h>

#define MAXL (1<<10)
#define MAXS (MAXL*MAXL)
#define BLOCKS 64

uint16_t P, Y, X, Nf, Hmax = 0, Vmax = 0;
int DCP[4];
uint16_t QT[4][BLOCKS];
uint8_t F[4][5]; //0: H, 1: V, 2: QT_ID, 3: DC_ID, 4: AC_ID.
uint16_t HT[2][2][1<<16]; // 0: DC, 1: AC;

uint8_t BGR[MAXL*MAXL][3];

void Decode_APP(char **buff, int *readSz){
	uint8_t *tmp = *buff;
	uint16_t size = (((uint16_t)tmp[0]) << 8) + tmp[1];
	*buff += size;
	*readSz += size;
}
void Decode_COM(char **buff, int *readSz){
	uint8_t *tmp = *buff;
	uint16_t size = (((uint16_t)tmp[0]) << 8) + tmp[1];
	*buff += size;
	*readSz += size;
}

void Decode_DQT(char **buff, int *readSz){
	uint8_t *tmp = *buff;
	uint16_t size = (((uint16_t)tmp[0]) << 8) + tmp[1];
	uint16_t tmpSz = 2;
	tmp += 2;
	while (tmpSz < size){
		uint8_t pq = (tmp[0]) >> 4;
		uint8_t tid = (tmp[0]) & 0xF;
		tmp += 1, tmpSz += 1;
		for (int i=0; i<BLOCKS; i++){
			if (pq == 0){
				QT[tid][i] = tmp[0];
				tmp += 1, tmpSz += 1;
			}
			else{
				QT[tid][i] = (((uint16_t)tmp[0]) << 8) + tmp[1];
				tmp += 2, tmpSz += 1;
			}
		}
/*
		// Debug
		printf("Lq: %d\n", size);
		printf("Pq: %d Tq: %d\n", pq, tid);
		printf("In Parsing order (Zig-Zag-scan order):\n");
		for (int i=0; i<BLOCKS; i++)
			printf("%d ", QT[tid][i]);
		puts("");
*/
	}

	*buff += size;
	*readSz += size;
}

void Decode_SOF0(char **buff, int *readSz){
	uint8_t *tmp = *buff;
	uint16_t size = (((uint16_t)tmp[0]) << 8) + tmp[1];
	P = (uint8_t)tmp[2];
	Y = (((uint16_t)tmp[3]) << 8) + tmp[4];
	X = (((uint16_t)tmp[5]) << 8) + tmp[6];
	Nf = tmp[7];
	tmp += 8;
	for (int i=0; i<Nf; i++){
		uint8_t _ID = tmp[0];
		F[_ID][0] = tmp[1] >> 4;
		F[_ID][1] = tmp[1] & 0xF;
		F[_ID][2] = tmp[2];
		tmp += 3;

		if (F[_ID][0] > Hmax) Hmax = F[_ID][0];
		if (F[_ID][1] > Vmax) Vmax = F[_ID][1];
	}
/*
	// Debug
	printf("Lf: %d P: %d Y: %d X: %d Nf: %d\n", size, P, Y, X, Nf);
	for (int i=0; i<=Nf; i++)
		printf("Component: %d  H: %d  V: %d  Tq: %d\n", i, F[i][0], F[i][1], F[i][2]);
*/
	*buff += size;
	*readSz += size;
}

void Decode_DHT(char **buff, int *readSz){
	uint8_t *tmp = *buff;
	uint16_t size = (((uint16_t)tmp[0]) << 8) + tmp[1];
	uint16_t tmpSz = 2;
	tmp += 2;

	while (tmpSz < size){
		uint8_t type = (tmp[0]) >> 4;
		uint8_t _ID = (tmp[0]) & 0xF;
		tmp += 1, tmpSz += 1;

		uint8_t node[16];
		for (int i=0; i<16; i++)
			node[i] = tmp[i];
		tmp += 16, tmpSz += 16;

		for (int i=0; i<(1<<16); i++)
			HT[type][_ID][i] = 0xF000;

		uint16_t codeword = 0;
		int codeL = 1;
		for (int i=0; i<16; i++){
			for (int j=0; j<node[i]; j++){
				HT[type][_ID][codeword] = (codeL << 8) + tmp[0];
				tmp += 1, tmpSz += 1;
				codeword += 1;
			}
			codeword <<= 1;
			codeL += 1;
		}
/*
		// Debug
		printf("Lh: %d\n", size);
		printf("Tc: %d Th: %d\n", type, _ID);
		printf("Li: ");
		for (int i=0; i<16; i++)
			printf("%d ", node[i]);
		puts("");
		for (int i=0; i<65536; i++){
			if (HT[type][_ID][i]==0xF000) continue;
			printf("%16x ,%2x\n", i, HT[type][_ID][i] & 0xFF);
		}
*/
	}

	*buff += size;
	*readSz += size;
}

int Decode_SOS_MCU_FF(uint8_t *tmp){
	if (tmp[0] != 0xFF) return 0;
	int shift = 0;
	while (tmp[1] == 0xff){
		tmp += 1;
		shift += 1;
	}
	if (tmp[1] == 0x00){
		tmp[1] = 0xFF;
		return shift + 1;
	}
	if (tmp[1] == 0xD9){
		puts("\n----- 0xFFD9 in MCU.");
		return shift;
	}
	if (tmp[1] >= 0xD0 && tmp[1] <= 0xD7){
		puts("\n----- RSTn in MCU.");
		return shift + 2;
	}
	return shift + 1;
}

inline int16_t Decode_SOS_MCU_Block_Extend(int16_t DIFF, uint8_t S){
	int16_t ret = 0;
	if (S == 0) return ret;
	if (DIFF < (1 << (S-1))){
		ret = -((1 << S)-1) + DIFF;
	}
	else {
		DIFF -= (1 << (S-1));
		ret = (1 << (S-1)) + DIFF;
	}
	return ret;
}

uint8_t Decode_SOS_MCU_Block_Huffman(char **buff, int *readSz, int *rest, int ht_type, int ht_ID){
	uint8_t *tmp = *buff;
	uint16_t size = 0;

	uint16_t codeword = 0, codeL = 0;
	uint16_t S = 0;

	while (1){
		if (*rest <= 0){
			tmp += 1;
			size += 1;
			*rest = 8;
		}
		if (*rest == 8 && tmp[0] == 0xFF){
			int shift = Decode_SOS_MCU_FF( tmp );
			tmp += shift;
			size += shift;
		}
		codeL += 1;
		codeword <<= 1;
		codeword += (tmp[0] >> (*rest-1)) & 0x1;
		*rest -= 1;

		S = HT[ht_type][ht_ID][codeword];
		if (S != 0xF000){
			if ((S >> 8) == codeL)
				break;
		}
	}

	*buff += size;
	*readSz += size;
	return (uint8_t)(S & 0xFF);
}

uint16_t Decode_SOS_MCU_Block_Bits(char **buff, int *readSz, int *rest, uint8_t S){
	uint8_t *tmp = *buff;
	uint16_t size = 0;

	uint16_t DIFF = 0;
	uint8_t tmpW = S;
	while (tmpW){
		if (*rest <= 0){
			tmp += 1;
			size += 1;
			*rest = 8;
		}
		if (*rest == 8 && tmp[0] == 0xFF){
			int shift = Decode_SOS_MCU_FF( tmp );
			tmp += shift;
			size += shift;
		}
		if (tmpW <= *rest){
			DIFF <<= tmpW;
			DIFF += ((tmp[0] >> (*rest - tmpW)) & ((1 << tmpW)-1));
			*rest -= tmpW;
			tmpW = 0;
		}
		else {
			DIFF <<= *rest;
			DIFF += (tmp[0] & ((1 << *rest)-1));
			tmpW -= *rest;
			*rest = 0;
		}
	}

	*buff += size;
	*readSz += size;
	return DIFF;
}

inline void Decode_SOS_MCU_Block_Dequantize(int16_t Block[BLOCKS], int fID){
	int QT_ID = F[fID][2];
	for (int i=0; i<BLOCKS; i++)
		Block[i] *= QT[QT_ID][i];

	/*
	// Debug
	for (int i=0; i<BLOCKS; i++)
		printf("%d ", Block[i]);
	puts("");
	*/
}

inline void Decode_SOS_MCU_Block_Zigzag(int16_t Block[BLOCKS]){
	static const uint8_t _T[BLOCKS] = {
		0,	1,	5,	6,	14,	15,	27,	28,
		2,	4,	7,	13,	16,	26,	29,	42,
		3,	8,	12,	17,	25,	30,	41,	43,
		9,	11,	18,	24,	31,	40,	44,	53,
		10,	19,	23,	32,	39,	45,	52,	54,
		20,	22,	33,	38,	46,	51,	55,	60,
		21,	34,	37,	47,	50,	56,	59,	61,
		35,	36,	48,	49,	57,	58,	62,	63
	};

	int16_t tmp[BLOCKS];
	memcpy(tmp, Block, sizeof(int16_t)*BLOCKS);
	for (int i=0; i<BLOCKS; i++)
		Block[i] = tmp[_T[i]];

	/*
	// Debug
	for (int i=0; i<8; i++, puts(""))
		for (int j=0; j<8; j++)
			printf("%6d", Block[i*8+j]);
	*/
}

inline void Decode_SOS_MCU_Block_IDCT(int16_t Block[BLOCKS]){

	static const float cosT[BLOCKS] = {
	1.000000000000,	0.980785280403,	0.923879532511,	0.831469612303,	0.707106781187,	0.555570233020,	0.382683432365,	0.195090322016,
	1.000000000000,	0.831469612303,	0.382683432365,	-0.195090322016,-0.707106781187,-0.980785280403,-0.923879532511,-0.555570233019,
	1.000000000000,	0.555570233020,	-0.382683432365,-0.980785280403,-0.707106781186,0.195090322016,	0.923879532511,	0.831469612302,
	1.000000000000,	0.195090322016,	-0.923879532511,-0.555570233019,0.707106781187,	0.831469612302,	-0.382683432366,-0.980785280403,
	1.000000000000,	-0.195090322016,-0.923879532511,0.555570233020,	0.707106781186,	-0.831469612303,-0.382683432364,0.980785280403,
	1.000000000000,	-0.555570233020,-0.382683432365,0.980785280403,	-0.707106781187,-0.195090322015,0.923879532511,	-0.831469612303,
	1.000000000000,	-0.831469612303,0.382683432365,	0.195090322016,	-0.707106781186,0.980785280403,	-0.923879532512,0.555570233021,
	1.000000000000,	-0.980785280403,0.923879532511,	-0.831469612303,0.707106781187,	-0.555570233020,0.382683432366,	-0.195090322017
	};
	static const float C[8] = {
	0.7071067811865475,	1,	1,	1,	1,	1,	1,	1
	};
/*
	float cosT[BLOCKS], C[8];
#define PI 3.14159265359
	for (int i=0; i<8; i++)
		for (int j=0; j<8; j++)
			cosT[i*8 + j] = cos((2*i+1)*j*PI / 16);
	for (int i=0; i<8; i++, puts(""))
		for (int j=0; j<8; j++)
			printf("%.12f, ", cosT[i*8 + j]);
	for (int i=0; i<8; i++){
		if (i==0)
			C[i] = 0.7071067811865475;
		else
			C[i] = 1;
	}
#undef PI
*/

	int16_t tmp[BLOCKS];
	memcpy(tmp, Block, sizeof(int16_t)*BLOCKS);

	for (int i=0; i<8; i++){
		for (int j=0; j<8; j++){
			float sum = 0;
			for (int u=0; u<8; u++){
				for (int v=0; v<8; v++){
					sum += C[u]*C[v]*tmp[u*8+v]*cosT[i*8+u]*cosT[j*8+v];
				}
			}
			Block[i*8+j] = sum / 4;
		}
	}
	/*
	// Debug
	for (int i=0; i<8; i++, puts(""))
		for (int j=0; j<8; j++)
			printf("%6d", Block[i*8+j]);
	*/

	/* level shift (+128) */
	for (int i=0; i<BLOCKS; i++)
		Block[i] += 128;

	/*
	// Debug
	for (int i=0; i<8; i++, puts(""))
		for (int j=0; j<8; j++)
			printf("%6d", Block[i*8+j]);
	*/
}

void Decode_SOS_MCU_Block(char **buff, int *readSz, int *rest, int16_t Block[BLOCKS], int fID){
	uint8_t S = Decode_SOS_MCU_Block_Huffman(buff, readSz, rest, 0, F[fID][3]);
	int16_t DIFF = Decode_SOS_MCU_Block_Bits(buff, readSz, rest, S);
	int16_t ExtendDIFF = Decode_SOS_MCU_Block_Extend(DIFF, S);
	DCP[fID] += ExtendDIFF;
	Block[0] = DCP[fID];

	/*
	// Debug
	printf("DC Predicter: %d\n", DCP[fID]);
	printf("DC:\n");
	printf("    T: %d", S);
	printf("    DIFF: %d", DIFF);
	printf("    EXTEND(): %d\n", ExtendDIFF);
	*/

	int pos = 1;
	uint8_t R;
	while (pos < BLOCKS){
		S = Decode_SOS_MCU_Block_Huffman(buff, readSz, rest, 1, F[fID][4]);
		R = ((S & 0xF0) >> 4);
		S &= 0x0F;
		if (R == 0 && S == 0){
			break;
		}
		DIFF = Decode_SOS_MCU_Block_Bits(buff, readSz, rest, S);
		ExtendDIFF = Decode_SOS_MCU_Block_Extend(DIFF, S);
		for (int i=0; i<R; i++, pos++)
			Block[pos] = 0;
		Block[pos++] = ExtendDIFF;
		/*
		// Debug
		printf("AC:\n");
		printf("    RS: 0x%x%x", R, S);
		printf("    ZZ(K): %d", DIFF);
		printf("    EXTEND(): %d\n", ExtendDIFF);
		*/
	}
	for (; pos < BLOCKS; pos++)
		Block[pos] = 0;

	/* DeQuantize */
	Decode_SOS_MCU_Block_Dequantize(Block, fID);

	/* Zig-zag order */
	Decode_SOS_MCU_Block_Zigzag(Block);

	/* IDCT */
	Decode_SOS_MCU_Block_IDCT(Block);
}

void Decode_SOS_MCU_RGB(int16_t YCC[][BLOCKS], int indexY, int indexX){
	static const int16_t shift[] = {	0,	-128, -128};
	static const float coef[] = {	1,		1,			1,
									1.772,	-0.34414,	0,
									0,		-0.71414,	1.402		};
	int nF = 0, nC = 0;
	float BGRtmp[Hmax*Vmax*8*8][3];
	memset(BGRtmp, 0, sizeof(BGRtmp));
	for (int f=0; f<4; f++){
		if (F[f][0] == 0 || F[f][1] == 0) continue;
		int xScale = Hmax / F[f][0];
		int yScale = Vmax / F[f][1];
		for (int y=0; y<Vmax; y++){
			for (int x=0; x<Hmax; x++){
				int yccX = (x / xScale);
				int yccY = (y / yScale) * F[f][0];
				int yccItmp = (y % yScale)*(8 / yScale);
				int yccJtmp = (x % xScale)*(8 / xScale);
				for (int i=0; i<8; i++){
					for (int j=0; j<8; j++){
						int yccI = (i / yScale + yccItmp) * 8;
						int yccJ = (j / xScale + yccJtmp);
						int rgbY = (y*8 + i) * Hmax * 8;
						int rgbX = (x*8 + j);
						int16_t tmp = YCC[nF + yccY + yccX][yccI + yccJ];
						BGRtmp[rgbY + rgbX][0] += coef[nC*3+0] * (tmp + shift[nC]);
						BGRtmp[rgbY + rgbX][1] += coef[nC*3+1] * (tmp + shift[nC]);
						BGRtmp[rgbY + rgbX][2] += coef[nC*3+2] * (tmp + shift[nC]);
					}
				}
			}
		}

		nF += F[f][0]*F[f][1];
		nC += 1;
	}
	int baseY = indexY * Vmax * 8;
	int baseX = indexX * Hmax * 8;
	for (int y=0, i=baseY; y<Vmax*8; y++, i++){
		for (int x=0, j=baseX; x<Hmax*8; x++, j++){
			for (int c=0; c<3; c++){
				float tmp = BGRtmp[y*Hmax*8+x][c];
				if (tmp > 255) tmp = 255;
				else if (tmp < 0) tmp = 0;
				BGR[i*MAXL+j][c] = tmp;
			}
		}
	}

	/*
	// Debug
	for (int y=0, i=baseY; y<8; y++, i++){
		for (int x=0, j=baseX; x<8; x++, j++)
			printf("[%3d %3d %3d] ", BGR[i*MAXL+j][0], BGR[i*MAXL+j][1], BGR[i*MAXL+j][2]);
		puts("");
	}
	*/
}

void Decode_SOS_MCU(char **buff, int *readSz, int *rest, int indexY, int indexX){
	uint32_t MCU_nB = 0;
	for (int i=0; i<=Nf; i++)
		MCU_nB += F[i][0] * F[i][1];

	int16_t Block[MCU_nB][BLOCKS];
	MCU_nB = 0;

//	printf("\nMCU(%2d, %2d)\n", indexY, indexX);
	for (int f=0; f<4; f++){
		for (int i=0; i<F[f][0]*F[f][1]; i++, MCU_nB++){
//			printf("\n----- MCU_nB: %d\n", MCU_nB);
			Decode_SOS_MCU_Block(buff, readSz, rest, Block[MCU_nB], f);
		}
	}

	Decode_SOS_MCU_RGB(Block, indexY, indexX);
}

void Decode_SOS(char **buff, int *readSz){
	uint8_t *tmp = *buff;
	uint16_t size = (((uint16_t)tmp[0])<<8) + tmp[1];
	if (Nf != tmp[2]){
		puts("----- Nf not the same");
		return;
	}
	tmp += 3;

	for (int i=0; i<Nf; i++){
		uint8_t _ID = tmp[0];
		F[_ID][3] = tmp[1] >> 4;
		F[_ID][4] = tmp[1] & 0xF;
		tmp += 2;
	}
	// 0x00 0x3F 0x00
	tmp += 2;

	*buff += size - 1;
	*readSz += size - 1;
	int rest = 0;

	uint32_t MCU_X = (X + (Hmax*8 - 1)) / (Hmax * 8);
	uint32_t MCU_Y = (Y + (Vmax*8 - 1)) / (Vmax * 8);

	for (int i=0; i<MCU_Y; i++){
		for (int j=0; j<MCU_X; j++){
			Decode_SOS_MCU(buff, readSz, &rest, i, j);
		}
	}
	*buff += 1;
	*readSz += 1;
}

int Decode(char *readBuff, int readBuffSz){
	char *scanPtr = readBuff;
	uint8_t *symbol;
	int scanNum = 0;
	while (scanNum <= readBuffSz){
		symbol = scanPtr;
		scanPtr += 2, scanNum += 2;
		if (*symbol != 0xFF){
			puts("undefine symbol error 1.");
			printf("----- symbol = %x at %x\n", *symbol, scanNum);
			return 0;
		}
		symbol += 1;
		// [SOI] : start of image
		if (*symbol == 0xD8){
			puts("[SOI] start of image ...");
		}

		// [APPn]
		else if (*symbol >= 0xE0 && *symbol <= 0xEF){
			printf("[APP%d] ...\n", *symbol & 0xF);
			Decode_APP(&scanPtr, &scanNum);
		}

		// [COM] command
		else if (*symbol == 0xFE){
			puts("[COM] command ...");
			Decode_COM(&scanPtr, &scanNum);
		}

		// [DQT] : define quantization table
		else if (*symbol == 0xDB){
			puts("[DQT] define quantization table ...");
			Decode_DQT(&scanPtr, &scanNum);
		}

		// [SOF] : start of frame
		else if ((*symbol >= 0xC0 && *symbol <= 0xC3) || (*symbol >= 0xC5 && *symbol <= 0xC7)){
			printf("[SOF%d] start of frame ...\n", *symbol & 0xF);
			if ((*symbol & 0xF) == 0)
				Decode_SOF0(&scanPtr, &scanNum);
			else {
				puts("----- not supported.");
				return 0;
			}
		}

		// [DHT] : define huffman table
		else if (*symbol == 0xC4){
			puts("[DHT] define huffman table ...");
			Decode_DHT(&scanPtr, &scanNum);
		}

		// [SOS] : start of scan
		else if (*symbol == 0xDA){
			puts("[SOS] start of scan ...");
			Decode_SOS(&scanPtr, &scanNum);
		}

		// [EOI] : end of image
		else if (*symbol == 0xD9){
			puts("[EOI] end of image ...");
			break;
		}

		// ignore current 0xFF
		else if (*symbol == 0xFF){
			scanPtr -= 1;
			scanNum -= 1;
		}
		else {
			puts("undefine symbol error 2.");
			printf("----- symbol = %x at %x\n", *symbol, scanNum);
			return 0;
		}
	}
	return 1;
}

int ReadFile(char *fn, char *buff, int *buffsize){
	FILE *fptr = fopen(fn, "rb");
	if (!fptr) return 0;
	fseek(fptr, 0L, SEEK_END);
	*buffsize = ftell(fptr);
	fseek(fptr, 0L, SEEK_SET);
	if (*buffsize > MAXS){
		puts("input file is too large.");
		return 0;
	}
	fread(buff, sizeof(char), *buffsize, fptr);
	fclose(fptr);
	return 1;
}

int WriteFile(char *fn, char *buff){
	uint8_t *ptr = buff;
	uint32_t size = 0;
	uint32_t MCU_X = (X + (Hmax*8 - 1)) / (Hmax * 8);
	uint32_t MCU_Y = (Y + (Vmax*8 - 1)) / (Vmax * 8);
	uint32_t Xmax = MCU_X * Hmax * 8;
	uint32_t Ymax = MCU_Y * Vmax * 8;

	BmpHeader header;
	BmpImageInfo info;

	memcpy(ptr, "BM", 2*sizeof(char));
	ptr += 2, size += 2;

	header.fileSize = Ymax * Xmax * 3 + 54;
	header.reserved = 0;
	header.offset = 54;
	memcpy(ptr, &header, sizeof(BmpHeader));
	ptr += sizeof(BmpHeader), size += sizeof(BmpHeader);

	info.headerSize = 40;
	info.width = Xmax;
	info.height = Ymax;
	info.planes = 1;
	info.bitsPerPixels = 24;
	info.compression = 0;
	info.bitmapDataSize = Ymax * Xmax * 3;
	info.HResolution = 0x0B12;
	info.VResolution = 0x0B12;
	info.usedColors = 0;
	info.importantColors = 0;
	memcpy(ptr, &info, sizeof(BmpImageInfo));
	ptr += sizeof(BmpImageInfo), size += sizeof(BmpImageInfo);

	int l = sizeof(uint8_t) * Xmax * 3;
	for (int y = Ymax - 1; y>=0; y--){
		if (size + l > MAXS * 3){
			puts("output file is too large.");
			return 0;
		}

		memcpy(ptr, BGR[y*MAXL], l);
		ptr += l, size += l;
	}

	FILE *fptr = fopen(fn, "wb");
	if (!fptr) return 0;
	fwrite(buff, sizeof(uint8_t), size, fptr);
	fclose(fptr);

	return 1;
}

/* decode.c */

char readBuff[MAXS], writeBuff[MAXS*3];
int readBuffSz;
int main(int argc, char *argv[]){
	if (argc < 3){
		puts("usage : output [input_file] [output_file]");
		return 0;
	}

	if (!ReadFile( argv[1], readBuff, &readBuffSz )){
		puts("Read file failed.");
		return 0;
	}

	if (!Decode( readBuff, readBuffSz )){
		puts("Decode file failed.");
		return 0;
	}

	if (!WriteFile( argv[2], writeBuff )){
		puts("Write file failed.");
		return 0;
	}

	return 0;
}
