#include "macroblock.h"
#include "bit.h"
#include <stdlib.h>

using namespace std;

std::map<Key, int> Macroblock::macroblock_address_increment_VLC_code;
std::map<Key, int> Macroblock::macroblock_type_VLC_code[4];
std::map<Key, int> Macroblock::macroblock_motion_VLC_code;
std::map<Key, int> Macroblock::macroblock_pattern_VLC_code;

Macroblock::Macroblock(InBit& x, const bool& d): inBit(x), DEBUG(d) {
	/* macroblock_address_increment VLC code */
	// only write the table once.
	if (macroblock_address_increment_VLC_code.empty()){
		macroblock_address_increment_VLC_code[Key(0b1, 1)] = 1;
		macroblock_address_increment_VLC_code[Key(0b011, 3)] = 2;
		macroblock_address_increment_VLC_code[Key(0b010, 3)] = 3;
		macroblock_address_increment_VLC_code[Key(0b0011, 4)] = 4;
		macroblock_address_increment_VLC_code[Key(0b0010, 4)] = 5;
		macroblock_address_increment_VLC_code[Key(0b00011, 5)] = 6;
		macroblock_address_increment_VLC_code[Key(0b00010, 5)] = 7;
		macroblock_address_increment_VLC_code[Key(0b0000111, 7)] = 8;
		macroblock_address_increment_VLC_code[Key(0b0000110, 7)] = 9;
		macroblock_address_increment_VLC_code[Key(0b00001011, 8)] = 10;
		macroblock_address_increment_VLC_code[Key(0b00001010, 8)] = 11;
		macroblock_address_increment_VLC_code[Key(0b00001001, 8)] = 12;
		macroblock_address_increment_VLC_code[Key(0b00001000, 8)] = 13;
		macroblock_address_increment_VLC_code[Key(0b00000111, 8)] = 14;
		macroblock_address_increment_VLC_code[Key(0b00000110, 8)] = 15;
		macroblock_address_increment_VLC_code[Key(0b0000010111, 10)] = 16;
		macroblock_address_increment_VLC_code[Key(0b0000010110, 10)] = 17;
		macroblock_address_increment_VLC_code[Key(0b0000010101, 10)] = 18;
		macroblock_address_increment_VLC_code[Key(0b0000010100, 10)] = 19;
		macroblock_address_increment_VLC_code[Key(0b0000010011, 10)] = 20;
		macroblock_address_increment_VLC_code[Key(0b0000010010, 10)] = 21;
		macroblock_address_increment_VLC_code[Key(0b00000100011, 11)] = 22;
		macroblock_address_increment_VLC_code[Key(0b00000100010, 11)] = 23;
		macroblock_address_increment_VLC_code[Key(0b00000100001, 11)] = 24;
		macroblock_address_increment_VLC_code[Key(0b00000100000, 11)] = 25;
		macroblock_address_increment_VLC_code[Key(0b00000011111, 11)] = 26;
		macroblock_address_increment_VLC_code[Key(0b00000011110, 11)] = 27;
		macroblock_address_increment_VLC_code[Key(0b00000011101, 11)] = 28;
		macroblock_address_increment_VLC_code[Key(0b00000011100, 11)] = 29;
		macroblock_address_increment_VLC_code[Key(0b00000011011, 11)] = 30;
		macroblock_address_increment_VLC_code[Key(0b00000011010, 11)] = 31;
		macroblock_address_increment_VLC_code[Key(0b00000011001, 11)] = 32;
		macroblock_address_increment_VLC_code[Key(0b00000011000, 11)] = 33;
		macroblock_address_increment_VLC_code[Key(0b00000001111, 11)] = -1;
		macroblock_address_increment_VLC_code[Key(0b00000001000, 11)] = -1;
	}
	
	/* macroblock_type VLC code */
	if (macroblock_type_VLC_code[0].empty()){
		macroblock_type_VLC_code[0][Key(0b1, 1)] = 0b00001;
		macroblock_type_VLC_code[0][Key(0b01, 2)] = 0b10001;
	}
	if (macroblock_type_VLC_code[1].empty()){
		macroblock_type_VLC_code[1][Key(0b1, 1)] = 0b01010;
		macroblock_type_VLC_code[1][Key(0b01, 2)] = 0b00010;
		macroblock_type_VLC_code[1][Key(0b001, 3)] = 0b01000;
		macroblock_type_VLC_code[1][Key(0b00011, 5)] = 0b00001;
		macroblock_type_VLC_code[1][Key(0b00010, 5)] = 0b11010;
		macroblock_type_VLC_code[1][Key(0b00001, 5)] = 0b10010;
		macroblock_type_VLC_code[1][Key(0b000001, 6)] = 0b10001;
	}
	if (macroblock_type_VLC_code[2].empty()){
		macroblock_type_VLC_code[2][Key(0b10, 2)] = 0b01100;
		macroblock_type_VLC_code[2][Key(0b11, 2)] = 0b01110;
		macroblock_type_VLC_code[2][Key(0b010, 3)] = 0b00100;
		macroblock_type_VLC_code[2][Key(0b011, 3)] = 0b00110;
		macroblock_type_VLC_code[2][Key(0b0010, 4)] = 0b01000;
		macroblock_type_VLC_code[2][Key(0b0011, 4)] = 0b01010;
		macroblock_type_VLC_code[2][Key(0b00011, 5)] = 0b00001;
		macroblock_type_VLC_code[2][Key(0b00010, 5)] = 0b11110;
		macroblock_type_VLC_code[2][Key(0b000011, 6)] = 0b11010;
		macroblock_type_VLC_code[2][Key(0b000010, 6)] = 0b10110;
		macroblock_type_VLC_code[2][Key(0b000001, 6)] = 0b10001;
	}
	if (macroblock_type_VLC_code[3].empty()){
		macroblock_type_VLC_code[3][Key(0b1, 1)] = 0b00001;
	}

	/* macroblock_type VLC code */
	if (macroblock_motion_VLC_code.empty()){
		macroblock_motion_VLC_code[Key(0b00000011001, 11)] = -16;
		macroblock_motion_VLC_code[Key(0b00000011011, 11)] = -15;
		macroblock_motion_VLC_code[Key(0b00000011101, 11)] = -14;
		macroblock_motion_VLC_code[Key(0b00000011111, 11)] = -13;
		macroblock_motion_VLC_code[Key(0b00000100001, 11)] = -12;
		macroblock_motion_VLC_code[Key(0b00000100011, 11)] = -11;
		macroblock_motion_VLC_code[Key(0b0000010011, 10)] = -10;
		macroblock_motion_VLC_code[Key(0b0000010101, 10)] = -9;
		macroblock_motion_VLC_code[Key(0b0000010111, 10)] = -8;
		macroblock_motion_VLC_code[Key(0b00000111, 8)] = -7;
		macroblock_motion_VLC_code[Key(0b00001001, 8)] = -6;
		macroblock_motion_VLC_code[Key(0b00001011, 8)] = -5;
		macroblock_motion_VLC_code[Key(0b0000111, 7)] = -4;
		macroblock_motion_VLC_code[Key(0b00011, 5)] = -3;
		macroblock_motion_VLC_code[Key(0b0011, 4)] = -2;
		macroblock_motion_VLC_code[Key(0b011, 3)] = -1;
		macroblock_motion_VLC_code[Key(0b1, 1)] = 0;
		macroblock_motion_VLC_code[Key(0b010, 3)] = 1;
		macroblock_motion_VLC_code[Key(0b0010, 4)] = 2;
		macroblock_motion_VLC_code[Key(0b00010, 5)] = 3;
		macroblock_motion_VLC_code[Key(0b0000110, 7)] = 4;
		macroblock_motion_VLC_code[Key(0b00001010, 8)] = 5;
		macroblock_motion_VLC_code[Key(0b00001000, 8)] = 6;
		macroblock_motion_VLC_code[Key(0b00000110, 8)] = 7;
		macroblock_motion_VLC_code[Key(0b0000010110, 10)] = 8;
		macroblock_motion_VLC_code[Key(0b0000010100, 10)] = 9;
		macroblock_motion_VLC_code[Key(0b0000010010, 10)] = 10;
		macroblock_motion_VLC_code[Key(0b00000100010, 11)] = 11;
		macroblock_motion_VLC_code[Key(0b00000100000, 11)] = 12;
		macroblock_motion_VLC_code[Key(0b00000011110, 11)] = 13;
		macroblock_motion_VLC_code[Key(0b00000011100, 11)] = 14;
		macroblock_motion_VLC_code[Key(0b00000011010, 11)] = 15;
		macroblock_motion_VLC_code[Key(0b00000011000, 11)] = 16;
	}

	/* macroblock_pattern VLC code */
	if (macroblock_pattern_VLC_code.empty()){
		macroblock_pattern_VLC_code[Key(0b111, 3)] = 60;
		macroblock_pattern_VLC_code[Key(0b1101, 4)] = 4;
		macroblock_pattern_VLC_code[Key(0b1100, 4)] = 8;
		macroblock_pattern_VLC_code[Key(0b1011, 4)] = 16;
		macroblock_pattern_VLC_code[Key(0b1010, 4)] = 32;
		macroblock_pattern_VLC_code[Key(0b10011, 5)] = 12;
		macroblock_pattern_VLC_code[Key(0b10010, 5)] = 48;
		macroblock_pattern_VLC_code[Key(0b10001, 5)] = 20;
		macroblock_pattern_VLC_code[Key(0b10000, 5)] = 40;
		macroblock_pattern_VLC_code[Key(0b01111, 5)] = 28;
		macroblock_pattern_VLC_code[Key(0b01110, 5)] = 44;
		macroblock_pattern_VLC_code[Key(0b01101, 5)] = 52;
		macroblock_pattern_VLC_code[Key(0b01100, 5)] = 56;
		macroblock_pattern_VLC_code[Key(0b01011, 5)] = 1;
		macroblock_pattern_VLC_code[Key(0b01010, 5)] = 61;
		macroblock_pattern_VLC_code[Key(0b01001, 5)] = 2;
		macroblock_pattern_VLC_code[Key(0b01000, 5)] = 62;
		macroblock_pattern_VLC_code[Key(0b001111, 6)] = 24;
		macroblock_pattern_VLC_code[Key(0b001110, 6)] = 36;
		macroblock_pattern_VLC_code[Key(0b001101, 6)] = 3;
		macroblock_pattern_VLC_code[Key(0b001100, 6)] = 63;
		macroblock_pattern_VLC_code[Key(0b0010111, 7)] = 5;
		macroblock_pattern_VLC_code[Key(0b0010110, 7)] = 9;
		macroblock_pattern_VLC_code[Key(0b0010101, 7)] = 17;
		macroblock_pattern_VLC_code[Key(0b0010100, 7)] = 33;
		macroblock_pattern_VLC_code[Key(0b0010011, 7)] = 6;
		macroblock_pattern_VLC_code[Key(0b0010010, 7)] = 10;
		macroblock_pattern_VLC_code[Key(0b0010001, 7)] = 18;
		macroblock_pattern_VLC_code[Key(0b0010000, 7)] = 34;
		macroblock_pattern_VLC_code[Key(0b00011111, 8)] = 7;
		macroblock_pattern_VLC_code[Key(0b00011110, 8)] = 11;
		macroblock_pattern_VLC_code[Key(0b00011101, 8)] = 19;
		macroblock_pattern_VLC_code[Key(0b00011100, 8)] = 35;
		macroblock_pattern_VLC_code[Key(0b00011011, 8)] = 13;
		macroblock_pattern_VLC_code[Key(0b00011010, 8)] = 49;
		// !!! Not finished !!!
	}
}

