COMPILER=g++
GXXFLAG=-std=c++11 -O2
OFILE=main.o bit.o video_seq.o picture.o slice.o macroblock.o block.o image.o

all: MPEG_decoder

main: main.cpp
	$(COMPILER) $(GXXFLAG) main.cpp -c

bit: bit.cpp
	$(COMPILER) $(GXXFLAG) bit.cpp -c

video_seq: video_seq.cpp
	$(COMPILER) $(GXXFLAG) video_seq.cpp -c

picture: picture.cpp
	$(COMPILER) $(GXXFLAG) picture.cpp -c

slice: slice.cpp
	$(COMPILER) $(GXXFLAG) slice.cpp -c

macroblock: macroblock.cpp
	$(COMPILER) $(GXXFLAG) macroblock.cpp -c

block: block.cpp
	$(COMPILER) $(GXXFLAG) block.cpp -c

image: image.cpp
	$(COMPILER) $(GXXFLAG) image.cpp -c

MPEG_decoder: main.o bit.o video_seq.o picture.o slice.o macroblock.o block.o image.o
	$(COMPILER) $(GXXFLAG) $(OFILE) -o MPEG_decoder

clean:
	rm *.o MPEG_decoder
