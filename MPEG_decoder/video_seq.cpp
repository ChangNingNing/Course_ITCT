#include "video_seq.h"
#include "bit.h"
#include "picture.h"
#include "image.h"
#include <iostream>

using namespace std;

VideoSeq::VideoSeq(InBit& x, const bool& d, Picture& p, Image& i): inBit(x), DEBUG(d), picture(p), image(i){}

VideoSeq::~VideoSeq() {}

void VideoSeq::video_sequence() {
	inBit.next_start_code();
	do {
		sequence_header();
		do {
			group_of_pictures();
		} while (inBit.nextbits() == group_start_code);

		// output the last image
		char fout[32];
		sprintf(fout, "mpeg%d.bmp", image.frameNum);
		image.outputBMP(backward_image_addr, vertical_size, horizontal_size, fout);
		image.outputFrame(backward_image_addr, vertical_size, horizontal_size);
	} while (inBit.nextbits() == sequence_header_code);

	int ret = inBit.getBits(32);
	if (ret != sequence_end_code)
		cout << "Video sequence ended with error." << endl;
	else
		cout << "Video sequence decoder succeed." << endl;
}

void VideoSeq::sequence_header() {
	int _seqence_header_code = inBit.getBits(32);

	horizontal_size = inBit.getBits(12);
	mb_width = (horizontal_size + 15) / 16;

	vertical_size = inBit.getBits(12);
	mb_height = (vertical_size + 15) / 16;

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
		load_default_intra_quantizer_matrix();
	}

	load_non_intra_quantizer_matrix = inBit.getBits(1);
	if (load_intra_quantizer_matrix){
		for (int i=0; i<64; i++)
			non_intra_quantizer_matrix[i] = inBit.getBits(8);
	}
	else{
		load_default_non_intra_quantizer_matrix();
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

	/*  */
	forward_image_addr = 0;
	backward_image_addr = 1;

/*
	if (DEBUG){
		puts("\n--- sequence_header ---");
		printf("horizontal_size %d\n", horizontal_size);
		printf("vertical_size %d\n", vertical_size);
		printf("pel_aspect_ratio %d\n", pel_aspect_ratio);
		printf("----- %f\n", table_pel_aspect_ratio[(unsigned int)pel_aspect_ratio]);
		printf("picture_rate %d\n", picture_rate);
		printf("----- %f\n", table_picture_rate[(unsigned int)picture_rate]);
		printf("bit_rate %d\n", bit_rate);
		printf("marker_bit %d\n", marker_bit);
		printf("vbv_buffer_size %d\n", vbv_buffer_size);
		printf("constrained_parameter_flag %d\n", constrained_parameter_flag);
		printf("load_intra_quantizer_matrix %d\n", load_intra_quantizer_matrix);
		printf("load_non_intra_quantizer_matrix %d\n", load_non_intra_quantizer_matrix);
		printf("intra_quantizer_matrix\n");
		for (int i=0; i<8; i++, puts(""))
			for (int j=0; j<8; j++)
				printf("%2d ", intra_quantizer_matrix[i*8+j]);
		printf("non_intra_quantizer_matrix\n");
		for (int i=0; i<8; i++, puts(""))
			for (int j=0; j<8; j++)
				printf("%2d ", non_intra_quantizer_matrix[i*8+j]);
	}
*/
}

void VideoSeq::group_of_pictures() {
	int _group_start_code = inBit.getBits(32);
	time_code = inBit.getBits(25);
	drop_frame_flag = (time_code >> 24) & 0x1;
	time_code_hours = (time_code >> 19) & 0x1F;
	time_code_minutes = (time_code >> 13) & 0x3F;
	marker_bit = (time_code >> 12) & 0x1;
	time_code_seconds = (time_code >> 6) & 0x3F;
	time_code_pictures = (time_code) & 0x3F;

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

/*
	if (DEBUG){
		puts("\n--- group_of_pictures ---");
		printf("time_code %d\n", time_code);
		printf("drop_frame_flag %d\n", drop_frame_flag);
		printf("time_code_hours %d\n", time_code_hours);
		printf("time_code_minutes %d\n", time_code_minutes);
		printf("marker_bit %d\n", marker_bit);
		printf("time_code_seconds %d\n", time_code_seconds);
		printf("time_code_pictures %d\n", time_code_pictures);
		printf("closed_gop %d\n", closed_gop);
		printf("broken_link %d\n", broken_link);
	}
*/

	do {
		picture.decoder(horizontal_size, vertical_size, mb_width,
						intra_quantizer_matrix, non_intra_quantizer_matrix,
						forward_image_addr, backward_image_addr);
	} while (inBit.nextbits() == picture_start_code);
}

void VideoSeq::load_default_intra_quantizer_matrix(){
	for (int i=0; i<64; i++)
		intra_quantizer_matrix[i] = default_intra_quantizer_matrix[i];
}

void VideoSeq::load_default_non_intra_quantizer_matrix(){
	for (int i=0; i<64; i++)
		non_intra_quantizer_matrix[i] = default_non_intra_quantizer_matrix[i];
}

int VideoSeq::width(){
	return horizontal_size;
}

int VideoSeq::height(){
	return vertical_size;
}

float VideoSeq::p_rate(){
	return table_picture_rate[picture_rate];
}

Frame* VideoSeq::get_frame(int num){
	if (image.frameNum != 0)
		return &(image.frame[num % image.frameNum]);
	else
		return &(image.frame[0]);
}
