# SkyHockey - Air Hockey in Space [![Build Status](https://travis-ci.org/Owlinated/SkyHockey.svg?branch=master)](https://travis-ci.org/Owlinated/SkyHockey)

This is a small physics based Air Hockey game. The project is part of the 2018 CG Course DAT205/DIT226 at Chalmers.

The following features are relevant for the course:
- Deferred Shading
- Variance Shadow Mapping
- Motion Blur
- FXAA

You, the player, control the blue striker with your mouse and try to push the puck into the opponent's goal. The "AI" opponent is a bit overpowered and will probably stop you from scoring most of the time.

The score board above the field indicates the current score, which is most likely a pretty remarkable lead for your opponent.

In the lower left corner you can find a performance overlay. This indicates the time required for the last 100 frames. The red line indicates a frame time of 1f/30fps, the green one 1f/60fps. As you can see my poor laptop manages 10fps quite easily!

![Screenshot](screenshot.png?raw=true "Screenshot")

## Credits

I am using the following libraries (not part of this repository):

- [GLFW](https://github.com/glfw/glfw)
- [GLM](https://github.com/g-truc/glm)
- [GLEW](https://github.com/nigels-com/glew)

I adapted these shader:

- [Space Background](https://www.shadertoy.com/view/MslGWN) under [CC BY-NC-SA 3.0](https://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US)
- [FXAA](https://github.com/NVIDIAGameWorks/GraphicsSamples) under this [license](https://github.com/NVIDIAGameWorks/GraphicsSamples/blob/master/license.txt)

This software contains source code provided by NVIDIA Corporation.

## Controls

| Key   | Action                                              |
|-------|-----------------------------------------------------|
| SPACE | Start the game.                                     |
| R     | Change between deferred and forward rendering.      |
| S     | Toggle animated background on and off.              |
| U/J   | Increase/Decrease smoothness of shadows.            |
| I/K   | Increase/Decrease number of motion blur samples.    |
| O/L   | Increase/Decrease sample step size for motion blur. |
| T/G   | Increase/Decrease anti aliasing quality.            |
| P     | Toggle performance overlay on and off.              |
| Y/H   | Increase/Decrease performance overlay size.         |

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
