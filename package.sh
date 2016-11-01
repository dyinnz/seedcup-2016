#!/bin/bash

origin_root=`pwd`
echo 'pwd ' $origin_root
dest_root='初赛提交-有点菜'
src=$dest_root/Src
bin=$dest_root/Bin
doc=$dest_root/Doc

echo 'generete directories and copy files...'
mkdir -p $dest_root
mkdir -p $src
mkdir -p $bin
mkdir -p $doc

cp -r ./src $src/
cp -r ./common $src/
rm $src/common/catch.hpp

cp -r ./doc/* $doc/

head -n 15 ./CMakeLists.txt > $src/CMakeLists.txt
cp -r ./clean.sh $src/
cp -r ./input.txt $src/

echo 'test compiling...'
cd $src
cmake .
make
./main
./clean.sh
cd $origin_root

rm $dest_root.zip
zip -r $dest_root.zip $dest_root
