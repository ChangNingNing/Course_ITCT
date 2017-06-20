#include "picture.h"
#include "bit.h"
#include "slice.h"

Picture::Picture(InBit& x, const bool& d): inBit(x), DEBUG(d){}

Picture::~Picture() {}

void Picture::decoder() {
	int _picture_start_code = inBit.getBits(32);
	temporal_reference = inBit.getBits(10);
	picture_coding_type = inBit.getBits(3);
	vbv_delay = inBit.getBits(16);
	if (picture_coding_type == 2 || picture_coding_type == 3){
		full_pel_forward_vector = inBit.getBits(1);
		forward_f_code = inBit.getBits(3);
		forward_r_size = forward_f_code - 1;
		forward_f = 1 << forward_r_size;
	}
	if (picture_coding_type == 3){
		full_pel_backward_vector = inBit.getBits(1);
		backward_f_code = inBit.getBits(3);
		backward_r_size = backward_f_code - 1;
		backward_f = 1 << backward_r_size;
	}
	while ((inBit.nextbits() >> 31) & 0x1 == 0x1){
		extra_bit_picture = inBit.getBits(1);
		extra_information_picture = inBit.getBits(8);
	}
	extra_bit_picture = inBit.getBits(1);
	inBit.next_start_code();

	if (inBit.nextbits() == extension_start_code){
		int _extension_start_code = inBit.getBits(32);
		while ((inBit.nextbits() >> 8) != 0x000001){
			picture_extension_data = inBit.getBits(8);
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
		puts("\n--- picture ---");
		printf("temporal_reference %d\n", temporal_reference);
		printf("picture_coding_type %d\n", picture_coding_type);
		printf("forward_f_code %d\n", forward_f_code);
		printf("forward_r_size %d\n", forward_r_size);
		printf("forward_f %d\n", forward_f);
		printf("backward_f_code %d\n", backward_f_code);
		printf("backward_r_size %d\n", backward_r_size);
		printf("backward_f %d\n", backward_f);
		printf("vbv_delay %d\n", vbv_delay);
		printf("extra_bit_picture %d\n", extra_bit_picture);
	}

	Slice slice(inBit, DEBUG);
	do {
		slice.decoder(picture_coding_type, forward_f, forward_r_size, backward_f, backward_r_size);
	} while (inBit.nextbits() >= 0x00000101 && inBit.nextbits() <= 0x000001AF);
	// slice_start_codes are a range.
}
