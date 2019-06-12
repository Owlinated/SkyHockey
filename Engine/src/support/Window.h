#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <src/renderer/material/IFramebuffer.h>

/**
 * Holds a window and can be used instead of a framebuffer for output.
 */
class Window : public IFramebuffer {
 public:
  Window();
  int width, height;
  GLFWwindow *handle;
  void bind() override;
  int getWidth() override { return width; }
  int getHeight() override { return height; }
  void EndFrame();
};

#endif //ENGINE_WINDOW_H
