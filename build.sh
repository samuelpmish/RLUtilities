#!/bin/bash


#for MSVC
#cd build_msvc
#cmake .. -G "Visual Studio 15 2017 Win64" -DWITH_TESTS=TRUE -DTARGET_LANGUAGE=both
#cmake --build . --config Release

#for MSYS/GCC
cd build_mingw
cmake .. -G "MSYS Makefiles" -DWITH_TESTS=TRUE -DCMAKE_BUILD_TYPE=Release -DTARGET_LANGUAGE=both
#cmake .. -G "MSYS Makefiles" -DWITH_TESTS=TRUE -DCMAKE_BUILD_TYPE=RelWithDebug -DTARGET_LANGUAGE=cpp
make -j10
