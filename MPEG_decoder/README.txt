ITCI Final Project	R05922087

Environment:
	win64, gcc version 5.3.0 (GCC)

Compile:
	Just use Makefile, MPEG_decoder and MPEG_play will be compiled out.
	You can see the detail of compilation in Makefile if you want.

Run test case:
	Use git bash script "run.sh".

	./run.sh [0~1] [0~2]

	, where
	the first argument's 	0 for	MPEG_decode
							1 for MPEG_player

	the second argument's	0 for I_Only.M1V
							1 for IP_ONLY.M1V
							2 for IPB_ALL.M1v

Run by your self:
	MPEG decoder

		./MPEG_decoder.exe [input file] [-p]

		, where
		-p : print the debugging information

	MPEG player

		./MPEG_player.exe [input file]

Note:
	1, Bmp files of the film will be created, and the file's name of each .bmp is the output order.
	2, GUI.cpp is reference from others.
