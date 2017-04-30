#!/bin/bash
for i in *.jpg
do
	tmp=${i%%.jpg}
	echo ${tmp}
	time ./JpegDecoder.exe "../JPEG/${tmp}.jpg" "${tmp}.bmp" > ${tmp}.log
done
