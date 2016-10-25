#!/bin/bash
make img 
./img $1 $2
echo "Opening Image"
it output/$1.exr &
