# Overview

This project aims to provide a way for members of the RLBot community to contribute
their ideas to a common set of tools for arithmetic, simulation, and car controllers.

## Build and Test

To build, you need a copy of 64 bit python 3.6 or 3.7 and a C++ compiler (assuming Windows only), [cmake](https://cmake.org/download/):

Get a copy of the repo

```
git clone https://github.com/samuelpmish/RLUtilities
```

Run build.sh to build the C++ tools and their python bindings

## Adding to a project

### C++

Add this repo as a submodule to your project, so that you might easily receive updates to it:

```
> cd my_rlbot_folder
> git submodule add -b master git@github.com:samuelpmish/RLUtilities.git
> git submodule update --remote
```

The project uses CMake, so doing ```add_directory(path/to/RLUtilities/rlutilities/cpp)``` should allow you to
include it in an existing CMake project.

### Python

If you just want to use RLUtilites for your python bot you can clone this repo and install it by:

```
pip install -e .
```
