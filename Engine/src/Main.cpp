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
#include "Config.h"

void messageCallback(GLenum source,
                     GLenum type,
                     GLuint id,
                     GLenum severity,
                     GLsizei length,
                     const GLchar *message,
                     const void *userParam) {
  // OpenGL debug callback - great location for a breakpoint ;)
  std::cerr << message << std::endl;
}

void toggle(bool &value, const std::string &name, int key, int toggle_key) {
  if (key == toggle_key) {
    value ^= 1;
    std::cout << "Toggled " << name << " to " << (value ? "on" : "off") << "." << std::endl;
  }
}

template <typename Numeric>
Numeric modify(Numeric& value, const std::string &name, int key, int increase_key, int decrease_key,
               Numeric min, Numeric max, Numeric step)
{
  if (key == increase_key && value < max) {
    value += step;
    std::cout << "Increased " << name << " to " << value << "." << std::endl;
  } else if (key == decrease_key && value > min) {
    value -= step;
    std::cout << "Decreased " << name << " to " << value << "." << std::endl;
  }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if(action != GLFW_PRESS) {
    return;
  }

  toggle(Config::forward_rendering, "forward rendering", key,
         GLFW_KEY_R);
  toggle(Config::fancy_background, "animated background", key,
         GLFW_KEY_S);
  modify(Config::shadow_blur_size, "shadow blur size", key,
         GLFW_KEY_U, GLFW_KEY_J, 0, 7, 1);
  modify(Config::motion_blur_steps, "motion blur steps", key,
         GLFW_KEY_I, GLFW_KEY_K, 0, 10, 1);
  modify(Config::motion_blur_step_size, "motion blur step size", key,
         GLFW_KEY_O, GLFW_KEY_L, 0.0f, 10.0f, 0.25f);
  toggle(Config::perf_overlay, "performance overlay", key,
         GLFW_KEY_P);
  modify(Config::perf_overlay_scale, "performance overlay scale", key,
         GLFW_KEY_Y, GLFW_KEY_H, 0.0f, 10.0f, 0.25f);
  modify(Config::anti_aliasing_level, "anti aliasing level", key,
         GLFW_KEY_T, GLFW_KEY_G, 0, 4, 1);

  if (key == GLFW_KEY_SPACE) {
    Game::request_start = true;
  }

  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, 1);
  }
}

int main(int argc, char *argv[]) {
  Config::parse(std::vector<std::string>(argv, argv + argc));

  try {
    // Setup window and debug callback
    auto window = std::make_shared<Window>();
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback((GLDEBUGPROC) messageCallback, nullptr);

    // Setup input callback
    glfwSetKeyCallback(window->handle, keyCallback);

    // Create game and renderer
    Renderer renderer(window);
    Game game(window);

    double lastFrameTime = glfwGetTime();
    do {
      auto currentFrame = glfwGetTime();
      auto deltaTime = static_cast<float>(currentFrame - lastFrameTime);
      lastFrameTime = currentFrame;

      // Update and render game
      game.update(deltaTime);
      renderer.renderFrame(game, deltaTime);

      glfwPollEvents();
    } while (glfwWindowShouldClose(window->handle) == GLFW_FALSE);
    glfwTerminate();
    return 0;
  } catch (std::exception &exception) {
    std::cerr << exception.what() << std::endl;
    return 1;
  }
}
