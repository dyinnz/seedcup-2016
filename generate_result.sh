#!/bin/sh

python3 test_tool.py $1 > tmp.c
gcc -c tmp.c
gcc -o tmp.exe tmp.o
./tmp.exe > /dev/null
rm tmp.*
