#include "video_seq.h"
#include "bit.h"
#include "picture.h"
#include <iostream>

using namespace std;

VideoSeq::VideoSeq(InBit& x, const bool& d): inBit(x), DEBUG(d) {}

VideoSeq::~VideoSeq() {}

void VideoSeq::video_sequence() {
	inBit.next_start_code();
	do {
		sequence_header();
		do {
			group_of_pictures();
		} while (inBit.nextbits() == group_start_code);
	} while (inBit.nextbits() == sequence_header_code);

	int ret = inBit.getBits(32);
	if (ret != sequence_end_code)
		cout << "Video sequence ended with error." << endl;
}

void VideoSeq::sequence_header() {
	int _seqence_header_code = inBit.getBits(32);
	horizontal_size = inBit.getBits(12);
	vertical_size = inBit.getBits(12);
	pel_aspect_ratio = inBit.getBits(4);
	picture_rate = inBit.getBits(4);
	bit_rate = inBit.getBits(18);
	marker_bit = inBit.getBits(1);
	vbv_buffer_size = inBit.getBits(10);
	constrained_parameter_flag = inBit.getBits(1);
	load_intra_quantizer_matrix = inBit.getBits(1);
	if (load_intra_quantizer_matrix){
		for (int i=0; i<64; i++)
			intra_quantizer_matrix[i] = inBit.getBits(8);
	}
	else{
	
	}
	load_non_intra_quantizer_matrix = inBit.getBits(1);
	if (load_intra_quantizer_matrix){
		for (int i=0; i<64; i++)
			non_intra_quantizer_matrix[i] = inBit.getBits(8);
	}
	else{
	
	}
	inBit.next_start_code();
	if (inBit.nextbits() == extension_start_code){
		int _extexsion_start_code = inBit.getBits(32);
		while ((inBit.nextbits() >> 8) != 0x000001){
			sequence_extension_data = inBit.getBits(8);
		}
		inBit.next_start_code();
	}
	if (inBit.nextbits() == user_data_start_code){
		int _user_data_start_code = inBit.getBits(32);
		while ((inBit.nextbits() >> 8) != 0x000001){
			user_data = inBit.getBits(8);
		}
		inBit.next_start_code();
	}

	if (DEBUG){
		puts("\n--- sequence_header ---");
		printf("horizontal_size %d\n", horizontal_size);
		printf("vertical_size %d\n", vertical_size);
		printf("pel_aspect_ratio %d\n", pel_aspect_ratio);
		printf("picture_rate %d\n", picture_rate);
		printf("bit_rate %d\n", bit_rate);
		printf("marker_bit %d\n", marker_bit);
		printf("vbv_buffer_size %d\n", vbv_buffer_size);
		printf("constrained_parameter_flag %d\n", constrained_parameter_flag);
		printf("load_intra_quantizer_matrix %d\n", load_intra_quantizer_matrix);
		printf("load_non_intra_quantizer_matrix %d\n", load_non_intra_quantizer_matrix);
	}
}

void VideoSeq::group_of_pictures() {
	int _group_start_code = inBit.getBits(32);
	time_code = inBit.getBits(25);
	closed_gop = inBit.getBits(1);
	broken_link = inBit.getBits(1);
	inBit.next_start_code();
	if (inBit.nextbits() == extension_start_code){
		int _extension_start_code = inBit.getBits(32);
		while ((inBit.nextbits() >> 8) != 0x000001){
			group_extension_data = inBit.getBits(8);
		}
		inBit.next_start_code();
	}
	if (inBit.nextbits() == user_data_start_code){
		int _user_data_start_code = inBit.getBits(32);
		while ((inBit.nextbits() >> 8) != 0x000001){
			user_data = inBit.getBits(8);
		}
		inBit.next_start_code();
	}

	if (DEBUG){
		puts("\n--- group_of_pictures ---");
		printf("time_code %d\n", time_code);
		printf("closed_gop %d\n", closed_gop);
		printf("broken_link %d\n", broken_link);
	}

	do {
		Picture picture(inBit, DEBUG);
		picture.decoder();
	} while (inBit.nextbits() == picture_start_code);
}
