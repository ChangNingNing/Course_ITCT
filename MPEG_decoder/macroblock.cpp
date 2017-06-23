#include "macroblock.h"
#include "bit.h"
#include "block.h"
#include <string.h>

#include <cmath>
using namespace std;

std::map<Key, int> Macroblock::macroblock_address_increment_VLC_code;
std::map<Key, int> Macroblock::macroblock_type_VLC_code[4];
std::map<Key, int> Macroblock::macroblock_motion_VLC_code;
std::map<Key, int> Macroblock::macroblock_pattern_VLC_code;

void IDCT(int block[8][8]){
 	static const float C[8] = {
	0.500000, 0.490393, 0.461940, 0.415735, 0.353553, 0.277785, 0.191342, 0.097545};

    float tmp[8][8], t[6], a[4], b[4], x[4];
    for(int i = 0; i < 8; i++) {
        t[0] = C[4] * block[0][i];
        t[1] = C[2] * block[2][i];
        t[2] = C[6] * block[2][i];
        t[3] = C[4] * block[4][i];
        t[4] = C[6] * block[6][i];
        t[5] = C[2] * block[6][i];
        a[0] = t[0] + t[1] + t[3] + t[4];
        a[1] = t[0] + t[2] - t[3] - t[5];
        a[2] = t[0] - t[2] - t[3] + t[5];
        a[3] = t[0] - t[1] + t[3] - t[4];
        x[0] = (float)block[1][i];
        x[1] = (float)block[3][i];
        x[2] = (float)block[5][i];
        x[3] = (float)block[7][i];
        b[0] = x[0]*C[1] + x[1]*C[3] + x[2]*C[5] + x[3]*C[7];
        b[1] = x[0]*C[3] - x[1]*C[7] - x[2]*C[1] - x[3]*C[5];
        b[2] = x[0]*C[5] - x[1]*C[1] + x[2]*C[7] + x[3]*C[3];
        b[3] = x[0]*C[7] - x[1]*C[5] + x[2]*C[3] - x[3]*C[1];
        tmp[0][i] = a[0] + b[0];
        tmp[7][i] = a[0] - b[0];
        tmp[1][i] = a[1] + b[1];
        tmp[6][i] = a[1] - b[1];
        tmp[2][i] = a[2] + b[2];
        tmp[5][i] = a[2] - b[2];
        tmp[3][i] = a[3] + b[3];
        tmp[4][i] = a[3] - b[3];
    }
    for(int i = 0; i < 8; i++) {
       	t[0] = tmp[i][0] * C[4];
        t[1] = tmp[i][2] * C[2];
        t[2] = tmp[i][2] * C[6];
        t[3] = tmp[i][4] * C[4];
        t[4] = tmp[i][6] * C[6];
        t[5] = tmp[i][6] * C[2];
        a[0] = t[0] + t[1] + t[3] + t[4];
        a[1] = t[0] + t[2] - t[3] - t[5];
        a[2] = t[0] - t[2] - t[3] + t[5];
        a[3] = t[0] - t[1] + t[3] - t[4];
        x[0] = tmp[i][1];
        x[1] = tmp[i][3];
        x[2] = tmp[i][5];
        x[3] = tmp[i][7];
        b[0] = x[0]*C[1] + x[1]*C[3] + x[2]*C[5] + x[3]*C[7];
        b[1] = x[0]*C[3] - x[1]*C[7] - x[2]*C[1] - x[3]*C[5];
        b[2] = x[0]*C[5] - x[1]*C[1] + x[2]*C[7] + x[3]*C[3];
        b[3] = x[0]*C[7] - x[1]*C[5] + x[2]*C[3] - x[3]*C[1];
        block[i][0] = 0.5 + a[0] + b[0];
        block[i][7] = 0.5 + a[0] - b[0];
        block[i][1] = 0.5 + a[1] + b[1];
        block[i][6] = 0.5 + a[1] - b[1];
        block[i][2] = 0.5 + a[2] + b[2];
        block[i][5] = 0.5 + a[2] - b[2];
        block[i][3] = 0.5 + a[3] + b[3];
        block[i][4] = 0.5 + a[3] - b[3];
    }
}

