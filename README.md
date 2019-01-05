![RLUtilities](http://rlutilities.com/img/header.jpg)

# Overview

This project aims to provide a way for members of the RLBot community to contribute
their ideas to a common set of tools for arithmetic, simulation, and car controllers.

## Build and Test

To build, you need a copy of 64 bit python 3.6 and a C++ compiler (assuming Windows only), [cmake](https://cmake.org/download/) (and [git bash](https://git-scm.com/download/) when on Windows):

Get a copy of the repo

```
git clone https://github.com/samuelpmish/RLUtilities
```

Run the build script to download submodules and build the C++ tools and their python bindings

```
cd RLUtilities
./build.sh
```

## Adding to a project

### C++

Add this repo as a submodule to your project, so that you might easily receive updates to it:

```
> cd my_rlbot_folder
> git submodule add -b master git@github.com:samuelpmish/Utilities.git
> git submodule update --remote
```

The project uses CMake, so doing ```add_directory(path/to/RLUtilities/cpp)``` should allow you to
include it in an existing CMake project.

### Python

```
pip install RLUtilities
```

Check out [ExampleBots](https://github.com/samuelpmish/ExampleBots) for demonstrations of how to use these
tools in Python.