Macroblock::~Macroblock() {}

void Macroblock::decoder(int p_c_t, int f_f, int f_r_s, int b_f, int b_r_s){
	/*  */
	picture_coding_type = p_c_t;
	forward_f = f_f;
	forward_r_size = f_r_s;
	backward_f = b_f;
	backward_r_size = b_r_s;

	while ((inBit.nextbits() >> 21) == 0b1111){
		macroblock_stuffing = inBit.getBits(11);
	}
	while ((inBit.nextbits() >> 21) == 0b1000){
		macroblock_escape = inBit.getBits(11);
	}
	macroblock_address_increment = find_macroblock_address_increment();
	macroblock_type = find_macroblock_type();
	macroblock_quant = (macroblock_type >> 4) & 0x1;
	macroblock_motion_forward = (macroblock_type >> 3) & 0x1;
	macroblock_motion_backward = (macroblock_type >> 2) & 0x1;
	macroblock_pattern = (macroblock_type >> 1) & 0x1;
	macroblock_intra = (macroblock_type) & 0x1;
	if (macroblock_quant)
		quantizer_scale = inBit.getBits(5);
	if (macroblock_motion_forward){
		motion_horizontal_forward_code = find_macroblock_motion_vector();
		if (forward_f != 1 && motion_horizontal_forward_code != 0)
			motion_horizontal_forward_r = inBit.getBits(forward_r_size);
		motion_vertical_forward_code = find_macroblock_motion_vector();
		if (forward_f != 1 && motion_vertical_forward_code != 0)
			motion_vertical_forward_r = inBit.getBits(forward_r_size);
	}
	if (macroblock_motion_backward){
		motion_horizontal_backward_code = find_macroblock_motion_vector();
		if (backward_f != 1 && motion_horizontal_backward_code != 0)
			motion_horizontal_backward_r = inBit.getBits(backward_r_size);
		motion_vertical_backward_code = find_macroblock_motion_vector();
		if (backward_f != 1 && motion_vertical_backward_code != 0)
			motion_vertical_backward_r = inBit.getBits(backward_r_size);
	}
	if (macroblock_pattern) {
		//coded_block_pattern;
	}

	if (DEBUG){
		puts("\n--- macroblock ---");
		printf("macroblock_stuffing %d\n", macroblock_stuffing);
		printf("macroblock_escape %d\n", macroblock_escape);
		printf("macroblock_address_increment %d\n", macroblock_address_increment);
		printf("macroblock_type %d\n", macroblock_type);
		printf("macroblock_motion_forward %d\n", macroblock_motion_forward);
		printf("macroblock_motion_backward %d\n", macroblock_motion_backward);
		printf("macroblock_pattern %d\n", macroblock_pattern);
		printf("macroblock_intra %d\n", macroblock_intra);
		if (macroblock_motion_forward){
			printf("motion_horizontal_forward_code %d\n", motion_horizontal_forward_code);
			printf("motion_horizontal_forward_r %d\n", motion_horizontal_forward_r);
			printf("motion_vertical_forward_code %d\n", motion_vertical_forward_code);
			printf("motion_vertical_forward_r %d\n", motion_vertical_backward_r);
		}
		if (macroblock_motion_backward){
			printf("motion_horizontal_backward_code %d\n", motion_horizontal_backward_code);
			printf("motion_horizontal_backward_r %d\n", motion_horizontal_backward_r);
			printf("motion_vertical_backward_code %d\n", motion_vertical_backward_code);
			printf("motion_vertical_backward_r %d\n", motion_vertical_backward_r);
		}
		if (macroblock_pattern){
			printf("coded_block_pattern %d\n", coded_block_pattern);
		}
	}
	exit(0);
}

