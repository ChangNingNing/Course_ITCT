#ifndef MACROBLOCK_H
#define MACROBLOCK_H

#include "bit.h"
#include <map>

typedef std::pair<int, int> Key;

class Macroblock {
	public:
		Macroblock(InBit& x, const bool& d);
		~Macroblock();
		void decoder(int p_c_t, int f_f, int f_r_s, int b_f, int b_r_s);
	private:
		const bool& DEBUG;
		InBit& inBit;
		int picture_coding_type;
		int forward_f;
		int forward_r_size;
		int backward_f;
		int backward_r_size;

		/* function */
		int find_macroblock_address_increment();
		int find_macroblock_type();
		int find_macroblock_motion_vector();

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
		int macroblock_intra;

		int coded_block_pattern;
		int end_of_macroblock;
		int quantizer_scale;
		int motion_horizontal_forward_code;
		int motion_horizontal_forward_r;
		int motion_horizontal_backward_code;
		int motion_horizontal_backward_r;
		int motion_vertical_forward_code;
		int motion_vertical_forward_r;
		int motion_vertical_backward_code;
		int motion_vertical_backward_r;

		static std::map<Key, int> macroblock_address_increment_VLC_code;
		static std::map<Key, int> macroblock_type_VLC_code[4];
		static std::map<Key, int> macroblock_motion_VLC_code;
		static std::map<Key, int> macroblock_pattern_VLC_code;
};
#endif
