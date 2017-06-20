#ifndef SLICE_H
#define SLICE_H

#include "bit.h"

class Slice {
	public:
		Slice(InBit& x, const bool& d);
		~Slice();
		void decoder(int p_c_t, int f_f, int f_r_s, int b_f, int b_r_s);
	private:
		const bool& DEBUG;
		InBit& inBit;
		int picture_coding_type;
		int forward_f;
		int forward_r_size;
		int backward_f;
		int backward_r_size;

		/* syntax codes */
		// slice_start_code 0x00000101 ~ 0x000001AF

		/* slice info. */
		int slice_start_code;
		int slice_vertical_position;
		int quantizer_scale;
		int extra_bit_slice;
		int extra_information_slice;
};

#endif
