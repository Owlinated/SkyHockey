#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <src/renderer/material/IFramebuffer.h>

class Window : public IFramebuffer {
 public:
  Window();
  int width, height;
  GLFWwindow *handle;
  void bind() override;
  int getWidth() override { return width; }
  int getHeight() override { return height; }
};

#endif //ENGINE_WINDOW_H
