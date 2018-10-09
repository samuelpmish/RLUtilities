# Overview

This project aims to provide a way for members of the RLBot community to contribute
their ideas to a common set of tools for arithmetic, simulation, and car controllers.

## Build and Test

To build, you need a copy of 64 bit python and a C++ compiler (assuming Windows only)

Get a copy of the repo

```
git clone https://github.com/samuelpmish/Utilities
```

Run the build script to download submodules and build the C++ tools and their python bindings

```
cd Utilities
./build.sh
```

## Adding to a project

Please consider adding this repo as a submodule to your project, so that you might easily
receive updates to it:

```
> cd my_rlbot_folder
> git submodule add -b master git@github.com:samuelpmish/Utilities.git
> git submodule update --remote
> cd Utilities
> ./build.sh
```

Then, (if your bot.py file is in my_rlbot_folder) you can import the modules:

```
from Utilities.Simulation import Car, Ball
from Utilities.LinearAlgebra import vec3, dot, euler_rotation
```
