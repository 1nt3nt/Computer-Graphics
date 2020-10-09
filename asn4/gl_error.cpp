#include <glad/glad.h>
#include <iostream>
#include "gl_error.hpp"

void GL_Error::check(const std::string& msg) {
    GLenum error = glGetError();
    switch (error) {
    case GL_NO_ERROR:
        return;

    case GL_INVALID_ENUM:
        std::cout << "Error: GL_INVALID_ENUM on " << msg << "\n";
        break;

    case GL_INVALID_VALUE:
        std::cout << "Error: GL_INVALID_VALUE on " << msg << "\n";
        break;

    case GL_INVALID_OPERATION:
        std::cout << "Error: GL_INVALID_OPERATION on " << msg << "\n";
        break;

    case GL_INVALID_FRAMEBUFFER_OPERATION:
        std::cout << "Error: GL_INVALID_FRAMEBUFFER_OPERATION on " << msg << "\n";
        break;

    case GL_OUT_OF_MEMORY:
        std::cout << "Error: GL_OUT_OF_MEMORY on " << msg << "\n";
        break;

    case GL_STACK_UNDERFLOW:
        std::cout << "Error: GL_STACK_UNDERFLOW on " << msg << "\n";
        break;

    case GL_STACK_OVERFLOW:
        std::cout << "Error: GL_STACK_OVERFLOW on " << msg << "\n";
        break;

    default:
        std::cout << "Unknown error on " << msg << "\n";
        break;
    }
}
