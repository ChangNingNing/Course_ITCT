#include "slice.h"
#include "bit.h"
#include "macroblock.h"

Slice::Slice(InBit& x, const bool& d): inBit(x), DEBUG(d) {}

Slice::~Slice() {}

void Slice::decoder(int p_c_t, int f_f, int f_r_s, int b_f, int b_r_s){
	/*  */
	picture_coding_type = p_c_t;
	forward_f = f_f;
	forward_r_size = f_r_s;
	backward_f = b_f;
	backward_r_size = b_r_s;

	slice_start_code = inBit.getBits(32);
	slice_vertical_position = slice_start_code & 0xFF;
	quantizer_scale = inBit.getBits(5);
	while ((inBit.nextbits() >> 31) & 0x1 == 0x1){
		extra_bit_slice = inBit.getBits(1);
		extra_information_slice = inBit.getBits(8);
	}
	extra_bit_slice = inBit.getBits(1);

	if (DEBUG){
		puts("\n--- slice ---");
		printf("slice_start_code %d\n", slice_start_code);
		printf("slice_vertical_position %d\n", slice_vertical_position);
		printf("quantizer_scale %d\n", quantizer_scale);
	}

	Macroblock macroblock(inBit, DEBUG);
	do {
		macroblock.decoder(picture_coding_type, forward_f, forward_r_size, backward_f, backward_r_size);
	} while ((inBit.nextbits() >> 9) != 0x0);
	inBit.next_start_code();
}
