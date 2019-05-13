#!/bin/bash

echo "converting soccar files..."
cd soccar
for file in *.bin
do
  ld -r -b binary ${file} -o ${file:0:-4}.o
done
cd ../

echo "converting hoops files..."
cd hoops
for file in *.bin
do
  ld -r -b binary ${file} -o ${file:0:-4}.o
done
cd ../

echo "converting dropshot files..."
cd dropshot
for file in *.bin
do
  ld -r -b binary ${file} -o ${file:0:-4}.o
done
cd ../

echo "converting throwback files..."
cd throwback
for file in *.bin
do
  ld -r -b binary ${file} -o ${file:0:-4}.o
done
cd ../

echo "converting LUT files..."
cd LUT
for file in *.bin
do
  ld -r -b binary ${file} -o ${file:0:-4}.o
done
cd ../

echo "done!"
