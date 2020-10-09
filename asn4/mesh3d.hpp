#ifndef MESH3D_HPP
#define MESH3D_HPP

#include <glad/glad.h>
#include <string>

// This class implements an array of vertices, with another array of indexes
// that says how to join the vertices into geometric elements.
// Each vertex carries a position (x y z) and a normal direction (nx ny nz)

class Mesh3d {
public:
    // Default constructor, to keep the compiler happy.
    Mesh3d();

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
    float normal_coords[16] = {0,0,1,0,    // normal 0
                               0,0,1,0,    // normal 1
                               0,0,1,0,    // normal 2
                               0,0,1,0};   // normal 3

    // four indexes into the coord array:
    float element_indexes[4] = {0,         // vertex 0 is the fan apex, and
                                1, 2, 3};  // other vertices are fan boundary
    Mesh3d *mesh = new Mesh3d(vertex_coords, normal_coords, 4, 4,
                              element_indexes, 4,
                              4,               // 4 indexes make up 2 triangles
                              GL_TRIANGLE_FAN) // how to join up the vertices
    */
    Mesh3d(float *vertices, float *normals, int n_vertices, int dimension,
          GLuint *indices, int n_indices,
          int num_elements,
          GLuint draw_mode,
          const std::string& mesh_name="NO NAME");

    // Destructor
    ~Mesh3d();

    void update_vertex(float *coords, int index);

    // Tell openGL to draw the mesh.
    void draw();

    // Convenience function to append a vertex's info to its arrays.
    static int add_vertex_normal(float x, float y, float z,
                                 float nx, float ny, float nz,
                                 float *coords, float *normals,
                                 int vert_index);

    // Mesh's name (optional, for debugging)
    std::string name;

private:
    GLuint vertex_array_buffer_handle;
    GLuint vertex_buffer_handle;
    GLuint normal_buffer_handle;
    GLuint element_buffer_handle;
    GLuint draw_mode;
    GLsizei num_elements;
    int dimension;
};

#endif
