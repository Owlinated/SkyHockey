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

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  // Map R(endermode) to switch between forward and deferred rendering
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    Config::forward_rendering ^= 1;
  }

  // Map S(pace) to toggling the fancy background on and off
  if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    Config::fancy_background ^= 1;
  }

  // Map E and D to increasing and decreasing the shadow map blur
  if (key == GLFW_KEY_E && action == GLFW_PRESS && Config::shadow_blur_size < 7) {
    Config::shadow_blur_size++;
    std::cout << "Shadow blur increased to " << Config::shadow_blur_size << std::endl;
  }
  if (key == GLFW_KEY_D && action == GLFW_PRESS && Config::shadow_blur_size > 0) {
    Config::shadow_blur_size--;
    std::cout << "Shadow blur decreased to " << Config::shadow_blur_size << std::endl;
  }

  // Map I and K to increasing and decreasing the number of motion blur samples
  if (key == GLFW_KEY_I && action == GLFW_PRESS) {
    Config::motion_blur_steps++;
    std::cout << "Motion blur steps increased to " << Config::motion_blur_steps << std::endl;
  }
  if (key == GLFW_KEY_K && action == GLFW_PRESS && Config::motion_blur_steps > 0) {
    Config::motion_blur_steps--;
    std::cout << "Motion blur steps decreased to " << Config::motion_blur_steps << std::endl;
  }

  // Map O and L to increasing and decreasing the motion blur step size
  if (key == GLFW_KEY_O && action == GLFW_PRESS) {
    Config::motion_blur_step_size += 0.25f;
    std::cout << "Motion blur step size increased to " << Config::motion_blur_step_size << std::endl;
  }
  if (key == GLFW_KEY_L && action == GLFW_PRESS && Config::motion_blur_step_size > 0) {
    Config::motion_blur_step_size -= 0.25f;
    std::cout << "Motion blur step size decreased to " << Config::motion_blur_step_size << std::endl;
  }

  // Close the window when escape is pressed
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
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
      renderer.renderFrame(game);

      glfwPollEvents();
    } while (!glfwWindowShouldClose(window->handle));
    glfwTerminate();
    return 0;
  } catch (std::exception &exception) {
    std::cerr << exception.what() << std::endl;
    return 1;
  }
}
