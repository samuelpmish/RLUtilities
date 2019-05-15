#!/bin/bash
cd cpp
mkdir build
cd build

if [ $1 = "MSVC" ]; then
  # MSVC
  cmake .. -G"Visual Studio 15 2017 Win64" -DWITH_TESTS=TRUE -DTARGET_LANGUAGE=cpp
  cmake --build . --config Release
else
  # MINGW
  cmake .. -G "MSYS Makefiles"  -DWITH_TESTS=TRUE -DTARGET_LANGUAGE=cpp -DCMAKE_BUILD_TYPE=Release
  make -j10
fi

