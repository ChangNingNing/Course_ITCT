#include <iostream>
#include <fstream>
#include <cstring>

#include "bit.h"
#include "video_seq.h"
#include "picture.h"
#include "slice.h"
#include "macroblock.h"
#include "block.h"
#include "image.h"

using namespace std;

bool DEBUG = false;

int main(int argc, char *argv[]){
	if (argc < 2)
		cout << "usage: ./MPEG_decoder [input file] [-p]" << endl;
	if (argc == 3 && !strcmp(argv[2], "-p"))
		DEBUG = true;

	ifstream fin(argv[1], ios::binary);
	if (!fin.is_open()){
		cout << "open input file error." << endl;
		return 0;
	}

	InBit inBit(fin);
	Image image;

	Block block(inBit, DEBUG);
	Macroblock macroblock(inBit, DEBUG, block, image);
	Slice slice(inBit, DEBUG, macroblock);
	Picture picture(inBit, DEBUG, slice, image);
	VideoSeq videoSeq(inBit, DEBUG, picture);

	videoSeq.video_sequence();

	fin.close();
	return 0;
}
