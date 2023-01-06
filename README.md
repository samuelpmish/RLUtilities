# Overview

This project provides some tools for car/ball simulation in Rocket League, and some basic controllers that might
be useful for writing a custom bot.

## Build and Test

1. Clone the repo and its dependencies

```
git clone https://github.com/samuelpmish/RLUtilities --recursive
```

2. Use [cmake](https://cmake.org/download/) to configure the project for the generator you want (Makefiles, Visual Studio, Ninja, ... )

```
cd RLUtilities
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

3. Build the library

```
cmake --build . --parallel
```

## Adding to a project

### C++

Add this repo as a submodule to your project, so that you might easily receive updates to it:

```
> cd my_rlbot_folder
> git submodule add -b develop git@github.com:samuelpmish/RLUtilities.git
> git submodule update --remote
```

The project uses CMake, so put ```add_directory(path/to/RLUtilities)``` in your CMakeLists.txt to
include it in an existing CMake project. From there, link it to any target that depends on it.

### Python
Download the [latest Release](https://github.com/samuelpmish/RLUtilities/releases/latest).