Macroblock::Macroblock(InBit& x, const bool& d, Block& b, Image& i): inBit(x), DEBUG(d), block(b), image(i){
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
		macroblock_address_increment_VLC_code[Key(0b00000001111, 11)] = 0;
		macroblock_address_increment_VLC_code[Key(0b00000001000, 11)] = 0;
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

	/* macroblock_motion_vectors VLC code */
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
		macroblock_pattern_VLC_code[Key(0b00011001, 8)] = 21;
		macroblock_pattern_VLC_code[Key(0b00011000, 8)] = 41;

		macroblock_pattern_VLC_code[Key(0b00010111, 8)] = 14;
		macroblock_pattern_VLC_code[Key(0b00010110, 8)] = 50;
		macroblock_pattern_VLC_code[Key(0b00010101, 8)] = 22;
		macroblock_pattern_VLC_code[Key(0b00010100, 8)] = 42;
		macroblock_pattern_VLC_code[Key(0b00010011, 8)] = 15;

		macroblock_pattern_VLC_code[Key(0b00010010, 8)] = 51;
		macroblock_pattern_VLC_code[Key(0b00010001, 8)] = 23;
		macroblock_pattern_VLC_code[Key(0b00010000, 8)] = 43;
		macroblock_pattern_VLC_code[Key(0b00001111, 8)] = 25;
		macroblock_pattern_VLC_code[Key(0b00001110, 8)] = 37;

		macroblock_pattern_VLC_code[Key(0b00001101, 8)] = 26;
		macroblock_pattern_VLC_code[Key(0b00001100, 8)] = 38;
		macroblock_pattern_VLC_code[Key(0b00001011, 8)] = 29;
		macroblock_pattern_VLC_code[Key(0b00001010, 8)] = 45;
		macroblock_pattern_VLC_code[Key(0b00001001, 8)] = 53;

		macroblock_pattern_VLC_code[Key(0b00001000, 8)] = 57;
		macroblock_pattern_VLC_code[Key(0b00000111, 8)] = 30;
		macroblock_pattern_VLC_code[Key(0b00000110, 8)] = 46;
		macroblock_pattern_VLC_code[Key(0b00000101, 8)] = 54;
		macroblock_pattern_VLC_code[Key(0b00000100, 8)] = 58;

		macroblock_pattern_VLC_code[Key(0b000000111, 9)] = 31;
		macroblock_pattern_VLC_code[Key(0b000000110, 9)] = 47;
		macroblock_pattern_VLC_code[Key(0b000000101, 9)] = 55;
		macroblock_pattern_VLC_code[Key(0b000000100, 9)] = 59;
		macroblock_pattern_VLC_code[Key(0b000000011, 9)] = 27;

		macroblock_pattern_VLC_code[Key(0b000000010, 9)] = 39;
	}
}

Macroblock::~Macroblock() {}

