#ifndef VIDEO_SEQ_H
#define VIDEO_SEQ_H

#include "bit.h"
#include "picture.h"

/* lookup table */
const float table_pel_aspect_ratio[16] = {
	-1.000, 1.0000, 0.6735, 0.7031,
	0.7615, 0.8055, 0.8437, 0.8935,
	0.9375, 0.9815, 1.0255, 1.0695,
	1.1250, 1.1575, 1.2015, 0.0000
};
const float table_picture_rate[16] = {
	-1.000, 23.976, 24.000, 25.000,
	29.970, 30.000, 50.000, 59.940,
	60.000, 0.0000, 0.0000, 0.0000,
	0.0000, 0.0000, 0.0000, 0.0000
};
const int default_intra_quantizer_matrix[64] = {
	 8, 16, 19, 22, 26, 27, 29, 34,
	16, 16, 22, 24, 27, 29, 34, 37,
	19, 22, 26, 27, 29, 34, 34, 38,
	22, 22, 26, 27, 29, 34, 37, 40,
	22, 26, 27, 29, 32, 35, 40, 48,
	26, 27, 29, 32, 35, 40, 48, 58,
	26, 27, 29, 34, 38, 46, 56, 69,
	27, 29, 35, 38, 46, 56, 69, 83
};
const int default_non_intra_quantizer_matrix[64] = {
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16
};

class VideoSeq {
	public:
		VideoSeq(InBit& x, const bool& d, Picture& p);
		~VideoSeq();
		void video_sequence();
	private:
		const bool& DEBUG;
		InBit &inBit;
		Picture& picture;

		void load_default_intra_quantizer_matrix();
		void load_default_non_intra_quantizer_matrix();
		void sequence_header();
		void group_of_pictures();

		/* syntax codes */
		const static int sequence_header_code = 0x000001B3;
		const static int sequence_end_code = 0x000001B7;
		const static int group_start_code = 0x000001B8;
		const static int extension_start_code = 0x000001B5;
		const static int user_data_start_code = 0x000001B2;
		const static int picture_start_code = 0x00000100;

		/* sequence_header info. */
		int horizontal_size;
		int vertical_size;
		int pel_aspect_ratio;
		int picture_rate;
		int bit_rate;
		int marker_bit;
		int vbv_buffer_size;
		int constrained_parameter_flag;
		int load_intra_quantizer_matrix;
		int load_non_intra_quantizer_matrix;
		int intra_quantizer_matrix[64];
		int non_intra_quantizer_matrix[64];
		int sequence_extension_data;
		int user_data;
		int mb_width;
		int mb_height;

		/* group_of_pictures info. */
		int time_code;
		int drop_frame_flag;
		int time_code_hours;
		int time_code_minutes;
		int time_code_seconds;
		int time_code_pictures;
		int closed_gop;
		int broken_link;
		int group_extension_data;
};

#endif
