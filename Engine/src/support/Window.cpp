#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"

// Whether to start fullscreen
// #define FULLSCREEN

Window::Window() {
  if(!glfwInit())
  {
    throw std::runtime_error("Failed to initialize GLFW.");
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef FULLSCREEN
  auto monitor = glfwGetPrimaryMonitor();
  auto video_mode = glfwGetVideoMode(monitor);
  handle = glfwCreateWindow(video_mode->width, video_mode->height, "AirHockey", monitor, nullptr);
#else
  handle = glfwCreateWindow(1280, 720, "AirHockey", nullptr, nullptr);
#endif

  if(handle == nullptr){
    throw std::runtime_error("Failed to open GLFW window.");
  }
  glfwMakeContextCurrent(handle);

  glfwGetFramebufferSize(handle, &width, &height);

  glfwSetInputMode(handle, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwPollEvents();
  glfwSetCursorPos(handle, width/2, height/2);

  glewExperimental = static_cast<GLboolean>(true);
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("Failed to initialize GLEW.");
  }
}
void Window::bindAsFramebuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, width, height);
}


