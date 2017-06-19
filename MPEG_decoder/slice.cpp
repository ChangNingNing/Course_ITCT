#include "slice.h"
#include "bit.h"
#include "macroblock.h"

Slice::Slice(InBit& x, const bool& d): inBit(x), DEBUG(d) {}

Slice::~Slice() {}

void Slice::decoder(){
	int _slice_start_code = inBit.getBits(32);
	quantizer_scale = inBit.getBits(5);
	while ((inBit.nextbits() >> 31) & 0x1 == 0x1){
		extra_bit_slice = inBit.getBits(1);
		extra_information_slice = inBit.getBits(8);
	}
	extra_bit_slice = inBit.getBits(1);

	if (DEBUG){
		puts("\n--- slice ---");
		printf("quantizer_scale %d\n", quantizer_scale);
	}

	do {
		Macroblock macroblock(inBit, DEBUG);
		macroblock.decoder();
	} while ((inBit.nextbits() >> 9) != 0x0);
	inBit.next_start_code();
}
