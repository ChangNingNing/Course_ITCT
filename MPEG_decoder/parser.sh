#!/bin/bash
rm log
for ((i=120; i<=120; i+=1))
do
	../MPEG_2016/MPEG_PARSER/MpegParser.exe ../MPEG_2016/MPEG/IPB_ALL.M1V $i >> log
done
