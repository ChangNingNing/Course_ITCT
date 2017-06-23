#ifndef PICTURE_H
#define PICTURE_H

#include "bit.h"
#include "slice.h"
#include "image.h"

class Picture {
	public:
		Picture(InBit& x, const bool& d, Slice& s, Image& i);
		~Picture();
		void decoder(	const int& horizontal_size, const int& vertical_size, const int& mb_width,
						const int* intra_quant, const int* non_intra_quant,
						int& forward_image_addr, int& backward_image_addr);
	private:
		const bool& DEBUG;
		InBit &inBit;
		Slice& slice;
		Image& image;
		int picture_num;
		int output_num;

		/* syntax codes */
		const static int picture_start_code = 0x00000100;
		const static int extension_start_code = 0x000001B5;
		const static int user_data_start_code = 0x000001B2;
		// slice_start_code 0x00000101 ~ 0x000001AF

		/* picture info. */
		unsigned int temporal_reference;
		int picture_coding_type;
		int vbv_delay;
		int full_pel_forward_vector;
		int full_pel_backward_vector;
		unsigned int forward_f_code;
		int forward_r_size;
		int forward_f;
		unsigned int backward_f_code;
		int backward_r_size;
		int backward_f;
		int extra_bit_picture;
		int extra_information_picture;
		int picture_extension_data;
		int user_data;
};

#endif
