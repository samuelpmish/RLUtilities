#!/bin/bash
git submodule init
git submodule update --remote
cd cpp
mkdir build
cd build
cmake .. -G"Visual Studio 15 2017 Win64" -DWITH_TESTS=TRUE
cmake --build . --config Release
cp $(find . -name "*.pyd") ../../python/rlutilities

ctest
