# Lobot

Lobot is a repo for a Rocket League bot, and a collection of tools used to
make predictions about how the car and ball behave. Although the sources 
are written in C++, it is possible to access some of these utilities in
python.

## Build and Test

To build, you need a copy of 64 bit python and a C++ compiler (assuming Windows only)

Get a copy of the repo

```
git clone https://github.com/samuelpmish/Lobot
```


Initialize the submodules

```
cd Lobot
git submodule init
git submodule update --remote
```

Generate the project files (e.g. for Visual Studio 15)

```
cmake . -G"Visual Studio 15 2017 Win64"
```

Build

```
cmake --build . --config Release
```

Testing (C++)

```
cd tests
ctest.exe
```
