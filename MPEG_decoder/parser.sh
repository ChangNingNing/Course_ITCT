#!/bin/bash
rm log
for ((i=1; i<=2; i+=1))
do
	../MPEG_2016/MPEG_PARSER/MpegParser.exe ../MPEG_2016/MPEG/IP_ONLY.M1V $i >> log
done
