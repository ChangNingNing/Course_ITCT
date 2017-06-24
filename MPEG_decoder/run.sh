#!/bin/bash

dir="./MPEG/"
film_l=("I_ONLY.M1V" "IP_ONLY.M1V" "IPB_ALL.M1V" "imsad.m1v")
exe=("./MPEG_decoder.exe" "./MPEG_player.exe")

echo ${exe[$1]} ${film_l[$2]}
time ${exe[$1]} ${dir}${film_l[$2]}

