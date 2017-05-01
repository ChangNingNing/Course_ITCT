#!/bin/bash
for i in *.jpg
do
	tmp=${i%%.jpg}
	printf "\n${tmp}.jpg"
	time ./JpegDecoder "${tmp}.jpg" "${tmp}.bmp" >${tmp}.log
done
