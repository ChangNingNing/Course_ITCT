#ifndef BLOCK_H
#define BLOCK_H

#include "bit.h"
#include <map>

typedef std::pair<int, int> Key;

class Block {
	public:
		Block(InBit& x, const bool& d);
		~Block();
		void decoder(	const int& i,
						const int& picture_coding_type, const int& macroblock_intra,
						int *dct_zz);
	private:
		const bool& DEBUG;
		InBit& inBit;

		/* function */
		void find_dct_dc_size_luminance(int *dct_zz);
		void find_dct_dc_size_chrominance(int *dct_zz);
		void find_dct_coeff_first(int *dct_zz);
		void find_dct_coeff_next(int *dct_zz);

		/* block info. */
		int dct_dc_differential;
		int dct_zz_i;
		int end_of_block;

		/* VLC table */
		static std::map<Key, int> dct_luminance_VLC_code;
		static std::map<Key, int> dct_chrominance_VLC_code;
		static std::map<Key, Key> dct_coeff_VLC_code;
};

#endif
