#include <cstdio>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <src/renderer/material/Shader.h>
#include <src/game/Camera.h>
#include <src/renderer/geometry/ObjLoader.h>
#include <iostream>
#include <src/renderer/Renderer.h>
#include <src/support/Logger.h>
#include <sstream>
#include "Config.h"
#include <OVR_CAPI.h>
#include "support/Oculus.h"

/**
 * OpenGL callback for warnings and errors.
 */
void KHRONOS_APIENTRY messageCallback(GLenum source,
                     GLenum type,
                     GLuint id,
                     GLenum severity,
                     GLsizei length,
                     const GLchar *message,
                     const void *userParam) {
  // OpenGL debug callback - great location for a breakpoint ;)
  if (severity > GL_DEBUG_SEVERITY_NOTIFICATION)
  {
    Logger::warn(message);
  }
}

/**
 * Toggle a boolean value based on a key code.
 * @param value Property to toggle.
 * @param name Name of property to output to console.
 * @param key Current key code.
 * @param toggle_key Key code for toggling property.
 */
void toggle(bool &value, const std::string &name, int key, int toggle_key) {
  if (key == toggle_key) {
    value ^= 1;
    std::stringstream message;
    message << "Toggled " << name << " to " << (value ? "on" : "off") << ".";
    Logger::info(message.str());
  }
}

/**
 * Modify a numeric value based on two key codes.
 * @tparam Numeric Numeric type to update.
 * @param value Value to update.
 * @param name Name of property to output to console.
 * @param key Current key code.
 * @param increase_key Key code for increasing the value.
 * @param decrease_key Key code for decreasing the value.
 * @param min Minimum value for decreasing the property.
 * @param max Maximum value for increasing the property.
 * @param step Step size for increments and decrements.
 */
template <typename Numeric>
void modify(Numeric& value, const std::string &name, int key, int increase_key, int decrease_key,
               Numeric min, Numeric max, Numeric step)
{
  std::stringstream message;
  if (key == increase_key && value < max) {
    value += step;
    message << "Increased " << name << " to " << value << ".";
    Logger::info(message.str());
  } else if (key == decrease_key && value > min) {
    value -= step;
    message << "Decreased " << name << " to " << value << ".";
    Logger::info(message.str());
  }
}

/**
 * Callback on key state change. Applies changes to config.
 * @param window Window on which key event was registered.
 * @param key Key with state change.
 * @param scancode Scancode of change.
 * @param action Registered key action.
 * @param mods Key modifiers.
 */
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

/**
 * Entry point for engine.
 * @param argc Number of arguments.
 * @param argv Arguments pointer.
 * @return Exit code.
 */
int main(int argc, char *argv[]) {
  Config::parse(std::vector<std::string>(argv, argv + argc));

  try {
    // Setup window and debug callback
    auto window = std::make_shared<Window>();
    auto oculus = std::make_shared<Oculus>();
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback((GLDEBUGPROC) messageCallback, nullptr);

    // Setup input callback
    glfwSetKeyCallback(window->handle, keyCallback);
  	
    // Create game and renderer
    Game game(window);
    Renderer renderers[] = {
      Renderer(window, game.camera),
      Renderer(oculus->Left, oculus->Left),
      Renderer(oculus->Right, oculus->Right)
    };

    auto lastFrameTime = glfwGetTime();
    long long frameIndex = 0;
    do {
      auto currentFrame = glfwGetTime();
      auto deltaTime = static_cast<float>(currentFrame - lastFrameTime);
      lastFrameTime = currentFrame;

      // Update and render game
      oculus->WaitToBeginFrame(frameIndex);
      game.update(deltaTime, oculus->active_hand_position);
      if (oculus->request_start_) {
        oculus->request_start_ = false;
        game.request_start = true;
      }

      oculus->BeginFrame(frameIndex);
      for (auto& renderer : renderers)
      {
        renderer.renderFrame(game, deltaTime);
      }
      oculus->EndFrame(frameIndex);
      window->EndFrame();

      ++frameIndex;
      glfwPollEvents();
    } while (glfwWindowShouldClose(window->handle) == GLFW_FALSE);
    glfwTerminate();
    return 0;
  } catch (std::exception &exception) {
    Logger::warn("Fatal: " + std::string(exception.what()));
    return 1;
  }
}
