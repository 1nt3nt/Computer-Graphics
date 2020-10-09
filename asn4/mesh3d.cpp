#include <string>
#include <iostream>

#include "mesh3d.hpp"
#include "gl_error.hpp"

Mesh3d::Mesh3d() {
}

Mesh3d::Mesh3d(float *vertices, float *normals, int n_vertices, int dimension,
             GLuint *indices, int n_indices, int num_elements,
             GLuint draw_mode, const std::string& mesh_name) {

    // We'll need these later, in Mesh3d::draw.
    this->draw_mode = draw_mode;
    this->num_elements = num_elements;
    this->dimension = dimension;

    glGenVertexArrays(1, &vertex_array_buffer_handle);
    GL_Error::check("Mesh3d ctor. GenVertexArrays");

    glGenBuffers(1, &element_buffer_handle);
    GL_Error::check("GenBuffers (element buffer)");
    // bind the Vertex Array Object first, then
    // bind and set vertex buffer(s), and then
    // configure vertex attributes(s).
    glBindVertexArray(vertex_array_buffer_handle);
    GL_Error::check("Mesh3d ctor. BindVertexArray");



    glGenBuffers(1, &vertex_buffer_handle);
    GL_Error::check("Mesh3d ctor. GenBuffers (vertex buffer)");

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
    GL_Error::check("Mesh3d ctor. BindBuffer (vertex buffer)");

    glBufferData(GL_ARRAY_BUFFER,
                 n_vertices * dimension * (int)sizeof(float),
                 vertices, GL_DYNAMIC_DRAW);
    GL_Error::check("Mesh3d ctor. BufferData (vertices)");

    glVertexAttribPointer(0, dimension, GL_FLOAT, GL_FALSE,
                          dimension * (int)sizeof(float),
                          (void*)0);
    GL_Error::check("Mesh3d ctor. VertexAttribPointer (vertices)");

    glEnableVertexAttribArray(0);
    GL_Error::check("Mesh3d ctor. EnableVertexAttribArray (vertices)");



    glGenBuffers(1, &normal_buffer_handle);
    GL_Error::check("Mesh3d ctor. GenBuffers (normal buffer)");

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_handle);
    GL_Error::check("Mesh3d ctor. BindBuffer (normal buffer)");

    glBufferData(GL_ARRAY_BUFFER,
                 n_vertices * dimension * (int)sizeof(float),
                 normals, GL_DYNAMIC_DRAW);
    GL_Error::check("Mesh3d ctor. BufferData (normals)");

    glVertexAttribPointer(1, dimension, GL_FLOAT, GL_FALSE,
                          dimension * (int)sizeof(float),
                          (void*)0);
    GL_Error::check("Mesh3d ctor. VertexAttribPointer (normals)");

    glEnableVertexAttribArray(1);
    GL_Error::check("Mesh3d ctor. EnableVertexAttribArray (normals)");




    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_handle);
    GL_Error::check("Mesh3d ctor. BindBuffer (element buffer)");

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 n_indices * (int)sizeof(unsigned int),
                 indices, GL_STATIC_DRAW);
    GL_Error::check("Mesh3d ctor. BufferData (elements)");


    // Unbind the array buffers for the vertices and normals
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_Error::check("Mesh3d ctor. un-BindBuffer(array buffer for vertices)");

    glBindBuffer(GL_ARRAY_BUFFER, 1);
    GL_Error::check("Mesh3d ctor. un-BindBuffer(array buffer for normals)");

    // and unbind the vertex array buffers
    glBindVertexArray(0);
    GL_Error::check("Mesh3d ctor. un-BindBuffer(vertex array)");
}

Mesh3d::~Mesh3d() {
    glDeleteVertexArrays(1, &vertex_array_buffer_handle);
    GL_Error::check("Mesh3d destructor. DeleteVertexArrays");

    glDeleteBuffers(1, &vertex_buffer_handle);
    GL_Error::check("Mesh3d destructor. DeleteBuffers (vertex buffer)");

    glDeleteBuffers(1, &element_buffer_handle);
    GL_Error::check("Mesh3d destructor. DeleteBuffers (element buffer)");
}

void Mesh3d::update_vertex(float *coords, int index) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
    GL_Error::check("Mesh3d update_vertex. BindBuffer (vertex buffer)");

    glBufferSubData(GL_ARRAY_BUFFER,
                    index * dimension * (int)sizeof(float),
                    dimension * (int)sizeof(float), coords);
    GL_Error::check("Mesh3d update_vertex. BufferSubData");
}

void Mesh3d::draw() {
    glBindVertexArray(vertex_array_buffer_handle);
    GL_Error::check("Mesh3d::draw. BindVertexArray");

    glDrawElements(draw_mode, num_elements, GL_UNSIGNED_INT, 0);
    GL_Error::check("Mesh3d::draw. DrawElements");
}

int Mesh3d::add_vertex_normal(float x, float y, float z,
                              float nx, float ny, float nz,
                              float *coords, float *normals, int vert_index) {
    coords[vert_index] = x;
    normals[vert_index] = nx;
    vert_index++;

    coords[vert_index] = y;
    normals[vert_index] = ny;
    vert_index++;

    coords[vert_index] = z;
    normals[vert_index] = nz;
    vert_index++;

    coords[vert_index] = 1;
    normals[vert_index] = 0;
    vert_index++;

    return vert_index;
}
