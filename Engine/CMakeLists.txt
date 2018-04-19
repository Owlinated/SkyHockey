# CMake entry point
cmake_minimum_required(VERSION 3.9)
set(PROJECT_NAME "Engine")
project(${PROJECT_NAME})

include(conanbuildinfo.cmake)
conan_basic_setup()

find_package(OpenGL REQUIRED)

# Compile non-conan dependencies
add_subdirectory(lib)

include_directories(
        lib/tinyobjloader/
        lib/lodepng/
        .
)

add_executable(${PROJECT_NAME}
        src/Main.cpp
        src/renderer/material/Shader.cpp src/renderer/material/Shader.h
        src/game/Camera.cpp src/game/Camera.h
        src/renderer/material/Texture.cpp src/renderer/material/Texture.h
        src/renderer/geometry/ObjLoader.cpp src/renderer/geometry/ObjLoader.h
        src/support/Window.cpp src/support/Window.h
        src/support/Formatter.h
        src/renderer/geometry/Shape.cpp src/renderer/geometry/Shape.h
        src/game/Puck.cpp src/game/Puck.h
        src/game/Striker.cpp src/game/Striker.h
        src/renderer/Renderer.cpp src/renderer/Renderer.h
        src/game/Game.cpp src/game/Game.h
        src/game/GameEntity.h src/game/GameEntity.cpp
        src/renderer/material/Framebuffer.cpp src/renderer/material/Framebuffer.h
        src/renderer/material/TexturePNG.cpp src/renderer/material/TexturePNG.h
        src/renderer/RenderEntity.cpp src/renderer/RenderEntity.h
        src/Config.cpp src/Config.h
        src/renderer/material/IFramebuffer.h)

target_link_libraries(${PROJECT_NAME}
        ${CONAN_LIBS}
        ${OPENGL_LIBRARY}
        tiny
        lodepng
        )

# Copy shaders to build directory
add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMENT "Copy Resources")

add_dependencies(${PROJECT_NAME} copy_resources)
