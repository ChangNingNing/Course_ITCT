#ifndef VIDEO_SEQ_H
#define VIDEO_SEQ_H

#include "bit.h"

class VideoSeq {
	public:
		VideoSeq(InBit& x, const bool& d);
		~VideoSeq();
		void video_sequence();
	private:
		const bool& DEBUG;
		InBit &inBit;
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
		int intra_quantizer_matrix[64];
		int load_non_intra_quantizer_matrix;
		int non_intra_quantizer_matrix[64];
		int sequence_extension_data;
		int user_data;

		/* group_of_pictures info. */
		int time_code;
		int closed_gop;
		int broken_link;
		int group_extension_data;
};

#endif
