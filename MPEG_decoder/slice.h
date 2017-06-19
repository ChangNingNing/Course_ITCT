#ifndef SLICE_H
#define SLICE_H

#include "bit.h"

class Slice {
	public:
		Slice(InBit& x, const bool& d);
		~Slice();
		void decoder();
	private:
		const bool& DEBUG;
		InBit& inBit;

		/* syntax codes */
		// slice_start_code 0x00000101 ~ 0x000001AF

		/* slice info. */
		int quantizer_scale;
		int extra_bit_slice;
		int extra_information_slice;
};

#endif
