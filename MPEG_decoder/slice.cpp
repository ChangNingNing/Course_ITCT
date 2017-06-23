#include "slice.h"
#include "bit.h"
#include "macroblock.h"
#include "block.h"

Slice::Slice(InBit& x, const bool& d, Macroblock& mb): inBit(x), DEBUG(d), macroblock(mb) {}

Slice::~Slice() {}

void Slice::decoder(const int& picture_coding_type,
					const int& full_pel_forward_vector, const int& full_pel_backward_vector,
					const int& forward_f, const int& forward_r_size,
					const int& backward_f, const int& backward_r_size,
					const int& mb_width,
					const int* intra_quant, const int* non_intra_quant,
					const int& forward_image_addr, const int& backward_image_addr, const int& cur_image_addr){
	dct_dc_y_past = 1024;
	dct_dc_cb_past = 1024;
	dct_dc_cr_past = 1024;
	past_intra_address = -2;

	recon_right_for_prev = 0;
	recon_down_for_prev = 0;
	recon_right_back_prev = 0;
	recon_down_back_prev = 0;
	/*  */

	slice_start_code = inBit.getBits(32);

	slice_vertical_position = slice_start_code & 0xFF;
	previous_macroblock_address = (slice_vertical_position - 1) * mb_width - 1;

	quantizer_scale = inBit.getBits(5);
	while ((inBit.nextbits() >> 31) & 0x1 == 0x1){
		extra_bit_slice = inBit.getBits(1);
		extra_information_slice = inBit.getBits(8);
	}
	extra_bit_slice = inBit.getBits(1);

	if (DEBUG){
		printf("==SLICE_START_CODE== -> %08x\n", slice_start_code);
		printf("	quantizer_scale: %d\n", quantizer_scale);
		printf("	extra_bit_slice: %d\n", extra_bit_slice);
	}

	do {
		macroblock.decoder(	picture_coding_type,
							full_pel_forward_vector, full_pel_backward_vector,
							forward_f, forward_r_size,
							backward_f, backward_r_size,
							recon_right_for_prev, recon_down_for_prev,
							recon_right_back_prev, recon_down_back_prev,
							quantizer_scale,
							dct_dc_y_past, dct_dc_cb_past, dct_dc_cr_past,
							past_intra_address, previous_macroblock_address,
							mb_width,
							intra_quant, non_intra_quant,
							forward_image_addr, backward_image_addr, cur_image_addr);
	} while ((inBit.nextbits() >> 9) != 0x0);
	inBit.next_start_code();
}
