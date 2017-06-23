#include "block.h"
#include "bit.h"

using namespace std;

std::map<Key, int> Block::dct_luminance_VLC_code;
std::map<Key, int> Block::dct_chrominance_VLC_code;
std::map<Key, Key> Block::dct_coeff_VLC_code;

Block::Block(InBit& x, const bool& d): inBit(x), DEBUG(d){
	/* dct_luminance VLC code */
	// only write the table once.
	if (dct_luminance_VLC_code.empty()){
		dct_luminance_VLC_code[Key(0b100, 3)] = 0;
		dct_luminance_VLC_code[Key(0b00, 2)] = 1;
		dct_luminance_VLC_code[Key(0b01, 2)] = 2;
		dct_luminance_VLC_code[Key(0b101, 3)] = 3;
		dct_luminance_VLC_code[Key(0b110, 3)] = 4;
		dct_luminance_VLC_code[Key(0b1110, 4)] = 5;
		dct_luminance_VLC_code[Key(0b11110, 5)] = 6;
		dct_luminance_VLC_code[Key(0b111110, 6)] = 7;
		dct_luminance_VLC_code[Key(0b1111110, 7)] = 8;
	}
	/* dct_chrominance VLC code */
	if (dct_chrominance_VLC_code.empty()){
		dct_chrominance_VLC_code[Key(0b00, 2)] = 0;
		dct_chrominance_VLC_code[Key(0b01, 2)] = 1;
		dct_chrominance_VLC_code[Key(0b10, 2)] = 2;
		dct_chrominance_VLC_code[Key(0b110, 3)] = 3;
		dct_chrominance_VLC_code[Key(0b1110, 4)] = 4;
		dct_chrominance_VLC_code[Key(0b11110, 5)] = 5;
		dct_chrominance_VLC_code[Key(0b111110, 6)] = 6;
		dct_chrominance_VLC_code[Key(0b1111110, 7)] = 7;
		dct_chrominance_VLC_code[Key(0b11111110, 8)] = 8;
	}
	/* dct_coeff VLC code */
	if (dct_coeff_VLC_code.empty()){
		dct_coeff_VLC_code[Key(0b10, 2)] = Key(-1, 0); // end_of_block
		dct_coeff_VLC_code[Key(0b1, 1)] = Key(0, 1); // dct_coeff_first
		dct_coeff_VLC_code[Key(0b11, 2)] = Key(0, 1); // dct_coeff_next
		dct_coeff_VLC_code[Key(0b011, 3)] = Key(1, 1);
		dct_coeff_VLC_code[Key(0b0100, 4)] = Key(0, 2);
		dct_coeff_VLC_code[Key(0b0101, 4)] = Key(2, 1);
		dct_coeff_VLC_code[Key(0b00101, 5)] = Key(0, 3);
		dct_coeff_VLC_code[Key(0b00111, 5)] = Key(3, 1);
		dct_coeff_VLC_code[Key(0b00110, 5)] = Key(4, 1);
		dct_coeff_VLC_code[Key(0b000110, 6)] = Key(1, 2);
		dct_coeff_VLC_code[Key(0b000111, 6)] = Key(5, 1);
		dct_coeff_VLC_code[Key(0b000101, 6)] = Key(6, 1);
		dct_coeff_VLC_code[Key(0b000100, 6)] = Key(7, 1);
		dct_coeff_VLC_code[Key(0b0000110, 7)] = Key(0, 4);
		dct_coeff_VLC_code[Key(0b0000100, 7)] = Key(2, 2);
		dct_coeff_VLC_code[Key(0b0000111, 7)] = Key(8, 1);
		dct_coeff_VLC_code[Key(0b0000101, 7)] = Key(9, 1);
		dct_coeff_VLC_code[Key(0b000001, 6)] = Key(-2, 0); // escape
		dct_coeff_VLC_code[Key(0b00100110, 8)] = Key(0, 5);
		dct_coeff_VLC_code[Key(0b00100001, 8)] = Key(0, 6);
		dct_coeff_VLC_code[Key(0b00100101, 8)] = Key(1, 3);
		dct_coeff_VLC_code[Key(0b00100100, 8)] = Key(3, 2);
		dct_coeff_VLC_code[Key(0b00100111, 8)] = Key(10, 1);
		dct_coeff_VLC_code[Key(0b00100011, 8)] = Key(11, 1);
		dct_coeff_VLC_code[Key(0b00100010, 8)] = Key(12, 1);
		dct_coeff_VLC_code[Key(0b00100000, 8)] = Key(13, 1);
		dct_coeff_VLC_code[Key(0b0000001010, 10)] = Key(0, 7);
		dct_coeff_VLC_code[Key(0b0000001100, 10)] = Key(1, 4);
		dct_coeff_VLC_code[Key(0b0000001011, 10)] = Key(2, 3);
		dct_coeff_VLC_code[Key(0b0000001111, 10)] = Key(4, 2);
		dct_coeff_VLC_code[Key(0b0000001001, 10)] = Key(5, 2);
		dct_coeff_VLC_code[Key(0b0000001110, 10)] = Key(14, 1);
		dct_coeff_VLC_code[Key(0b0000001101, 10)] = Key(15, 1);
		dct_coeff_VLC_code[Key(0b0000001000, 10)] = Key(16, 1);

		dct_coeff_VLC_code[Key(0b000000011101, 12)] = Key(0, 8);
		dct_coeff_VLC_code[Key(0b000000011000, 12)] = Key(0, 9);
		dct_coeff_VLC_code[Key(0b000000010011, 12)] = Key(0, 10);
		dct_coeff_VLC_code[Key(0b000000010000, 12)] = Key(0, 11);
		dct_coeff_VLC_code[Key(0b000000011011, 12)] = Key(1, 5);
		dct_coeff_VLC_code[Key(0b000000010100, 12)] = Key(2, 4);
		dct_coeff_VLC_code[Key(0b000000011100, 12)] = Key(3, 3);
		dct_coeff_VLC_code[Key(0b000000010010, 12)] = Key(4, 3);
		dct_coeff_VLC_code[Key(0b000000011110, 12)] = Key(6, 2);
		dct_coeff_VLC_code[Key(0b000000010101, 12)] = Key(7, 2);
		dct_coeff_VLC_code[Key(0b000000010001, 12)] = Key(8, 2);
		dct_coeff_VLC_code[Key(0b000000011111, 12)] = Key(17, 1);
		dct_coeff_VLC_code[Key(0b000000011010, 12)] = Key(18, 1);
		dct_coeff_VLC_code[Key(0b000000011001, 12)] = Key(19, 1);
		dct_coeff_VLC_code[Key(0b000000010111, 12)] = Key(20, 1);
		dct_coeff_VLC_code[Key(0b000000010110, 12)] = Key(21, 1);
		dct_coeff_VLC_code[Key(0b0000000011010, 13)] = Key(0, 12);
		dct_coeff_VLC_code[Key(0b0000000011001, 13)] = Key(0, 13);
		dct_coeff_VLC_code[Key(0b0000000011000, 13)] = Key(0, 14);
		dct_coeff_VLC_code[Key(0b0000000010111, 13)] = Key(0, 15);
		dct_coeff_VLC_code[Key(0b0000000010110, 13)] = Key(1, 6);
		dct_coeff_VLC_code[Key(0b0000000010101, 13)] = Key(1, 7);
		dct_coeff_VLC_code[Key(0b0000000010100, 13)] = Key(2, 5);
		dct_coeff_VLC_code[Key(0b0000000010011, 13)] = Key(3, 4);
		dct_coeff_VLC_code[Key(0b0000000010010, 13)] = Key(5, 3);
		dct_coeff_VLC_code[Key(0b0000000010001, 13)] = Key(9, 2);
		dct_coeff_VLC_code[Key(0b0000000010000, 13)] = Key(10, 2);
		dct_coeff_VLC_code[Key(0b0000000011111, 13)] = Key(22, 1);
		dct_coeff_VLC_code[Key(0b0000000011110, 13)] = Key(23, 1);
		dct_coeff_VLC_code[Key(0b0000000011101, 13)] = Key(24, 1);
		dct_coeff_VLC_code[Key(0b0000000011100, 13)] = Key(25, 1);
		dct_coeff_VLC_code[Key(0b0000000011011, 13)] = Key(26, 1);

		dct_coeff_VLC_code[Key(0b00000000011111, 14)] = Key(0, 16);
		dct_coeff_VLC_code[Key(0b00000000011110, 14)] = Key(0, 17);
		dct_coeff_VLC_code[Key(0b00000000011101, 14)] = Key(0, 18);
		dct_coeff_VLC_code[Key(0b00000000011100, 14)] = Key(0, 19);
		dct_coeff_VLC_code[Key(0b00000000011011, 14)] = Key(0, 20);
		dct_coeff_VLC_code[Key(0b00000000011010, 14)] = Key(0, 21);
		dct_coeff_VLC_code[Key(0b00000000011001, 14)] = Key(0, 22);
		dct_coeff_VLC_code[Key(0b00000000011000, 14)] = Key(0, 23);
		dct_coeff_VLC_code[Key(0b00000000010111, 14)] = Key(0, 24);
		dct_coeff_VLC_code[Key(0b00000000010110, 14)] = Key(0, 25);
		dct_coeff_VLC_code[Key(0b00000000010101, 14)] = Key(0, 26);
		dct_coeff_VLC_code[Key(0b00000000010100, 14)] = Key(0, 27);
		dct_coeff_VLC_code[Key(0b00000000010011, 14)] = Key(0, 28);
		dct_coeff_VLC_code[Key(0b00000000010010, 14)] = Key(0, 29);
		dct_coeff_VLC_code[Key(0b00000000010001, 14)] = Key(0, 30);
		dct_coeff_VLC_code[Key(0b00000000010000, 14)] = Key(0, 31);
		dct_coeff_VLC_code[Key(0b000000000011000, 15)] = Key(0, 32);
		dct_coeff_VLC_code[Key(0b000000000010111, 15)] = Key(0, 33);
		dct_coeff_VLC_code[Key(0b000000000010110, 15)] = Key(0, 34);
		dct_coeff_VLC_code[Key(0b000000000010101, 15)] = Key(0, 35);
		dct_coeff_VLC_code[Key(0b000000000010100, 15)] = Key(0, 36);
		dct_coeff_VLC_code[Key(0b000000000010011, 15)] = Key(0, 37);
		dct_coeff_VLC_code[Key(0b000000000010010, 15)] = Key(0, 38);
		dct_coeff_VLC_code[Key(0b000000000010001, 15)] = Key(0, 39);
		dct_coeff_VLC_code[Key(0b000000000010000, 15)] = Key(0, 40);
		dct_coeff_VLC_code[Key(0b000000000011111, 15)] = Key(1, 8);
		dct_coeff_VLC_code[Key(0b000000000011110, 15)] = Key(1, 9);
		dct_coeff_VLC_code[Key(0b000000000011101, 15)] = Key(1, 10);
		dct_coeff_VLC_code[Key(0b000000000011100, 15)] = Key(1, 11);
		dct_coeff_VLC_code[Key(0b000000000011011, 15)] = Key(1, 12);
		dct_coeff_VLC_code[Key(0b000000000011010, 15)] = Key(1, 13);
		dct_coeff_VLC_code[Key(0b000000000011001, 15)] = Key(1, 14);
		dct_coeff_VLC_code[Key(0b0000000000010011, 16)] = Key(1, 15);
		dct_coeff_VLC_code[Key(0b0000000000010010, 16)] = Key(1, 16);
		dct_coeff_VLC_code[Key(0b0000000000010001, 16)] = Key(1, 17);
		dct_coeff_VLC_code[Key(0b0000000000010000, 16)] = Key(1, 18);
		dct_coeff_VLC_code[Key(0b0000000000010100, 16)] = Key(6, 3);
		dct_coeff_VLC_code[Key(0b0000000000011010, 16)] = Key(11, 2);
		dct_coeff_VLC_code[Key(0b0000000000011001, 16)] = Key(12, 2);
		dct_coeff_VLC_code[Key(0b0000000000011000, 16)] = Key(13, 2);
		dct_coeff_VLC_code[Key(0b0000000000010111, 16)] = Key(14, 2);
		dct_coeff_VLC_code[Key(0b0000000000010110, 16)] = Key(15, 2);
		dct_coeff_VLC_code[Key(0b0000000000010101, 16)] = Key(16, 2);
		dct_coeff_VLC_code[Key(0b0000000000011111, 16)] = Key(27, 1);
		dct_coeff_VLC_code[Key(0b0000000000011110, 16)] = Key(28, 1);
		dct_coeff_VLC_code[Key(0b0000000000011101, 16)] = Key(29, 1);
		dct_coeff_VLC_code[Key(0b0000000000011100, 16)] = Key(30, 1);
		dct_coeff_VLC_code[Key(0b0000000000011011, 16)] = Key(31, 1);
	}
}

