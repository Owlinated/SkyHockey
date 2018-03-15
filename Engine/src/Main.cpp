#include <cstdio>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <src/renderer/material/Shader.h>
#include <src/game/Camera.h>
#include <src/renderer/geometry/ObjLoader.h>
#include <iostream>
#include <src/renderer/Renderer.h>

// OpenGL messages
void MessageCallback(GLenum source, GLenum type, GLuint id,
                     GLenum severity, GLsizei length,
                     const GLchar* message, const void* userParam ) {
  std::cerr << message << std::endl;
}

int main() {
  auto window = Window::getInstance();
  Renderer renderer;
  Game game;

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC)MessageCallback, nullptr);

  double lastFrameTime = glfwGetTime();

  while (glfwGetKey(window.handle(), GLFW_KEY_ESCAPE) != GLFW_PRESS
      && glfwWindowShouldClose(window.handle()) == 0) {
    // Get elapsed time
    auto currentFrame = glfwGetTime();
    auto deltaTime = static_cast<float>(currentFrame - lastFrameTime);
    lastFrameTime = currentFrame;

    game.update(deltaTime);

    renderer.renderFrame(game);

    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
