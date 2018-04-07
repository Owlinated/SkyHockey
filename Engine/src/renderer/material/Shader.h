#ifndef SHADER_HPP
#define SHADER_HPP

#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include "Texture.h"

class Shader {
 public:
  Shader(const char * vertex_file_path, const char * fragment_file_path);
  GLuint handle;
  void use();
  GLint getUniform(const char *uniform_name);
  void bind(std::shared_ptr<Texture>& texture, const char* uniform_name, int unit_index);
  void bind(int scalar, const char* uniform_name);
  void bind(float scalar, const char* uniform_name);
  void bind(glm::vec2 &vector, const char* uniform_name);
  void bind(glm::vec3 &vector, const char* uniform_name);
  void bind(glm::vec4 &vector, const char* uniform_name);
  void bind(glm::mat4 &matrix, const char* uniform_name);
};

#endif //SHADER_HPP
