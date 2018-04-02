#ifndef SHADER_HPP
#define SHADER_HPP

#include "Texture.h"

class Shader {
 private:
  GLuint handle_;
 public:
  Shader(const char * vertex_file_path, const char * fragment_file_path);
  GLuint handle() const { return handle_; }
  void use();
  GLint getUniform(const char *uniform_name);
  void bind(std::shared_ptr<Texture>& texture, const char* uniform_name, int unit_index);
};

#endif //SHADER_HPP
