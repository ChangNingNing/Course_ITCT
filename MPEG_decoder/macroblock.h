#ifndef MACROBLOCK_H
#define MACROBLOCK_H

#include "bit.h"

class Macroblock {
	public:
		Macroblock(InBit& x, const bool& d);
		~Macroblock();
		void decoder();
	private:
		const bool& DEBUG;
		InBit& inBit;

		/* syntax codes */

		/* macroblock info. */
		int macroblock_stuffing;
		int macroblock_escape;
		int macroblock_address_increment;
		int macroblock_type;
		int macroblock_quant;
		int macroblock_motion_forward;
		int macroblock_motion_backward;
		int macroblock_pattern;
		int coded_block_pattern;
		int end_of_macroblock;
		int quantizer_scale;
		int forward_f;
		int backward_f;
		int motion_horizontal_forward_code;
		int motion_horizontal_forward_r;
		int motion_horizontal_backward_code;
		int motion_horizontal_backward_r;
		int motion_vertical_forward_code;
		int motion_vertical_forward_r;
		int motion_vertical_backward_code;
		int motion_vertical_backward_r;
};

#endif
