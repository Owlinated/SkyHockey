# SkyHockey - Air Hockey in Space [![Build Status](https://travis-ci.org/flostellbrink/SkyHockey.svg?branch=master)](https://travis-ci.org/flostellbrink/SkyHockey)

This is a small physics based Air Hockey game. The project is part of the 2018 CG Course DAT205/DIT226 at Chalmers.

The following features are relevant for the course:
- Game
- Deferred Shading
- Variance Shadow Mapping
- Motion Blur
- FXAA
- Collision detection and basic physics
- Performance overlay

You, the player, control the blue striker with your mouse and try to push the puck into the opponent's goal. The "AI" opponent is a bit overpowered and will probably stop you from scoring most of the time.

The score board above the field indicates the current score, which is most likely a pretty remarkable lead for your opponent.

In the lower left corner you can find a performance overlay. This indicates the time required for the last 100 frames. The red line indicates a frame time of 1f/30fps, the green one 1f/60fps. As you can see my poor laptop manages 10fps quite easily!

![Screenshot](Screenshots/game_with_overlay.png?raw=true "Screenshot")

## Credits

I am using the following libraries:

- [GLFW](https://github.com/glfw/glfw) under a [zlib license](https://github.com/glfw/glfw/blob/master/LICENSE.md)
- [GLM](https://github.com/g-truc/glm) under the [Happy Bunny License](https://github.com/g-truc/glm/blob/master/manual.md#section0)
- [GLEW](https://github.com/nigels-com/glew) under this [license](https://github.com/nigels-com/glew/blob/master/LICENSE.txt)
- [tinyobjloader](https://github.com/syoyo/tinyobjloader) under an [MIT license](https://github.com/syoyo/tinyobjloader/blob/master/LICENSE)
- [lodepng](https://github.com/lvandeve/lodepng) under a [voluntary attribution license](https://github.com/lvandeve/lodepng/blob/d03d7df9888aafb9c7f615895c34b05acf033908/LICENSE)

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

## Rendering

This game has two render modes: Forward and deferred.

When using forward rendering everything is very straight forward.
The process for deferred rendering involves a few intermediate states.
Let's look at the actual steps:

1. The Shadow map is rendered from the light's perspective.
    1. 1 Channel: The depth value is normalized and stored.
        ![Depth](Screenshots/deferred_0_depth.png?raw=true "Depth")
    2. 1 Channel: The square of the depth is also normalized and stored.
        ![DepthSquared](Screenshots/deferred_0_depth_squared.png?raw=true "DepthSquared")
    3. The entire texture is blurred to enable smooth shadows with VSM.
2. All objects properties are collected in 4 textures. These are the stored channels:
    1. 1 Channel: The objects ids, for looking up material properties:
        ![Ids](Screenshots/deferred_1_id.png?raw=true "Ids")
    2. 3 Channels: The albedo colors, contain the object textures:
        ![Colors](Screenshots/deferred_1_color.png?raw=true "Colors")
    3. 3 Channels: The worldspace coordinates, for shadow mapping:
        ![Coords](Screenshots/deferred_1_coords.png?raw=true "Coords")
    4. 3 Channels: The surface normals, for lighting:
        ![Normals](Screenshots/deferred_1_normals.png?raw=true "Normals")
    5. 2 Channels: The camera space object velocity, for motion blur:
        ![Velocities](Screenshots/deferred_1_velocity.png?raw=true "Velocities")
3. The information from all textures is composited into a single texture.
4. Motion blur is applied to the rendered image.
5. FXAA is applied to hide aliasing artifacts.
6. The blurred image is combined with the sharp background.
7. Finally the performance overlay is drawn ontop of the frame.

## Installation

Don't you just love setting up C++ projects? Here we go.

### Preparation

1. Setup your C++ toolchain of choice. (Tested with MinGW x86_64-6.1.0-posix-seh-rt_v5-rev1)
2. Install [CMake](https://cmake.org/download/) (Tested with 3.10.3)
3. Install [Python](https://www.python.org/downloads/) for Conan (Tested with 3.6)
4. Install [Conan](https://www.conan.io/downloads.html) as admin: `pip install conan`
5. Add Conan remote: `conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan True`

### Dependencies

1. Clone the repository: `git clone --recurse-submodules https://github.com/Owlinated/SkyHockey`
2. Navigate into directory: `cd SkyHockey/Engine`
3. Get dependencies: `conan install . --build`

### Build

1. Build with conan `conan build .`
