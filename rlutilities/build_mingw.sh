#!/bin/bash
cd cpp/build
cmake .. -G "MSYS Makefiles"  -DWITH_TESTS=TRUE -DTARGET_LANGUAGE=cpp -DCMAKE_BUILD_TYPE=Release
make -j10
#cp $(find . -name "*.pyd") ../../
