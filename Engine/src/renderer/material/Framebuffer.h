#ifndef ENGINE_FRAMEBUFFER_H
#define ENGINE_FRAMEBUFFER_H

#include <memory>
#include <vector>
#include <GL/glew.h>
#include "Texture.h"

class Framebuffer {
 private:
  GLuint handle_;
  std::vector<GLenum> draw_buffers;
 public:
  explicit Framebuffer(GLuint handle, std::shared_ptr<Texture> texture);
  Framebuffer(int width, int height, int texture_count, bool depth, bool mipmap, std::string debug_name);
  int width, height, texture_count;
  std::vector<std::shared_ptr<Texture>> textures;
  void bind();
};

#endif //ENGINE_FRAMEBUFFER_H
