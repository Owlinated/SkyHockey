# CMake entry point
cmake_minimum_required (VERSION 3.9)
set (PROJECT_NAME "Engine")
project (${PROJECT_NAME})

find_package(OpenGL REQUIRED)

# Compile external dependencies
add_subdirectory (lib)

include_directories(
    lib/glfw-3.2.1/include/
    lib/glew-2.1.0/include/
    lib/glm-0.9.8.5/
    lib/tiny-1.1.1/
    .
)

add_definitions(
    -DTW_STATIC
    -DTW_NO_LIB_PRAGMA
    -DTW_NO_DIRECT3D
    -DGLEW_STATIC
    -D_CRT_SECURE_NO_WARNINGS
)

# Tutorial 16
add_executable(${PROJECT_NAME}
        src/Main.cpp
        src/renderer/material/Shader.cpp src/renderer/material/Shader.h
        src/game/Camera.cpp src/game/Camera.h
        src/renderer/material/Texture.cpp src/renderer/material/Texture.h
        src/renderer/geometry/ObjLoader.cpp src/renderer/geometry/ObjLoader.h
        src/support/Window.cpp src/support/Window.h
        src/support/Formatter.h
        src/renderer/geometry/Shape.cpp src/renderer/geometry/Shape.h
        src/renderer/VisualDebug.cpp src/renderer/VisualDebug.h
        src/game/Puck.cpp src/game/Puck.h
        src/game/Striker.cpp src/game/Striker.h
        src/renderer/Renderer.cpp src/renderer/Renderer.h
        src/game/Game.cpp src/game/Game.h
        src/renderer/RenderEntity.h src/game/GameEntity.h)

# Copy shaders to build directory
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${destination}
    COMMENT "Copy shaders from ${CMAKE_SOURCE_DIR}/res to ${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARY}
    glfw
    glew
    tiny
)