int Macroblock::find_macroblock_address_increment(){
	int tmp = 0;
	map<Key, int>::iterator iter;
	for (int i=1; i<=11; i++){
		tmp = (tmp << 1) + inBit.getBits(1);
		iter = macroblock_address_increment_VLC_code.find(Key(tmp, i));
		if (iter != macroblock_address_increment_VLC_code.end()){
			// prevent last 2 item in VLC code
			if (iter->second <= 0){
				if (tmp == 0b1111) tmp = macroblock_stuffing;
				else if (tmp == 0b1000) tmp = macroblock_escape;
			}
			else{
				tmp = iter->second;
			}
			return tmp;
		}
	}
	return 0;
}

int Macroblock::find_macroblock_type(){
	int tid = picture_coding_type - 1;
	int tmp = 0;
	map<Key, int>::iterator iter;
	for (int i=1; i<=6; i++){
		tmp = (tmp << 1) + inBit.getBits(1);
		iter = macroblock_type_VLC_code[tid].find(Key(tmp, i));
		if (iter != macroblock_type_VLC_code[tid].end()){
			tmp = iter->second;
			return tmp;
		}
	}
	return 0;
}

int Macroblock::find_macroblock_motion_vector(){
	int tmp = 0;
	map<Key, int>::iterator iter;
	for (int i=0; i<=11; i++){
		tmp = (tmp << 1) + inBit.getBits(1);
		iter = macroblock_motion_VLC_code.find(Key(tmp, i));
		if (iter != macroblock_motion_VLC_code.end()){
			tmp = iter->second;
			return tmp;
		}
	}
	return 0;
}
