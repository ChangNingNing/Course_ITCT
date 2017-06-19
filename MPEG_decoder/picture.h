#ifndef PICTURE_H
#define PICTURE_H

#include "bit.h"

class Picture {
	public:
		Picture(InBit& x, const bool& d);
		~Picture();
		void decoder();
	private:
		const bool& DEBUG;
		InBit &inBit;

		/* syntax codes */
		const static int picture_start_code = 0x00000100;
		const static int extension_start_code = 0x000001B5;
		const static int user_data_start_code = 0x000001B2;
		// slice_start_code 0x00000101 ~ 0x000001AF

		/* picture info. */
		int temporal_reference;
		int picture_coding_type;
		int vbv_delay;
		int full_pel_forward_vector;
		int forward_f_code;
		int full_pel_backward_vector;
		int backward_f_code;
		int extra_bit_picture;
		int extra_information_picture;
		int picture_extension_data;
		int user_data;
};

#endif
