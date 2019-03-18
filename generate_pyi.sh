#!/bin/bash
python.exe pybind11_stubgen.py rlutilities
cp -r stubs/rlutilities-stubs/rlutilities/* rlutilities
rm -rf stubs
