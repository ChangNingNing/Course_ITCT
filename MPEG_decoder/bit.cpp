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
	if (!bytealigned()) _read();
	while ((nextbits()>>8) != 0x000001)
		int zero_byte = getBits(8);
}

int InBit::nextbits() {
	int ret = buff;
	ret = (ret << 8) + fin.get();
	ret = (ret << 8) + fin.get();
	ret = (ret << 8) + fin.get();
	ret = (ret << (8-bufSize)) + ((fin.get() >> bufSize) & ((1 << (8-bufSize))-1));

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
