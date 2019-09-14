#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <memory>
#include <glad/glad.h>

/**
 * Holds a color texture.
 */
class Texture {
 protected:
  Texture();
  void init();
 public:
  explicit Texture(GLuint handle, int width, int height);
  GLuint handle;
  int width, height;
  void bind();
};

#endif //TEXTURE_HPP