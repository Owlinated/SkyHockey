#ifndef ENGINE_FRAMEBUFFER_H
#define ENGINE_FRAMEBUFFER_H

#include <memory>
#include <vector>
#include <GL/glew.h>
#include "Texture.h"
#include "IFramebuffer.h"

enum SamplingMode {
  Linear, Mipmap, Nearest
};

enum Precision {
  Pos16 = GL_RGB16, Float16 = GL_RGB16F, Float32 = GL_RGB32F
};

class Framebuffer : public IFramebuffer {
 private:
  GLuint handle_;
  std::vector<GLenum> draw_buffers;
 public:
  explicit Framebuffer(GLuint handle, std::shared_ptr<Texture> texture);
  Framebuffer(int width,
              int height,
              int texture_count,
              bool depth,
              SamplingMode sample,
              Precision precision,
              const std::string &fb_name);
  int width, height, texture_count;
  std::vector<std::shared_ptr<Texture>> textures;
  void bind() override;
  int getWidth() override { return width; }
  int getHeight() override { return height; }
  GLuint getHandle() override { return handle_; }
};

#endif //ENGINE_FRAMEBUFFER_H
