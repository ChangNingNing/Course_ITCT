#ifndef BIT_H
#define BIT_H

#include <fstream>

#define MAXB 8
class InBit {
	public:
		InBit(std::ifstream& x);
		~InBit();

		int getBits(int num);
		void next_start_code();
		int nextbits();
		bool bytealigned();
	private:
		std::ifstream& fin;
		int buff;
		int bufSize;

		void _reset(); // discard the current content in buffer.
		bool _read(); // read 65536*1 byte once.
};


#endif
