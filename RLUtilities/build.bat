git submodule init
git submodule update --remote

cd cpp

mkdir build
cd build
cmake .. -G"Visual Studio 15 2017 Win64"
cmake --build . --config Release
for /R . %%f in (*.pyd) do copy "%%f" ..\..\python\RLUtilities
