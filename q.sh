#!/bin/bash
make img 
echo "Raytracing"
./img $1 $2
echo "Opening Image"
it output/$1.exr &