void Macroblock::decoder(	const int& picture_coding_type,
							const int& full_pel_forward_vector, const int& full_pel_backward_vector,
							const int& forward_f, const int& forward_r_size,
							const int& backward_f, const int& backward_r_size,
							int& recon_right_for_prev, int& recon_down_for_prev,
							int& recon_right_back_prev, int& recon_down_back_prev,
							int& quantizer_scale,
							int& dct_dc_y_past, int& dct_dc_cb_past, int& dct_dc_cr_past,
							int& past_intra_address, int& previous_macroblock_address,
							const int& mb_width,
							const int* intra_quant, const int* non_intra_quant,
							const int& forward_addr, const int& backward_addr, const int& cur_addr){

	while ((inBit.nextbits() >> 21) == 0b00000001111){
		macroblock_stuffing = inBit.getBits(11);
	}
	int nEscape = 0;
	while ((inBit.nextbits() >> 21) == 0b00000001000){
		macroblock_escape = inBit.getBits(11);
		nEscape++;
	}
	macroblock_address_increment = find_macroblock_address_increment();
	macroblock_address_increment += nEscape * 33;

	// skipped macroblock
	if (macroblock_address_increment > 1){
		for (int i=1; i<macroblock_address_increment; i++){
			macroblock_address = previous_macroblock_address + i;
			mb_row = macroblock_address / mb_width;
			mb_column = macroblock_address % mb_width;
			if (picture_coding_type == 2){
				recon_right_for = 0;
				recon_down_for = 0;

				dct_recon_skipped_p(cur_addr, forward_addr, mb_row, mb_column);

				recon_right_for_prev = 0;
				recon_down_for_prev = 0;
			}
			else if (picture_coding_type == 3){
				dct_recon_skipped_b(forward_addr, backward_addr, cur_addr,
									mb_row, mb_column, non_intra_quant, quantizer_scale);
			}
		}
	}

	macroblock_address = previous_macroblock_address + macroblock_address_increment;
	previous_macroblock_address = macroblock_address;
	mb_row = macroblock_address / mb_width;
	mb_column = macroblock_address % mb_width;

	macroblock_type = find_macroblock_type(picture_coding_type);
	macroblock_quant = (macroblock_type >> 4) & 0x1;
	macroblock_motion_forward = (macroblock_type >> 3) & 0x1;
	macroblock_motion_backward = (macroblock_type >> 2) & 0x1;
	macroblock_pattern = (macroblock_type >> 1) & 0x1;
	macroblock_intra = (macroblock_type) & 0x1;

	if (macroblock_quant)
		quantizer_scale = inBit.getBits(5);

	if (macroblock_motion_forward){
		motion_horizontal_forward_code = find_macroblock_motion_vector();
		if (forward_f != 1 && motion_horizontal_forward_code != 0){
			motion_horizontal_forward_r = inBit.getBits(forward_r_size);
		}

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

	coded_block_pattern = 0;
	if (macroblock_pattern) {
		coded_block_pattern = find_macroblock_pattern();
	}
	for (int i=0; i<6; i++){
		pattern_code[i] = 0;
		if (coded_block_pattern & (1<<(5-i))) pattern_code[i] = 1;
		if (macroblock_intra) pattern_code[i] = 1;
	}

	/* Reconstruct the motion vector horizontal and vertical components */
	if (macroblock_motion_forward){
		recon_motion_vector_forward(forward_f, full_pel_forward_vector,
									recon_right_for_prev, recon_down_for_prev);
	}
	else{
		if (picture_coding_type == 2){
			recon_right_for = 0;
			recon_down_for = 0;
			recon_right_for_prev = 0;
			recon_down_for_prev = 0;
		}
		else if (picture_coding_type == 3){
			recon_right_for = recon_right_for_prev;
			recon_down_for = recon_down_for_prev;
		}
	}
	if (macroblock_motion_backward){
		recon_motion_vector_backward(backward_f, full_pel_backward_vector,
									recon_right_back_prev, recon_down_back_prev);
	}
	else {
		if(picture_coding_type == 3){
			recon_right_back = recon_right_back_prev;
			recon_down_back = recon_down_back_prev;
		}
	}

	if (DEBUG){
		printf("	==MACROBLOCK()==\n");
		printf("		macroblock_address_increment: %d\n", macroblock_address_increment);
		printf("		macroblock_type: %d\n", macroblock_type);
		printf("			macroblock_quant: %d\n", macroblock_quant);
		printf("			macroblock_motion_forward: %d\n", macroblock_motion_forward);
		printf("			macroblock_motion_backward: %d\n", macroblock_motion_backward);
		printf("			macroblock_pattern: %d\n", macroblock_pattern);
		printf("			macroblock_intra: %d\n", macroblock_intra);
		if (macroblock_quant){
			printf("		quantizer_scale: %d\n", quantizer_scale);
		}
		if (macroblock_motion_forward){
			printf("		motion_horizontal_forward_code: %d\n", motion_horizontal_forward_code);
			printf("		motion_vertical_forward_code: %d\n", motion_vertical_forward_code);
		}
		if (macroblock_motion_backward){
			printf("		motion_horizontal_backward_code: %d\n", motion_horizontal_backward_code);
			printf("		motion_vertical_backward_code: %d\n", motion_vertical_backward_code);
		}
		if (macroblock_motion_forward)
			printf("		ForMV(%d,%d)\n", recon_right_for, recon_down_for);
		if (macroblock_motion_backward)
			printf("		BackMV(%d,%d)\n", recon_right_back, recon_down_back);
		if (macroblock_pattern)
			printf("		coded_block_pattern: %d\n", coded_block_pattern);
	}

	for (int i=0; i<6; i++){
		memset(dct_zz, 0, sizeof(int)*64);
		if (pattern_code[i])
			block.decoder(i, picture_coding_type, macroblock_intra, dct_zz);

		// Intra frame
		if (macroblock_intra){
			dct_recon_intra(i, intra_quant, past_intra_address,
							quantizer_scale, dct_dc_y_past, dct_dc_cb_past, dct_dc_cr_past);
			recon_right_back_prev = 0;
			recon_down_back_prev = 0;
			recon_right_for_prev = 0;
			recon_down_for_prev = 0;
		}
		// P frame
		else if (picture_coding_type == 2){
			dct_recon_p(forward_addr, i, mb_row, mb_column, non_intra_quant, quantizer_scale);
		}
		// B frame
		else if (picture_coding_type == 3){
			dct_recon_b(forward_addr, backward_addr, i, mb_row, mb_column, non_intra_quant, quantizer_scale);
		}
		image.inputYUV(cur_addr, i, mb_row, mb_column, dct_recon);
	}

	if (macroblock_intra){
		past_intra_address = macroblock_address;
	}

	if (picture_coding_type == 4)
		end_of_macroblock = inBit.getBits(1);
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

int Macroblock::find_macroblock_type(const int& picture_coding_type){
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
	for (int i=1; i<=11; i++){
		tmp = (tmp << 1) + inBit.getBits(1);
		iter = macroblock_motion_VLC_code.find(Key(tmp, i));
		if (iter != macroblock_motion_VLC_code.end()){
			tmp = iter->second;
			return tmp;
		}
	}
	return 0;
}


int Macroblock::find_macroblock_pattern(){
	int tmp = inBit.getBits(2);
	map<Key, int>::iterator iter;
	for (int i=3; i<=9; i++){
		tmp = (tmp << 1) + inBit.getBits(1);
		iter = macroblock_pattern_VLC_code.find(Key(tmp, i));
		if (iter != macroblock_pattern_VLC_code.end()){
			tmp = iter->second;
			return tmp;
		}
	}
	return 0;
}

#define Sign(x) (((x)>0)? 1: ((x)<0)? -1: 0)
#define CeilDiv(a, b) ( ((a)-1)/(b) + 1)

void Macroblock::dct_recon_intra(	const int& bid, const int* intra_quant, const int& past_intra_addr,
									int& quantizer_scale, int& y_past, int& cb_past, int& cr_past){
	/* I. Reconstruct dct */
	for (int m=0; m<8; m++){
		for (int n=0; n<8; n++){
			int i = scan[m][n];
			dct_recon[m][n] = (dct_zz[i] * quantizer_scale * intra_quant[m*8+n]) / 8;
			if ((dct_recon[m][n] & 1) == 0)
				dct_recon[m][n] = dct_recon[m][n] - Sign(dct_recon[m][n]);
			if (dct_recon[m][n] > 2047) dct_recon[m][n] = 2047;
			else if(dct_recon[m][n] < -2048) dct_recon[m][n] = -2048;
		}
	} 
	// luminance block
	int diff_tmp = macroblock_address - past_intra_addr;
	dct_recon[0][0] = dct_zz[0] * 8;
	if (bid < 4){
		if (bid == 0){
			if (diff_tmp > 1)
				dct_recon[0][0] += 1024;
			else
				dct_recon[0][0] += y_past;
		}
		else{
			dct_recon[0][0] += y_past;
		}
		y_past = dct_recon[0][0];
	}
	// chrominance Cb
	else if (bid == 4){
		if (diff_tmp > 1)
			dct_recon[0][0] += 1024;
		else
			dct_recon[0][0] += cb_past;
		cb_past = dct_recon[0][0];
	}
	// chrominance Cr
	else {
		if (diff_tmp > 1)
			dct_recon[0][0] += 1024;
		else
			dct_recon[0][0] += cr_past;
		cr_past = dct_recon[0][0];
	}

	/* II. IDCT */
	IDCT(dct_recon);
}

void Macroblock::dct_recon_p(const int& forward_addr, const int& bid, const int& mb_row, const int& mb_col,
								const int* non_intra_quant, const int& quantizer_scale){
	/* I. Find motion vector */ 
	int right_for;
	int down_for;
	int right_half_for;
	int down_half_for;

	// luminance
	if (bid < 4){
		right_for = recon_right_for >> 1;
		down_for = recon_down_for >> 1;
		right_half_for = recon_right_for - 2 * right_for;
		down_half_for = recon_down_for - 2 * down_for;
	}
	// Cb, Cr
	else {
		right_for = (recon_right_for / 2) >> 1;
		down_for = (recon_down_for / 2) >> 1;
		right_half_for = recon_right_for / 2 - 2 * right_for;
		down_half_for = recon_down_for / 2 - 2 * down_for;
	}

	/* II. Find pel block */
	int pel[8][8] = {0}, row, col, tid;
	// luminance
	if (bid < 4){
		row = (mb_row << 4) + ((bid >> 1) << 3);
		col = (mb_col << 4) + ((bid &  1) << 3);
		tid = 0;
	}
	// Cb, Cr
	else {
		row = (mb_row << 3);
		col = (mb_col << 3);
		tid = bid - 3;
	}
	if (!right_half_for && !down_half_for){
		for (int i=0; i<8; i++) for (int j=0; j<8; j++){
				pel[i][j] = image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for];
		}
	}
	else if (!right_half_for && down_half_for){
		for (int i=0; i<8; i++) for (int j=0; j<8; j++){
				pel[i][j] = image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for] + 
							image.image_buf[forward_addr][tid][row+i + down_for+1][col+j + right_for];
				pel[i][j] = CeilDiv(pel[i][j], 2);
		}
	}
	else if (right_half_for && !down_half_for){
		for (int i=0; i<8; i++) for (int j=0; j<8; j++){
				pel[i][j] = image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for] + 
							image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for+1];
				pel[i][j] = CeilDiv(pel[i][j], 2);
		}
	}
	else {
		for (int i=0; i<8; i++) for (int j=0; j<8; j++){
				pel[i][j] = image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for] + 
							image.image_buf[forward_addr][tid][row+i + down_for+1][col+j + right_for] +
							image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for+1]+
							image.image_buf[forward_addr][tid][row+i + down_for+1][col+j + right_for+1];
				pel[i][j] = CeilDiv(pel[i][j], 4);
		}
	}

	/* III. Reconstruct dct */
	for (int m=0; m<8; m++){
		for (int n=0; n<8; n++){
			int i = scan[m][n];
			dct_recon[m][n] = (2*dct_zz[i]+Sign(dct_zz[i]))*quantizer_scale*non_intra_quant[m*8+n]/16;
			if ((dct_recon[m][n] & 1) == 0)
				dct_recon[m][n] -= Sign(dct_recon[m][n]);
			if (dct_recon[m][n] > 2047) dct_recon[m][n] = 2047;
			if (dct_recon[m][n] < -2048) dct_recon[m][n] = -2048;
			if (dct_zz[i] == 0) dct_recon[m][n] = 0;
		}
	}

	/* IV. IDCT */
	IDCT(dct_recon);

	/* V. Add pel matrix */
	for (int m=0; m<8; m++)
		for (int n=0; n<8; n++)
			dct_recon[m][n] += pel[m][n];
}

