#ifndef FLAT_LIGHTER_HPP
#define FLAT_LIGHTER_HPP

#include <glad/glad.h>
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4

#include "camera.hpp"

// A wrapper around a simple Shader_Program.
// The program lets you set these uniform values:
//  - modeling transform
//  - camera's projection-view transform
//  - the vertex color
// No lighting is done.  Objects will look "flat".  Hence the name.

class Flat_Lighter {
public:
    // Constructor
    Flat_Lighter();

    // Initializer.  Should be called by client program
    // AFTER constructing the Flat_Lighter object.
    void init();

    // Retrieve the openGL object's handle for the installed program.
    // Use this when you make a Mesh object, and when
    // you ask the Camera object to update the projection-view matrix.
    GLuint get_program_handle() const;

    // Set the transformation matrix in the program
    void set_model_transform(const glm::mat4& transform) const;

    // Set the drawing color in the program
    void set_color(const glm::vec4& color) const;

    // Ask the Camera to copy its projection-view matrix into
    // the program's.
    void set_camera(const Camera& cam) const;

private:
    /////////////////////////
    // handles for accessing the shader
    // and its uniform values
    /////////////////////////
    GLuint shader_program_handle;
    GLint transform_uniform_loc;
    GLint proj_view_uniform_loc;
    GLint color_uniform_loc;

    void install_shader();

    static const char *vertex_shader_source;
    static const char *fragment_shader_source;

};
#endif
