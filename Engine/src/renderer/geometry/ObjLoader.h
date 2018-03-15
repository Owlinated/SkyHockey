#ifndef ENGINE_OBJLOADER_H
#define ENGINE_OBJLOADER_H

#include <tiny_obj_loader.h>
#include <vector>
#include <memory>
#include "Shape.h"

class ObjLoader {
 private:
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::shared_ptr<Shape> loadShape(tinyobj::shape_t shape);
 public:
  explicit ObjLoader(const char *path);
  std::shared_ptr<Shape> loadShape(const std::string &name);
  static std::shared_ptr<Shape> createQuad();
};

#endif //ENGINE_OBJLOADER_H