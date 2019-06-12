# CMake entry point
cmake_minimum_required(VERSION 3.9)
set (CMAKE_CXX_STANDARD 14)
set(PROJECT_NAME "Engine")
project(${PROJECT_NAME})

include(conanbuildinfo.cmake)
conan_basic_setup()

find_package(OpenGL REQUIRED)

# Compile non-conan dependencies
add_subdirectory(lib)

include_directories(
        lib/tinyobjloader/
        lib/oculus/LibOVR/Include
        .
)

add_executable(${PROJECT_NAME}
        src/Main.cpp
        src/renderer/material/Shader.cpp src/renderer/material/Shader.h
        src/game/Camera.cpp src/game/Camera.h
        src/renderer/material/Texture.cpp src/renderer/material/Texture.h
        src/renderer/geometry/ObjLoader.cpp src/renderer/geometry/ObjLoader.h
        src/support/Window.cpp src/support/Window.h
        src/support/Oculus.cpp src/support/Oculus.h
        src/support/OculusEye.cpp src/support/OculusEye.h
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
        src/renderer/material/IFramebuffer.h
        src/renderer/material/Light.h
        src/renderer/material/Material.h
        src/support/Animator.cpp src/support/Animator.h
        src/support/FadeAnimation.cpp src/support/FadeAnimation.h
        src/support/Logger.cpp src/support/Logger.cpp)

target_link_libraries(${PROJECT_NAME}
        ${CONAN_LIBS}
        ${OPENGL_LIBRARY}
        tiny
        ${CMAKE_SOURCE_DIR}/lib/oculus/LibOVR/Lib/Windows/x64/Release/VS2017/LibOVR.lib
        )

# Copy shaders to build directory
add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res
        COMMENT "Copy Resources")

add_dependencies(${PROJECT_NAME} copy_resources)

# Setup installer
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
install(DIRECTORY res
        DESTINATION ./)

SET(CPACK_PACKAGE_NAME "SkyHockey")
SET(CPACK_PACKAGE_VERSION_MAJOR 1)
SET(CPACK_PACKAGE_VERSION_MINOR 0)
SET(CPACK_PACKAGE_VERSION_PATCH 0)
SET(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/../README.md)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AirHockey in Space")
SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/../LICENSE)
SET(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME};SkyHockey")
SET(CPACK_CREATE_DESKTOP_LINKS "${PROJECT_NAME}")
include(CPack)
