#ifndef ENGINE_FRAMEBUFFER_H
#define ENGINE_FRAMEBUFFER_H

#include <memory>
#include <GL/glew.h>
#include "Texture.h"

class Framebuffer {
 private:
  GLuint handle_;
 public:
  explicit Framebuffer(GLuint handle, std::shared_ptr<Texture> texture);
  std::shared_ptr<Texture> texture;
  void bind();
  static std::shared_ptr<Framebuffer> create(int width, int height, bool depth, bool mipmap);
};

#endif //ENGINE_FRAMEBUFFER_H
