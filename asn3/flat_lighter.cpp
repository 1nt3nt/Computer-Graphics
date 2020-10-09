// A wrapper around a simple Shader_Program.
// The program lets you set these uniform values:
//  - modeling transform
//  - camera's projection-view transform
//  - the vertex color
// No lighting is done.  Objects will look "flat".  Hence the name.

#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <iostream>

#include "flat_lighter.hpp"
#include "gl_error.hpp"
#include "shader.hpp"

//////////////////////////
// The vertex shader.
// Apply model-view-projection transform, and set the vertex color.
//////////////////////////

const char *Flat_Lighter::vertex_shader_source = R"(
#version 330 core
in vec4 aPos;
out vec4 vertexColor;

uniform mat4 proj_view;
uniform mat4 transform;
uniform vec4 color;

void main()
{
    gl_Position = proj_view * transform * aPos;
    vertexColor = color;
}
)";

//////////////////////////
// The fragment shader.
// Just pass the vertex color through to fragment.
//////////////////////////

const char *Flat_Lighter::fragment_shader_source = R"(
#version 330 core
in vec4 vertexColor;
out vec4 output_color;

void main()
{
    output_color = vertexColor;
}
)";

Flat_Lighter::Flat_Lighter() {
}

void Flat_Lighter::init() {
    install_shader();
}

void Flat_Lighter::install_shader() {
    // Set up the shaders for the vertical and horizontal lines.
    Shader_Program program(vertex_shader_source,
                           fragment_shader_source);
    shader_program_handle = program.get_handle();
    GL_Error::check("Flat_Lighter::install_shader: get program handle");

    // Get handles for the vertex shader's uniform params
    glUseProgram(shader_program_handle);

    transform_uniform_loc = glGetUniformLocation(shader_program_handle,
                                                 "transform");
    GL_Error::check("Flat_Lighter::install_shader: get transform loc");

    proj_view_uniform_loc = glGetUniformLocation(shader_program_handle,
                                                 "proj_view");
    GL_Error::check("Flat_Lighter::install_shader: get proj_view loc");

    color_uniform_loc = glGetUniformLocation(shader_program_handle,
                                             "color");
    GL_Error::check("Flat_Lighter::install_shader: get proj_view loc");

}

void Flat_Lighter::set_model_transform(const glm::mat4& transform) const {
    glUseProgram(shader_program_handle);
    GL_Error::check("Flat_Lighter::set_model_transform: glUseProgram");

    glUniformMatrix4fv(transform_uniform_loc, 1, GL_FALSE,
                       glm::value_ptr(transform));
    GL_Error::check("Flat_Lighter::set_model_transform: set transform uniform");
}

void Flat_Lighter::set_color(const glm::vec4& color) const {
    glUseProgram(shader_program_handle);
    GL_Error::check("Flat_Lighter::set_color: glUseProgram");

    glUniform4fv(color_uniform_loc, 1, glm::value_ptr(color));
    GL_Error::check("Flat_Lighter::set_color: set color uniform");
}

void Flat_Lighter::set_camera(const Camera& cam) const {
    glUseProgram(shader_program_handle);
    GL_Error::check("Flat_Lighter::set_camera: glUseProgram");

    cam.update_shader(shader_program_handle,
                      (GLint) proj_view_uniform_loc);
}

GLuint Flat_Lighter::get_program_handle() const {
    return shader_program_handle;
}
