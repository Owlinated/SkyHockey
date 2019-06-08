# tiny obj loader
add_library(tiny STATIC tinyobjloader/tiny_obj_loader.cc tinyobjloader/tiny_obj_loader.h)
target_include_directories(tiny PRIVATE tiny_obj_loader)
