# Overview

This project aims to provide a way for members of the RLBot community to contribute
their ideas to a common set of tools for arithmetic, simulation, and car controllers.

## Build and Test

To build, you need a copy of 64 bit python and a C++ compiler (assuming Windows only)

Get a copy of the repo

```
git clone https://github.com/samuelpmish/Utilities
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
