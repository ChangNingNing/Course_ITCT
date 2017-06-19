#include "bit.h"
#include <iostream>
#include <fstream>

using namespace std;

InBit::InBit(ifstream& x): fin(x) {
	_reset();
}

InBit::~InBit() {}

void InBit::_reset() {
	buff = 0;
	bufSize = 0;
}

bool InBit::_read(){
	// Make sure input file has enough bytes.
	if (!fin.eof()){
		buff = fin.get();
		bufSize = MAXB;
		return true;
	}
	else{
		_reset();
		return false;
	}
}

int InBit::getBits(int num = 1) {
	int ret = 0;

	// keep reading in, until num == 0.
	while (num > 0){
		if (bufSize <= num){
			ret = (ret << bufSize) + (buff & ((1<<bufSize)-1));
			num -= bufSize;
			if (!_read()){
				cout << "file ended" << endl;
				return ret;
			}
		}
		else {
			ret = (ret << num) + ((buff >> (bufSize-num)) & ((1<<num)-1));
			bufSize -= num;
			num = 0;
		}
	}
	return ret;
}

void InBit::next_start_code() {
	while (!bytealigned())
		int zero_bit = getBits(1);
	while ((nextbits()>>8) != 0x000001)
		int zero_byte = getBits(8);
}

int InBit::nextbits() {
	int ret = 0;
	if (bufSize == 8)
		ret = buff & ((1<<MAXB)-1);
	else
		ret = (ret << MAXB) + fin.get();
	ret = (ret << MAXB) + fin.get();
	ret = (ret << MAXB) + fin.get();
	ret = (ret << MAXB) + fin.get();

	if (bufSize == 0)
		fin.unget();
	fin.unget();
	fin.unget();
	fin.unget();
	return ret;
}

bool InBit::bytealigned() {
	// check if current position is on a byte boundary.
	if (bufSize == 8 || bufSize == 0)
		return true;
	else
		return false;
}
