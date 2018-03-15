#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
 private:
    Window();
    int width_, height_;
    GLFWwindow* handle_;
 public:
    int width() const { return width_; }
    int height() const { return height_; }
    GLFWwindow* handle() const {return handle_; }
    static Window& getInstance();
};

#endif //ENGINE_WINDOW_H
