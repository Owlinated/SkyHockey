#ifndef ENGINE_SHAPE_H
#define ENGINE_SHAPE_H

#include <vector>
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class Shape {
 private:
  GLuint handle_;
  int index_count_;
 public:
  Shape(std::vector<unsigned short> &indices,
        std::vector<glm::vec3> &vertices,
        std::vector<glm::vec3> &normals,
        std::vector<glm::vec2> &textures);
  void bind();
  void bindVertexOnly();
  void draw();
};

#endif //ENGINE_SHAPE_H
