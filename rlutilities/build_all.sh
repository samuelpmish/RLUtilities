#!/bin/bash
pypath=C:/Users/sam/AppData/Local/Programs/Python

cd cpp

mkdir build_py36
cd build_py36
cmake -DPYTHON_EXECUTABLE="$pypath/python36/python.exe" .. -G"Visual Studio 15 2017 Win64" -DWITH_PYTHON=TRUE
cmake --build . --config Release
cp $(find . -name "*.pyd") ../../

cd ../
mkdir build_py37
cd build_py37
cmake -DPYTHON_EXECUTABLE="$pypath/python37/python.exe" .. -G"Visual Studio 15 2017 Win64" -DWITH_PYTHON=TRUE
cmake --build . --config Release
cp $(find . -name "*.pyd") ../../
