#ifndef MACROBLOCK_H
#define MACROBLOCK_H

#include "bit.h"
#include "block.h"
#include "image.h"
#include <map>

typedef std::pair<int, int> Key;

const static int scan[8][8] = {
	0,	1,	5,	6,	14,	15,	27,	28,
	2,	4,	7,	13,	16,	26,	29,	42,
	3,	8,	12,	17,	25,	30,	41,	43,
	9,	11,	18,	24,	31,	40,	44,	53,
	10,	19,	23,	32,	39,	45,	52,	54,
	20,	22,	33,	38,	46,	51,	55,	60,
	21,	34,	37,	47,	50,	56,	59,	61,
	35,	36,	48,	49,	57,	58,	62,	63};

class Macroblock {
	public:
		Macroblock(InBit& x, const bool& d, Block& b, Image& i);
		~Macroblock();
		void decoder(	const int& picture_coding_type,
						const int& full_pel_forward_vector, const int& full_pel_backward_vector,
						const int& forward_f, const int& forward_r_size,
						const int& backward_f, const int& backward_r_size,
						int& recon_right_for_prev, int& recon_down_for_prev,
						int& recon_right_back_prev, int& recon_down_back_prev,
						int& quantizer_scale,
						int& dct_dc_y_past, int& dct_dc_cb_past, int& dct_dc_cr_past,
						int& past_intra_address, int& previous_macroblock_address,
						const int& mb_width,
						const int* intra_quant, const int* non_intra_quant,
						const int& forward_image_addr, const int& backward_image_addr, const int& cur_image_addr);
	private:
		const bool& DEBUG;
		InBit& inBit;
		Block& block;
		Image& image;

		/* Intra-macroblock Decoder */
		int dct_zz[64];
		int dct_recon[8][8];
		/* Reconstruct motion vector */
		int recon_right_for;
		int recon_down_for;
		int recon_right_back;
		int recon_down_back;

		/* function */
		int find_macroblock_address_increment();
		int find_macroblock_type(const int& picture_coding_type);
		int find_macroblock_motion_vector();
		int find_macroblock_pattern();
		void dct_recon_intra(	const int& bid, const int* intra_quant, const int& past_intra_address,
								int& quantizer_scale, int& y_past, int& cb_past, int& cr_past);
		void dct_recon_p(	const int& forward_addr, const int& bid, const int& mb_row, const int& mb_col,
							const int* non_intra_quant, const int& quantizer_scale);
		void dct_recon_b(	const int& forward_addr, const int& backward_addr, const int& bid,
							const int& mb_row, const int& mb_col,
							const int* non_intra_quant, const int& quantizer_scale);
		void dct_recon_skipped_p(const int& cur_addr, const int& forward_addr, const int& mb_row, const int& mb_col);
		void dct_recon_skipped_b(const int& forward_addr, const int& backward_addr, const int& cur_addr,
									const int& mb_row, const int& mb_col,
									const int* non_intra_quant, const int& quantizer_scale);
		void recon_motion_vector_forward(	const int& forward_f, const int& full_pel_forward_vector,
											int& recon_right_for_prev, int& recon_down_for_prev);
		void recon_motion_vector_backward(	const int& backward_f, const int& full_pel_backward_vector,
											int& recon_right_back_prev, int& recon_down_back_prev);

		/* syntax codes */

		/* macroblock info. */
		int macroblock_stuffing;
		int macroblock_escape;
		int macroblock_address_increment;
		int macroblock_address;
		int mb_row;
		int mb_column;

		int macroblock_type;
		int macroblock_quant;
		int macroblock_motion_forward;
		int macroblock_motion_backward;
		int macroblock_pattern;
		int macroblock_intra;

		int coded_block_pattern;
		int pattern_code[6];
		int end_of_macroblock;
		int motion_horizontal_forward_code;
		int motion_horizontal_forward_r;
		int motion_horizontal_backward_code;
		int motion_horizontal_backward_r;
		int motion_vertical_forward_code;
		int motion_vertical_forward_r;
		int motion_vertical_backward_code;
		int motion_vertical_backward_r;

		/* VLC table */
		static std::map<Key, int> macroblock_address_increment_VLC_code;
		static std::map<Key, int> macroblock_type_VLC_code[4];
		static std::map<Key, int> macroblock_motion_VLC_code;
		static std::map<Key, int> macroblock_pattern_VLC_code;
};
#endif
