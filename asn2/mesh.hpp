#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>
#include <string>

// This class implements an array of vertices, with another array of indexes
// that says how to join the vertices into geometric elements.

class Mesh {
public:
    // Default constructor, to keep the compiler happy.
    Mesh();

    // An array of vertex coordinates, connected using values from
    // the associated array of indexes.
    //
    // Example: a square:
    //
    /*
    // four 3D points, each of them uses 4 coords (x,y,z,w=1)
    float vertex_coords[16] = {0,0,0,1,    // vertex 0
                               1,0,0,1,    // vertex 1
                               1,1,0,1,    // vertex 2
                               0,1,0,1};   // vertex 3
    // four indexes into the coord array:
    float element_indexes[4] = {0,         // vertex 0 is the fan apex, and
                                1, 2, 3};  // other vertices are fan boundary
    Mesh *mesh = new Mesh(vertex_coords, 4, 4,
                          element_indexes, 4,
                          4,               // 4 indexes make up 2 triangles
                          GL_TRIANGLE_FAN, // how to join up the vertices
                          handle);         // a vertex-fragment program's handle
    */
    Mesh(float *vertices, int n_vertices, int dimension,
         GLuint *indices, int n_indices,
         int num_elements,
         GLuint draw_mode,
         GLuint shader_program_handle,
         const std::string& mesh_name="NO NAME");

    // Destructor
    ~Mesh();

    void update_vertex(float *coords, int index);

    // Tell openGL to draw the mesh.
    void draw();

    // Mesh's name (optional, for debugging)
    std::string name;

private:
    GLuint vertex_array_buffer_handle;
    GLuint vertex_buffer_handle;
    GLuint element_buffer_handle;
    GLuint draw_mode;
    GLuint shader_program_handle;
    GLsizei num_elements;
    int dimension;
};

#endif
