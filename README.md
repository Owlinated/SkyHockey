# SkyHockey - Air Hockey in Space [![Build Status](https://travis-ci.org/Owlinated/SkyHockey.svg?branch=master)](https://travis-ci.org/Owlinated/SkyHockey)

// TODO intro

## Installation

Don't you just love setting up C++ projects? Here we go.

### Preparation

1. Setup your C++ toolchain of choice. (Tested with MinGW x86_64-6.1.0-posix-seh-rt_v5-rev1)
2. Install [CMake](https://cmake.org/download/) (Tested with 3.10.3)
3. Install [Python](https://www.python.org/downloads/) (Tested with 3.6)
4. Install [Conan](https://www.conan.io/downloads.html) as admin: `pip install conan`
5. Add Conan remote: `conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan True`

### Dependencies

1. Clone the repository: `git clone --recurse-submodules https://github.com/Owlinated/SkyHockey`
2. Navigate into directory: `cd SkyHockey/Engine`
3. Get dependencies: `conan install ./ --build`  
Depending on your environment you might want to specify your compiler:  
E.g. `-s compiler=gcc -s compiler.version=6.1`

### Build

1. Create build files: `cmake ./`  
Again, you might want to specify your profile:  
E.g. `-G "MinGW Makefiles"`
2. Build the project: `cmake --build ./ --target Engine`

## Controls
// TODO