Block::~Block(){}

void Block::decoder(const int& bid,
					const int& picture_coding_type, const int& macroblock_intra,
					int *dct_zz){

	if (DEBUG){
		printf("		==BLOCK(%d)==\n", bid);
	}

	if (macroblock_intra){
		dct_zz_i = 0;
		if (bid < 4) find_dct_dc_size_luminance(dct_zz);
		else find_dct_dc_size_chrominance(dct_zz);
	}
	else {
		find_dct_coeff_first(dct_zz);
	}
	if (picture_coding_type != 4){
		while (((inBit.nextbits() >> 30) & 0b11) != 0b10){
			find_dct_coeff_next(dct_zz);
		}
		end_of_block = inBit.getBits(2);
	}

	if (DEBUG){
		printf("			EOB\n");
	}
}

void Block::find_dct_dc_size_luminance(int *dct_zz){
	int tmp = inBit.getBits(1);
	map<Key, int>::iterator iter;
	for (int i=2; i<=7; i++){
		tmp = (tmp << 1) + inBit.getBits(1);
		iter = dct_luminance_VLC_code.find(Key(tmp, i));
		if (iter != dct_luminance_VLC_code.end()){
			tmp = iter->second;
			break;
		}
	}

	if (tmp > 0){
		dct_dc_differential = inBit.getBits(tmp);
		if (dct_dc_differential & (1 << (tmp-1))) dct_zz[0] = dct_dc_differential;
		else dct_zz[0] = (-1 << tmp) | (dct_dc_differential+1);
	}
	else{
		dct_zz[0] = 0;
	}

	if (DEBUG){
		printf("			dct_dc_size_luminance: %d\n", tmp);
		if (tmp > 0) printf("			dct_dc_differential_luminance: %d\n", dct_dc_differential);
	}
}

