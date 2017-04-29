#include <stdio.h>

/* decode.c */
#include <stdint.h>

#define MAXS (1<<20)
#define BLOCKS 64

uint16_t P, Y, X, Nf, Hmax = 0, Vmax = 0;
int DCP[4];
uint16_t QT[4][BLOCKS];
uint8_t F[4][5]; //0: H, 1: V, 2: QT_ID, 3: DC_ID, 4: AC_ID.
uint16_t HT[2][2][1<<16]; // 0: DC, 1: AC;

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

int16_t Decode_SOS_MCU_Block_Extend(int16_t DIFF, uint8_t S){
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

void Decode_SOS_MCU_Block(char **buff, int *readSz, int *rest, int16_t Block[BLOCKS], int fID, int ht_dc_id, int ht_ac_id){
printf("DC Predictot: %d\n", DCP[fID]);
printf("DC:\n");
	uint8_t S = Decode_SOS_MCU_Block_Huffman(buff, readSz, rest, 0, ht_dc_id);
printf("    T: %d", S);
	int16_t DIFF = Decode_SOS_MCU_Block_Bits(buff, readSz, rest, S);
printf("    DIFF: %d", DIFF);
	DIFF = Decode_SOS_MCU_Block_Extend(DIFF, S);
printf("    EXTEND(): %d\n", DIFF);
	DCP[fID] += DIFF;
	Block[0] = DCP[fID];

	int pos = 1;
	uint8_t R;
printf("AC:\n");
	while (pos < BLOCKS){
		S = Decode_SOS_MCU_Block_Huffman(buff, readSz, rest, 1, ht_ac_id);
		R = ((S & 0xF0) >> 4);
		S &= 0x0F;
printf("    RS: 0x%x%x", R, S);
		if (R == 0 && S == 0){
puts("");
			break;
		}
		DIFF = Decode_SOS_MCU_Block_Bits(buff, readSz, rest, S);
printf("    ZZ(K): %d", DIFF);
		DIFF = Decode_SOS_MCU_Block_Extend(DIFF, S);
printf("    EXTEND(): %d\n", DIFF);
		for (int i=0; i<R; i++, pos++)
			Block[pos] = 0;
		Block[pos++] = DIFF;
	}
	for (; pos < BLOCKS; pos++)
		Block[pos] = 0;
}

void Decode_SOS_MCU(char **buff, int *readSz, int *rest){
	uint32_t MCU_nB = 0;
	for (int i=0; i<=Nf; i++)
		MCU_nB += F[i][0] * F[i][1];

	int16_t Block[MCU_nB][BLOCKS];
	MCU_nB = 0;

	for (int f=0; f<=Nf; f++){
		for (int i=0; i<F[f][0]*F[f][1]; i++, MCU_nB++){
			printf("\n----- MCU_nB: %d\n", MCU_nB);
			Decode_SOS_MCU_Block(buff, readSz, rest, Block[MCU_nB], f, F[f][3], F[f][4]);
		}
	}
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
			printf("\nMCU(%2d, %2d)\n", i, j);
			Decode_SOS_MCU(buff, readSz, &rest);
		}
	}
	*buff += 1;
	*readSz += 1;
}

int Decode(char *readBuff, int readBuffSz, char *writeBuff, int *writeBuffSz){
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
	fread(buff, sizeof(char), *buffsize, fptr);
	fclose(fptr);
	return 1;
}

int WriteFile(char *fn, char *buff, int buffsize){
	FILE *fptr = fopen(fn, "wb");
	if (!fptr) return 0;
	fwrite(buff, sizeof(char), buffsize, fptr);
	fclose(fptr);
	return 1;
}

/* decode.c */

char readBuff[MAXS], writeBuff[MAXS];
int readBuffSz, writeBuffSz;
int main(int argc, char *argv[]){
	if (argc < 3){
		puts("usage : output [input_file] [output_file]");
		return 0;
	}

	if (!ReadFile( argv[1], readBuff, &readBuffSz )){
		puts("Read file failed.");
		return 0;
	}

	if (!Decode( readBuff, readBuffSz, writeBuff, &writeBuffSz )){
		puts("Decode file failed.");
		return 0;
	}

	if (!WriteFile( argv[2], writeBuff, writeBuffSz )){
		puts("Write file failed.");
		return 0;
	}
	return 0;
}
