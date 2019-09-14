#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <src/Config.h>
#include "Window.h"
#include "Logger.h"

/**
 * Initialize a new window.
 */
Window::Window() {
  if(!glfwInit())
  {
    Logger::error("Failed to initialize GLFW.");
  }

  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (Config::full_screen) {
    auto monitor = glfwGetPrimaryMonitor();
    auto video_mode = glfwGetVideoMode(monitor);
    handle = glfwCreateWindow(video_mode->width, video_mode->height, "SkyHockey", monitor, nullptr);
  } else {
    handle = glfwCreateWindow(1280, 720, "SkyHockey", nullptr, nullptr);
  }

  if(handle == nullptr){
    Logger::error("Failed to open GLFW window.");
  }
  glfwMakeContextCurrent(handle);

  glfwGetFramebufferSize(handle, &width, &height);

  glfwSetInputMode(handle, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwPollEvents();
  glfwSetCursorPos(handle, width/2, height/2);

  if(!gladLoadGL()) {
    Logger::error("Failed to initialize GLAD.");
  }
}

/**
 * Bind the window as an output.
 */
void Window::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, width, height);
}
