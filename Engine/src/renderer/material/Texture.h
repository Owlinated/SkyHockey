#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <memory>
#include <GL/glew.h>

class Texture {
 private:
  GLuint handle_;
  int width_, height_;
 public:
  explicit Texture(GLuint handle);
  int width() const { return width_; }
  int height() const { return height_; }
  GLuint handle() const { return handle_; }
  void bind();
  static std::shared_ptr<Texture> loadDds(const char * image_path);
  static std::shared_ptr<Texture> createDepthTexture(int width, int height);
};

#endif //TEXTURE_HPP