#ifndef SLICE_H
#define SLICE_H

#include "bit.h"
#include "macroblock.h"

class Slice {
	public:
		Slice(InBit& x, const bool& d, Macroblock& mb);
		~Slice();
		void decoder(	const int& picture_coding_type,
						const int& forward_f, const int& forward_r_size,
						const int& backward_f, const int& backward_r_size,
						const int& mb_width,
						const int* intra_quant, const int* non_intra_quant);
	private:
		const bool& DEBUG;
		InBit& inBit;
		Macroblock& macroblock;

		/* Intra type decoder */
		int dct_dc_y_past;
		int dct_dc_cb_past;
		int dct_dc_cr_past;
		int past_intra_address;

		/* syntax codes */
		// slice_start_code 0x00000101 ~ 0x000001AF

		/* slice info. */
		int slice_start_code;
		int slice_vertical_position;
		int quantizer_scale;
		int extra_bit_slice;
		int extra_information_slice;
		int previous_macroblock_address;
};

#endif
