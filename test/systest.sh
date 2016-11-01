#!/bin/bash

echo "run systest on ${1}"

main=../SeedCup2016.exe
input=systest_in/${1}.c
gcc_output=systest_out/${1}_gcc.txt
lq_output=systest_out/${1}_lq.txt
tmp_file=tmp.c

# using gcc
sed "s/^\\/\\//\\/**\\//g" $input > $tmp_file
gcc $tmp_file
./a.out > $gcc_output
sed -i "s/ /\n/g" $gcc_output

# using lingqian
cp $input ./input.txt
$main $input
mv output.txt $lq_output
sed -i "s/ /\n/g" $lq_output

rm ./input.txt
rm $tmp_file
rm ./a.out