void Macroblock::dct_recon_b(const int& forward_addr, const int& backward_addr, const int& bid,
								const int& mb_row, const int& mb_col,
								const int* non_intra_quant, const int& quantizer_scale){
	/* I. Find motion vector */ 
	int right_for, down_for, right_half_for, down_half_for;
	int right_back, down_back, right_half_back, down_half_back;

	// luminance
	if (bid < 4){
		right_for = recon_right_for >> 1;
		down_for = recon_down_for >> 1;
		right_half_for = recon_right_for - 2 * right_for;
		down_half_for = recon_down_for - 2 * down_for;

		right_back = recon_right_back >> 1;
		down_back = recon_down_back >> 1;
		right_half_back = recon_right_back - 2 * right_back;
		down_half_back = recon_down_back - 2 * down_back;
	}
	// Cb, Cr
	else {
		right_for = (recon_right_for / 2) >> 1;
		down_for = (recon_down_for / 2) >> 1;
		right_half_for = recon_right_for / 2 - 2 * right_for;
		down_half_for = recon_down_for / 2 - 2 * down_for;

		right_back = (recon_right_back / 2) >> 1;
		down_back = (recon_down_back / 2) >> 1;
		right_half_back = recon_right_back / 2 - 2 * right_back;
		down_half_back = recon_down_back / 2 - 2 * down_back;
	}

	/* II. Find pel block */
	int pel[8][8] = {0}, row, col, tid;
	// luminance
	if (bid < 4){
		row = (mb_row << 4) + ((bid >> 1) << 3);
		col = (mb_col << 4) + ((bid &  1) << 3);
		tid = 0;
	}
	// Cb, Cr
	else {
		row = (mb_row << 3);
		col = (mb_col << 3);
		tid = bid - 3;
	}
	int num = 0;
	if (macroblock_motion_forward){
		num++;
		if (!right_half_for && !down_half_for){
			for (int i=0; i<8; i++) for (int j=0; j<8; j++){
					pel[i][j] = image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for];
			}
		}
		else if (!right_half_for && down_half_for){
			for (int i=0; i<8; i++) for (int j=0; j<8; j++){
					pel[i][j] = image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for] + 
								image.image_buf[forward_addr][tid][row+i + down_for+1][col+j + right_for];
					pel[i][j] = CeilDiv(pel[i][j], 2);
			}
		}
		else if (right_half_for && !down_half_for){
			for (int i=0; i<8; i++) for (int j=0; j<8; j++){
					pel[i][j] = image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for] + 
								image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for+1];
					pel[i][j] = CeilDiv(pel[i][j], 2);
			}
		}
		else {
			for (int i=0; i<8; i++) for (int j=0; j<8; j++){
					pel[i][j] = image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for] + 
								image.image_buf[forward_addr][tid][row+i + down_for+1][col+j + right_for] +
								image.image_buf[forward_addr][tid][row+i + down_for+0][col+j + right_for+1]+
								image.image_buf[forward_addr][tid][row+i + down_for+1][col+j + right_for+1];
					pel[i][j] = CeilDiv(pel[i][j], 4);
			}
		}
	}
	if (macroblock_motion_backward){
		num++;
		int tmp;
		if (!right_half_back && !down_half_back){
			for (int i=0; i<8; i++) for (int j=0; j<8; j++){
					tmp = 	image.image_buf[backward_addr][tid][row+i + down_back+0][col+j + right_back];
					pel[i][j] = CeilDiv(tmp + pel[i][j], num);
			}
		}
		else if (!right_half_back && down_half_back){
			for (int i=0; i<8; i++) for (int j=0; j<8; j++){
					tmp = 	image.image_buf[backward_addr][tid][row+i + down_back+0][col+j + right_back] + 
							image.image_buf[backward_addr][tid][row+i + down_back+1][col+j + right_back];
					tmp = CeilDiv(tmp, 2);
					pel[i][j] = CeilDiv(tmp + pel[i][j], num);
			}
		}
		else if (right_half_back && !down_half_back){
			for (int i=0; i<8; i++) for (int j=0; j<8; j++){
					tmp =	image.image_buf[backward_addr][tid][row+i + down_back+0][col+j + right_back] + 
							image.image_buf[backward_addr][tid][row+i + down_back+0][col+j + right_back+1];
					tmp = CeilDiv(tmp, 2);
					pel[i][j] = CeilDiv(tmp + pel[i][j], num);
			}
		}
		else {
			for (int i=0; i<8; i++) for (int j=0; j<8; j++){
					tmp =	image.image_buf[backward_addr][tid][row+i + down_back][col+j + right_back] + 
							image.image_buf[backward_addr][tid][row+i + down_back+1][col+j +right_back] +
							image.image_buf[backward_addr][tid][row+i + down_back][col+j + right_back+1]+
							image.image_buf[backward_addr][tid][row+i + down_back+1][col+j + right_back+1];
					tmp = CeilDiv(tmp, 4);
					pel[i][j] = CeilDiv(tmp + pel[i][j], num);
			}
		}
	}

	/* III. Reconstruct dct */ 
	for (int m=0; m<8; m++){
		for (int n=0; n<8; n++){
			int i = scan[m][n];
			dct_recon[m][n] = (2*dct_zz[i]+Sign(dct_zz[i]))*quantizer_scale*non_intra_quant[m*8+n]/16;
			if ((dct_recon[m][n] & 1) == 0)
				dct_recon[m][n] -= Sign(dct_recon[m][n]);
			if (dct_recon[m][n] > 2047) dct_recon[m][n] = 2047;
			if (dct_recon[m][n] < -2048) dct_recon[m][n] = -2048;
			if (dct_zz[i] == 0) dct_recon[m][n] = 0;
		}
	}

	/* IV. IDCT */
	IDCT(dct_recon);

	/* V. Add pel matrix */
	for (int m=0; m<8; m++)
		for (int n=0; n<8; n++)
			dct_recon[m][n] += pel[m][n];
}

