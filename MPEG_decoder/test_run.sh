#!/bin/bash

dir="../MPEG_2016/MPEG/"
#film_l=("I_ONLY.M1V" "IP_ONLY.M1V" "IPB_ALL.M1V")
film_l=("I_ONLY.M1V")
exe="./MPEG_decoder.exe"

for film in ${film_l[*]}
do
	echo ${film}
	time ${exe} ${dir}${film}
done
