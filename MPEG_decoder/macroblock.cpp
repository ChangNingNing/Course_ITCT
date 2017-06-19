#include "macroblock.h"
#include "bit.h"
#include <stdlib.h>

Macroblock::Macroblock(InBit& x, const bool& d): inBit(x), DEBUG(d) {}

Macroblock::~Macroblock() {}

void Macroblock::decoder(){
	while ((inBit.nextbits() >> 21) == 0x00F){
		macroblock_stuffing = inBit.getBits(11);
	}
	while ((inBit.nextbits() >> 21) == 0x008){
		macroblock_escape = inBit.getBits(11);
	}
	exit(1);
}
