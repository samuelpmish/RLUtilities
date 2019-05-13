#!/bin/bash
cd cpp
mkdir build
cd build
#cmake .. -G"Visual Studio 15 2017 Win64" -DWITH_TESTS=TRUE -DWITH_PYTHON=TRUE
cmake .. -G"Visual Studio 15 2017 Win64" -DWITH_TESTS=TRUE -DWITH_PYTHON=FALSE
#cmake --build . --config RelWithDebInfo
cmake --build . --config Release
cp $(find . -name "*.pyd") ../../
