add_definitions(
    -DTW_STATIC
    -DTW_NO_LIB_PRAGMA
    -DTW_NO_DIRECT3D
    -DGLEW_STATIC
    -D_CRT_SECURE_NO_WARNINGS
)

# glfw
include_directories(glfw-3.2.1/include/GLFW/)
add_subdirectory (glfw-3.2.1)

set(OPENGL_LIBRARY
    ${OPENGL_LIBRARY}
    ${CMAKE_DL_LIBS}
    ${GLFW_LIBRARIES}
)

# glew
include_directories(glew-2.1.0/include/)
add_library(glew STATIC glew-2.1.0/src/glew.c glew-2.1.0/include/GL/glew.h tiny-1.1.1/tiny_obj_loader.cpp)
target_link_libraries(glew ${OPENGL_LIBRARY} ${EXTRA_LIBS})

# glm is header only

# tiny obj loader
add_library(tiny STATIC tiny-1.1.1/tiny_obj_loader.cpp tiny-1.1.1/tiny_obj_loader.h)
target_include_directories(tiny PRIVATE tiny_obj_loader)