void Block::find_dct_dc_size_chrominance(int *dct_zz){
	int tmp = inBit.getBits(1);
	map<Key, int>::iterator iter;
	for (int i=2; i<=7; i++){
		tmp = (tmp << 1) + inBit.getBits(1);
		iter = dct_chrominance_VLC_code.find(Key(tmp, i));
		if (iter != dct_chrominance_VLC_code.end()){
			tmp = iter->second;
			break;
		}
	}

	if (tmp > 0){
		dct_dc_differential = inBit.getBits(tmp);
		if (dct_dc_differential & (1 << (tmp-1))) dct_zz[0] = dct_dc_differential;
		else dct_zz[0] = (-1 << tmp) | (dct_dc_differential+1);
	}
	else{
		dct_zz[0] = 0;
	}

	if (DEBUG){
		printf("			dct_dc_size_chrominance: %d\n", tmp);
		if (tmp > 0) printf("			dct_dc_differential_chrominance: %d\n", dct_dc_differential);
	}
}

void Block::find_dct_coeff_first(int *dct_zz){
	int tmp = 0;
	int s;
	Key value(-1, 0);
	map<Key, Key>::iterator iter;
	for (int i=1; i<=16; i++){
		tmp = (tmp << 1) + inBit.getBits(1);
		iter = dct_coeff_VLC_code.find(Key(tmp, i));
		if (iter != dct_coeff_VLC_code.end()){
			value = iter->second;
			break;
		}
	}

	int run, level;
	if (value.first >= 0){
		s = inBit.getBits(1);

		run = value.first;
		level = s == 0 ? value.second: -value.second;
	}
	else if (value.first == -2){ // escape
		run = inBit.getBits(6);
		level = inBit.getBits(8);
		if (level != 0x0 && level != 0x80){ // not forbidden
			level = (level << 24) >> 24;
		}
		else{
			level = (level<<1 | inBit.getBits(8)) << 23 >> 23;
		}
	}
	else { // not found, decoded error.
		puts("dct_coeff_first decoded error.");
		return;
	}

	dct_zz_i = run;
	dct_zz[dct_zz_i] = level;

	if (DEBUG){
		printf("			dct_coeff_first: Run: %d Level: %d\n", run, level);
	}
}

void Block::find_dct_coeff_next(int *dct_zz){
	int tmp = inBit.getBits(1);
	int s;
	Key value(-1, 0);
	map<Key, Key>::iterator iter;
	for (int i=2; i<=16; i++){
		tmp = (tmp << 1) + inBit.getBits(1);
		iter = dct_coeff_VLC_code.find(Key(tmp, i));
		if (iter != dct_coeff_VLC_code.end()){
			value = iter->second;
			break;
		}
	}

	int run, level;
	if (value.first >= 0){
		s = inBit.getBits(1);

		run = value.first;
		level = s == 0 ? value.second: -value.second;
	}
	else if (value.first == -2){ // escape
		run = inBit.getBits(6);
		level = inBit.getBits(8);
		if (level != 0x0 && level != 0x80){ // not forbidden
			level = (level << 24) >> 24;
		}
		else{
			level = (level<<1 | inBit.getBits(8)) << 23 >> 23;
		}
	}

	dct_zz_i += run + 1;
	dct_zz[dct_zz_i] = level;

	if (DEBUG){
		printf("			dct_coeff_next: Run: %d Level: %d\n", run, level);
	}
}

