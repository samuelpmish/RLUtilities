#!/bin/bash

cd build

#for MSVC
cmake .. -G "Visual Studio 15 2017 Win64" -DWITH_TESTS=TRUE -DTARGET_LANGUAGE=cpp
cmake --build . --config Release

#for MSYS/GCC
#cmake .. -G "MSYS Makefiles" -DWITH_TESTS=TRUE -DCMAKE_BUILD_TYPE=Release -DTARGET_LANGUAGE=cpp
#make -j10
