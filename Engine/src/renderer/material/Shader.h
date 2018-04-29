#ifndef SHADER_HPP
#define SHADER_HPP

#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <map>
#include "Texture.h"

/**
 * Holds a shader program.
 */
class Shader {
 private:
  std::map<std::string, int> cache_;
 public:
  Shader(const std::string & vertex_file_path, const std::string & fragment_file_path);
  GLuint handle;
  void use();
  GLint getUniform(const std::string &uniform_name);
  void bind(std::shared_ptr<Texture> &texture, const std::string &uniform_name, int unit_index);
  void bind(int scalar, const std::string &uniform_name);
  void bind(float scalar, const std::string &uniform_name);
  void bind(glm::vec2 &vector, const std::string &uniform_name);
  void bind(glm::vec3 &vector, const std::string &uniform_name);
  void bind(glm::vec4 &vector, const std::string &uniform_name);
  void bind(glm::mat4 &matrix, const std::string &uniform_name);
  void bind(std::vector<int> &vector, const std::string &uniform_name);
};

#endif //SHADER_HPP
