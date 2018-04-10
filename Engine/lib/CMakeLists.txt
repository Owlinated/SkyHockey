# tiny obj loader
add_library(tiny STATIC tiny-1.1.1/tiny_obj_loader.cpp tiny-1.1.1/tiny_obj_loader.h)
target_include_directories(tiny PRIVATE tiny_obj_loader)

# lodepng
add_library(lodepng STATIC lodepng/lodepng.cpp lodepng/lodepng.h)
target_include_directories(lodepng PRIVATE lodepng)
