#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
 public:
    Window();
    int width, height;
    GLFWwindow* handle;
};

#endif //ENGINE_WINDOW_H
