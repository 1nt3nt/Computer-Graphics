#include <string>
#include <iostream>
#include "shader.hpp"
#include "gl_error.hpp"

Shader_Program::Shader_Program(const char *vertex_shader_source,
                               const char *fragment_shader_source) {

    // Install the vertex shader
    GLuint vertex_shader_handle = glCreateShader(GL_VERTEX_SHADER);
    GL_Error::check("CreateShader (vertex shader)");

    // Load the vertex shader's source
    glShaderSource(vertex_shader_handle, 1, &vertex_shader_source, NULL);
    GL_Error::check("ShaderSource (vertex shader)");

    // Compile the vertex shader.
    glCompileShader(vertex_shader_handle);
    GL_Error::check("Compile (vertex shader)");

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader_handle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader_handle, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // install fragment shader
    GLuint fragment_shader_handle = glCreateShader(GL_FRAGMENT_SHADER);
    GL_Error::check("CreateShader (fragment shader)");

    // Load the fragment shader's source
    glShaderSource(fragment_shader_handle, 1, &fragment_shader_source,
                   NULL);
    GL_Error::check("ShaderSource (fragment shader)");

    // Compile the fragment shader.
    glCompileShader(fragment_shader_handle);
    GL_Error::check("Compile (fragment shader)");

    // check for shader compile errors
    glGetShaderiv(fragment_shader_handle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader_handle, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // link the two shaders together.
    shader_program_handle = glCreateProgram();
    GL_Error::check("CreateProgram");

    glAttachShader(shader_program_handle, vertex_shader_handle);
    GL_Error::check("AttachShader (vertex shader)");

    glAttachShader(shader_program_handle, fragment_shader_handle);
    GL_Error::check("AttachShader (fragment shader)");

    glLinkProgram(shader_program_handle);
    GL_Error::check("LinkProgram");

    // check for linking errors
    glGetProgramiv(shader_program_handle, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program_handle, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    // The two shaders are now joined into a Program.
    // Can delete them.
    glDeleteShader(vertex_shader_handle);
    GL_Error::check("DeleteShader (vertex shader)");

    glDeleteShader(fragment_shader_handle);
    GL_Error::check("DeleteShader (fragment shader)");
}

GLuint Shader_Program::get_handle() {
    return shader_program_handle;
}
