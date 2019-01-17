git submodule init
git submodule update --remote

SET pypath="C:\Users\Admin\AppData\Local\Programs\Python"

cd cpp

mkdir build_py36
cd build_py36
cmake .. -DPYTHON_EXECUTABLE="%pypath%\Python36\python.exe" .. -G"Visual Studio 15 2017 Win64"
cmake --build . --config Release
cd ..


mkdir build_py37
cd build_py37
cmake .. -DPYTHON_EXECUTABLE="%pypath%\Python37\python.exe" .. -G"Visual Studio 15 2017 Win64"
cmake --build . --config Release
cd ..

for /R . %%f in (*.pyd) do copy "%%f" ..\python\RLUtilities