void Macroblock::dct_recon_skipped_p(	const int& cur_addr, const int& forward_addr,
										const int& mb_row, const int& mb_col){
	for (int bid=0; bid<6; bid++){
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

		for (int i=0; i<8; i++) for (int j=0; j<8; j++){
			image.image_buf[cur_addr][tid][row+i][col+j] = image.image_buf[forward_addr][tid][row+i][col+j];
		}
	}
}

void Macroblock::dct_recon_skipped_b(const int& forward_addr, const int& backward_addr, const int& cur_addr,
										const int& mb_row, const int& mb_col,
										const int* non_intra_quant, const int& quantizer_scale){
	for (int bid=0; bid<6; bid++){
		dct_recon_b(forward_addr, backward_addr, bid, mb_row, mb_col, non_intra_quant, quantizer_scale);
		image.inputYUV(cur_addr, bid, mb_row, mb_column, dct_recon);
	}
}

void Macroblock::recon_motion_vector_forward(	const int& forward_f, const int& full_pel_forward_vector,
												int& recon_right_for_prev, int& recon_down_for_prev){
	int complement_horizontal_forward_r;
	int complement_vertical_forward_r;

	if (forward_f == 1 || motion_horizontal_forward_code == 0){
		complement_horizontal_forward_r = 0;
	}
	else{
		complement_horizontal_forward_r = forward_f - 1 - motion_horizontal_forward_r;
	}

	if (forward_f == 1 || motion_vertical_forward_code == 0){
		complement_vertical_forward_r = 0;
	}
	else{
		complement_vertical_forward_r = forward_f - 1 - motion_vertical_forward_r;
	}

	int right_little = motion_horizontal_forward_code * forward_f;
	int right_big;
	if (right_little == 0){
		right_big = 0;
	}
	else{
		if (right_little > 0){
			right_little = right_little - complement_horizontal_forward_r;
			right_big = right_little - 32 * forward_f;
		}
		else {
			right_little = right_little + complement_horizontal_forward_r;
			right_big = right_little + 32 * forward_f;
		}
	}

	int down_little = motion_vertical_forward_code * forward_f;
	int down_big;
	if (down_little == 0){
		down_big = 0;
	}
	else{
		if (down_little > 0){
			down_little = down_little - complement_vertical_forward_r;
			down_big = down_little - 32 * forward_f;
		}
		else {
			down_little = down_little + complement_vertical_forward_r;
			down_big = down_little + 32 * forward_f;
		}
	}

	int max = (16 * forward_f) - 1;
	int min = (-16 * forward_f);

	int new_vector = recon_right_for_prev + right_little;
	if (new_vector <= max && new_vector >= min)
		recon_right_for = new_vector;
	else
		recon_right_for = recon_right_for_prev + right_big;
	recon_right_for_prev = recon_right_for;
	if (full_pel_forward_vector) recon_right_for = recon_right_for << 1;

	new_vector = recon_down_for_prev + down_little;
	if (new_vector <= max && new_vector >= min)
		recon_down_for = new_vector;
	else
		recon_down_for = recon_down_for_prev + down_big;
	recon_down_for_prev = recon_down_for;

	if (full_pel_forward_vector) recon_down_for = recon_down_for << 1;
}

