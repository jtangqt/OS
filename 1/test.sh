#!/bin/bash

bufsize=1
for i in {0..20}
do
	time ./mini.exe -b $bufsize -o output.txt dummy.txt
	echo Buffer size: $bufsize
	bufsize=$(($bufsize*2))
done