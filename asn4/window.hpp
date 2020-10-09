#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// A wrapper around a GLFWwindow object.

class GLFW_OpenGL_Window {
public:
    // Constructor.  Create the window, specifying various attributes:
    // - the shader versions that should be supported by the GPU.
    // - window dimensions and title
    // - a callback for when user resizes the window.
    GLFW_OpenGL_Window(int major_version,
                       int minor_version,
                       const int width,
                       const int height,
                       const GLchar *title,
                       void (*resize_callback)(GLFWwindow*, int, int) );

    // retrieve the GLFWindow object.
    GLFWwindow *get_window();

private:
    GLFWwindow *window;

};

#endif
