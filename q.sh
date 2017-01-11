#!/bin/bash
make clean
make 
make genswig
python noise.py NoiseVolumeTest 1 2
#./img $1 $2
#echo "Opening Image"
#it output/$1.exr &
