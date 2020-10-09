#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <glad/glad.h>

// A wrapper around a vertex shader, plus a fragment shader.
// Hides a lot of the openGL boilerplate from the client program.

class Shader_Program {
public:
    // Constructor.  Pass it two strings, which are
    // the source code for the two shaders.
    Shader_Program(const char *vertex_shader_source,
                   const char *fragment_shader_source);

    // Return openGL object's handle, which is used by
    // the Mesh and Camera classes.
    GLuint get_handle();

private:
    GLuint shader_program_handle;
};

#endif

