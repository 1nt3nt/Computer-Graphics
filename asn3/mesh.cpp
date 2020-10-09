#include <string>
#include <iostream>

#include "mesh.hpp"
#include "gl_error.hpp"

Mesh::Mesh() {
}

Mesh::Mesh(float *vertices, int n_vertices, int dimension,
           GLuint *indices, int n_indices,
           int num_elements,
           GLuint draw_mode,
           GLuint shader_program_handle,
           const std::string& mesh_name) {

    // We'll need these later, in Mesh::draw.
    this->draw_mode = draw_mode;
    this->shader_program_handle = shader_program_handle;
    this->num_elements = num_elements;
    this->dimension = dimension;

    glGenVertexArrays(1, &vertex_array_buffer_handle);
    GL_Error::check("Mesh ctor. GenVertexArrays");

    glGenBuffers(1, &vertex_buffer_handle);
    GL_Error::check("Mesh ctor. GenBuffers (vertex buffer)");

    glGenBuffers(1, &element_buffer_handle);
    GL_Error::check("GenBuffers (element buffer)");
    // bind the Vertex Array Object first, then
    // bind and set vertex buffer(s), and then
    // configure vertex attributes(s).
    glBindVertexArray(vertex_array_buffer_handle);
    GL_Error::check("Mesh ctor. BindVertexArray");

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
    GL_Error::check("Mesh ctor. BindBuffer (vertex buffer)");

    glBufferData(GL_ARRAY_BUFFER,
                 n_vertices * dimension * (int)sizeof(float),
                 vertices, GL_DYNAMIC_DRAW);
    GL_Error::check("Mesh ctor. BufferData (vertices)");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_handle);
    GL_Error::check("Mesh ctor. BindBuffer (element buffer)");

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 n_indices * (int)sizeof(unsigned int),
                 indices, GL_STATIC_DRAW);
    GL_Error::check("Mesh ctor. BufferData (elements)");

    glVertexAttribPointer(0, dimension, GL_FLOAT, GL_FALSE,
                          dimension * (int)sizeof(float),
                          (void*)0);
    GL_Error::check("Mesh ctor. VertexAttribPointer");

    glEnableVertexAttribArray(0);
    GL_Error::check("Mesh ctor. EnableVertexAttribArray");

    // Unbind the array buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_Error::check("Mesh ctor. un-BindBuffer(array buffer)");

    // and unbind the vertex array buffer
    glBindVertexArray(0);
    GL_Error::check("Mesh ctor. un-BindBuffer(vertex array)");
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vertex_array_buffer_handle);
    GL_Error::check("Mesh destructor. DeleteVertexArrays");

    glDeleteBuffers(1, &vertex_buffer_handle);
    GL_Error::check("Mesh destructor. DeleteBuffers (vertex buffer)");

    glDeleteBuffers(1, &element_buffer_handle);
    GL_Error::check("Mesh destructor. DeleteBuffers (element buffer)");
}

void Mesh::update_vertex(float *coords, int index) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
    GL_Error::check("Mesh update_vertex. BindBuffer (vertex buffer)");

    glBufferSubData(GL_ARRAY_BUFFER,
                    index * dimension * (int)sizeof(float),
                    dimension * (int)sizeof(float), coords);
    GL_Error::check("Mesh update_vertex. BufferSubData");
}

void Mesh::draw() {
    glUseProgram(shader_program_handle);
    GL_Error::check("Mesh::draw. UseProgram");

    glBindVertexArray(vertex_array_buffer_handle);
    GL_Error::check("Mesh::draw. BindVertexArray");

    glDrawElements(draw_mode, num_elements, GL_UNSIGNED_INT, 0);
    GL_Error::check("Mesh::draw. DrawElements");
}