void Macroblock::recon_motion_vector_backward(	const int& backward_f, const int& full_pel_backward_vector,
												int& recon_right_back_prev, int& recon_down_back_prev){
	int complement_horizontal_backward_r;
	int complement_vertical_backward_r;

	if (backward_f == 1 || motion_horizontal_backward_code == 0)
		complement_horizontal_backward_r = 0;
	else
		complement_horizontal_backward_r = backward_f - 1 - motion_horizontal_backward_r;

	if (backward_f == 1 || motion_vertical_backward_code == 0)
		complement_vertical_backward_r = 0;
	else
		complement_vertical_backward_r = backward_f - 1 - motion_vertical_backward_r;

	int right_little = motion_horizontal_backward_code * backward_f;
	int right_big;
	if (right_little == 0){
		right_big = 0;
	}
	else{
		if (right_little > 0){
			right_little = right_little - complement_horizontal_backward_r;
			right_big = right_little - 32 * backward_f;
		}
		else {
			right_little = right_little + complement_horizontal_backward_r;
			right_big = right_little + 32 * backward_f;
		}
	}

	int down_little = motion_vertical_backward_code * backward_f;
	int down_big;
	if (down_little == 0){
		down_big = 0;
	}
	else{
		if (down_little > 0){
			down_little = down_little - complement_vertical_backward_r;
			down_big = down_little - 32 * backward_f;
		}
		else {
			down_little = down_little + complement_vertical_backward_r;
			down_big = down_little + 32 * backward_f;
		}
	}

	int max = (16 * backward_f) - 1;
	int min = (-16 * backward_f);

	int new_vector = recon_right_back_prev + right_little;
	if (new_vector <= max && new_vector >= min)
		recon_right_back = new_vector;
	else
		recon_right_back = recon_right_back_prev + right_big;
	recon_right_back_prev = recon_right_back;
	if (full_pel_backward_vector) recon_right_back = recon_right_back << 1;

	new_vector = recon_down_back_prev + down_little;
	if (new_vector <= max && new_vector >= min)
		recon_down_back = new_vector;
	else
		recon_down_back = recon_down_back_prev + down_big;
	recon_down_back_prev = recon_down_back;
	if (full_pel_backward_vector) recon_down_back = recon_down_back << 1;
}
