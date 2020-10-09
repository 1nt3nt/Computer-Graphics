#include <vector>
#include <glad/glad.h>
#include <glm/gtc/constants.hpp> // glm::pi()
#include <glm/trigonometric.hpp> // glm::sin, cos
#include <glm/gtx/transform.hpp> // translate, rotate, scale
#include <string.h>
#include <iostream>

#include "shapes.hpp"

extern void print_mat4(const glm::mat4& m);

int add_triangle(float *A, float *B, float *C, float *N,
                 float *coords, float *normals,
                 GLuint *element_indexes, int vertex_index) {
    // Convenience function that adds one triangle to the
    // coordinate, normals, and element indexes, for
    // a GL_TRIANGLES mesh.

    int i = vertex_index / 4;

    vertex_index = Mesh3d::add_vertex_normal(A[0], A[1], A[2],
                                             N[0], N[1], N[2],
                                             coords, normals,
                                             vertex_index);
    element_indexes[i++] = i;

    vertex_index = Mesh3d::add_vertex_normal(B[0], B[1], B[2],
                                             N[0], N[1], N[2],
                                             coords, normals,
                                             vertex_index);
    element_indexes[i++] = i;

    vertex_index = Mesh3d::add_vertex_normal(C[0], C[1], C[2],
                                             N[0], N[1], N[2],
                                             coords, normals,
                                             vertex_index);
    element_indexes[i++] = i;

    return vertex_index;
}

Shapes::Shapes() {
    meshes_allocated = false;
}

void Shapes::init() {
    my_sphere = sphere(8, 16);
    my_pyramid = pyramid();
    my_cube = cube();
    meshes_allocated = true;
}

std::vector<Mesh3d*> Shapes::cube() {
    // Return an axis-aligned cube with corners
    // at (-1 -1 -1) and (+1 +1 +1)
    // It consists of two Mesh3d'es: one is a triang strip for the sides
    // of the cube, and another is 2 quads, for the top and bottom.
    // Each vertex has an associated normal, which points out from
    // the cube's center.

    std::vector<Mesh3d*> meshes;
    float *coords = new float[24 * 4];
    float *normals = new float[24 * 4];
    GLuint *indexes = new GLuint[36];

    int i = 0;

    i = Mesh3d::add_vertex_normal(-1,-1,-1, 0,0,-1, coords, normals, i);  // 0
    i = Mesh3d::add_vertex_normal(+1,-1,-1, 0,0,-1, coords, normals, i);  // 1
    i = Mesh3d::add_vertex_normal(+1,+1,-1, 0,0,-1, coords, normals, i);  // 2
    i = Mesh3d::add_vertex_normal(-1,+1,-1, 0,0,-1, coords, normals, i);  // 3

    i = Mesh3d::add_vertex_normal(-1,-1,-1, 0,-1,0, coords, normals, i);  // 4
    i = Mesh3d::add_vertex_normal(+1,-1,-1, 0,-1,0, coords, normals, i);  // 5
    i = Mesh3d::add_vertex_normal(+1,-1,+1, 0,-1,0, coords, normals, i);  // 6
    i = Mesh3d::add_vertex_normal(-1,-1,+1, 0,-1,0, coords, normals, i);  // 7

    i = Mesh3d::add_vertex_normal(-1,-1,-1,-1,0,0, coords, normals, i);  // 8
    i = Mesh3d::add_vertex_normal(-1,-1,+1,-1,0,0, coords, normals, i);  // 9
    i = Mesh3d::add_vertex_normal(-1,+1,+1,-1,0,0, coords, normals, i);  // 10
    i = Mesh3d::add_vertex_normal(-1,+1,-1,-1,0,0, coords, normals, i);  // 11

    i = Mesh3d::add_vertex_normal(-1,+1,-1, 0,1,0, coords, normals, i);  // 12
    i = Mesh3d::add_vertex_normal(+1,+1,-1, 0,1,0, coords, normals, i);  // 13
    i = Mesh3d::add_vertex_normal(+1,+1,+1, 0,1,0, coords, normals, i);  // 14
    i = Mesh3d::add_vertex_normal(-1,+1,+1, 0,1,0, coords, normals, i);  // 15

    i = Mesh3d::add_vertex_normal(+1,+1,-1, 1,0,0, coords, normals, i);  // 16
    i = Mesh3d::add_vertex_normal(+1,-1,-1, 1,0,0, coords, normals, i);  // 17
    i = Mesh3d::add_vertex_normal(+1,-1,+1, 1,0,0, coords, normals, i);  // 18
    i = Mesh3d::add_vertex_normal(+1,+1,+1, 1,0,0, coords, normals, i);  // 19

    i = Mesh3d::add_vertex_normal(-1,-1,+1, 0,0,1, coords, normals, i);  // 20
    i = Mesh3d::add_vertex_normal(+1,-1,+1, 0,0,1, coords, normals, i);  // 21
    i = Mesh3d::add_vertex_normal(+1,+1,+1, 0,0,1, coords, normals, i);  // 22
    i = Mesh3d::add_vertex_normal(-1,+1,+1, 0,0,1, coords, normals, i);  // 23

    i = 0;
    indexes[i++] = 0;
    indexes[i++] = 1;
    indexes[i++] = 2;
    indexes[i++] = 0;
    indexes[i++] = 2;
    indexes[i++] = 3;

    indexes[i++] = 4;
    indexes[i++] = 5;
    indexes[i++] = 6;
    indexes[i++] = 4;
    indexes[i++] = 6;
    indexes[i++] = 7;

    indexes[i++] = 8;
    indexes[i++] = 9;
    indexes[i++] = 10;
    indexes[i++] = 8;
    indexes[i++] = 10;
    indexes[i++] = 11;

    indexes[i++] = 12;
    indexes[i++] = 13;
    indexes[i++] = 14;
    indexes[i++] = 12;
    indexes[i++] = 14;
    indexes[i++] = 15;

    indexes[i++] = 16;
    indexes[i++] = 17;
    indexes[i++] = 18;
    indexes[i++] = 16;
    indexes[i++] = 18;
    indexes[i++] = 19;

    indexes[i++] = 20;
    indexes[i++] = 21;
    indexes[i++] = 22;
    indexes[i++] = 20;
    indexes[i++] = 22;
    indexes[i++] = 23;

    Mesh3d *a_cube = new Mesh3d(coords, normals, 24, 4,
                                indexes, 36, 36,
                                GL_TRIANGLES, "Cube");

    meshes.push_back(a_cube);

    delete[] coords;
    delete[] normals;
    delete[] indexes;

    return meshes;
}

std::vector<Mesh3d*> Shapes::pyramid() {
    // A square pyramid.
    // Its base is on the xy plane, and the apex is on the z axis.

    std::vector<Mesh3d*> meshes;
    int n_vertices = 18;
    int n_triangles = 6;
    float *coords = new float[n_vertices * 4];
    float *normals = new float[n_vertices * 4];
    GLuint *triang_indexes = new GLuint[n_triangles * 3];

    // The vertices.  A is the apex, and
    // B,C,D,E make up the square base.

    float A[3] = { 0,  0,  1}; // 0
    float B[3] = {-1, -1,  0}; // 1
    float C[3] = {+1, -1,  0}; // 2
    float D[3] = {+1, +1,  0}; // 3
    float E[3] = {-1, +1,  0}; // 4

    float r2 = sqrt(0.5f);
    float Nabc[3]  = {  0,-r2, r2};
    float Nacd[3]  = { r2,  0, r2};
    float Nade[3]  = {  0, r2, r2};
    float Naeb[3]  = {-r2,  0, r2};
    float Nbase[3] = {  0,  0, -1};


    int i = 0;
    i = add_triangle(A, B, C, Nabc, coords, normals, triang_indexes, i);
    i = add_triangle(A, C, D, Nacd, coords, normals, triang_indexes, i);
    i = add_triangle(A, D, E, Nade, coords, normals, triang_indexes, i);
    i = add_triangle(A, E, B, Naeb, coords, normals, triang_indexes, i);

    i = add_triangle(B, C, D, Nbase, coords, normals, triang_indexes, i);
    i = add_triangle(B, D, E, Nbase, coords, normals, triang_indexes, i);

    Mesh3d *pyr =new Mesh3d(coords, normals, n_vertices, 4,
                            triang_indexes, n_triangles * 3,
                            n_triangles * 3,
                            GL_TRIANGLES, "Pyramid");
    meshes.push_back(pyr);

    delete[] coords;
    delete[] normals;
    delete[] triang_indexes;

    return meshes;
}

int add_sphere_vertex(float phi, float theta,
                      float *coords, float *normals,
                      int vertex_index) {
    // Convenience function to add a vertex, and its normal,
    // to the coordinates and normals.

    // std::cout << "sphere vert. phi: " << phi << " theta: " << theta << "\n";

    float x = glm::sin(theta) * glm::cos(phi);
    float y = glm::sin(theta) * glm::sin(phi);
    float z = glm::cos(theta);

    // std::cout << "x y z: " << x << " " << y << " " << z << "\n";

    int new_index = Mesh3d::add_vertex_normal(x, y, z,
                                              x, y, z,
                                              coords, normals,
                                              vertex_index);

    // std::cout << "After adding vertex. Vertices:\n";
    // for (int i = 0; i < vertex_index; i++) {
    //     int j = i * 4;
    //     std::cout << "vert[" << i << "] ("
    //               << coords[j] << " " << coords[j+1] << " " << coords[j+2]
    //               << ") N("
    //               << normals[j] << " " << normals[j+1] << " " << normals[j+2]
    //               << ")\n";
    // }

    return new_index;
}

std::vector<Mesh3d*> Shapes::sphere(int n_latitude,
                                    int n_longitude) {
    // A unit-radius sphere, made up of triangle strips.

    std::vector<Mesh3d*> meshes;
    int n_vertices = n_longitude * (n_latitude + 1);
    float *coords = new float[n_vertices * 4];
    float *normals = new float[n_vertices * 4];

    int vertex_index;
    float theta = 0;
    float phi = 0;
    float d_theta = glm::pi<float>() / n_latitude;
    float d_phi = 2 * glm::pi<float>() / n_longitude;

    // std::cout << "theta: " << theta << " n_latitude: " << n_latitude
    //           << " n_longitude: " << n_longitude << "\n";
    // std::cout << "dtheta: " << d_theta << " d_phi: " << d_phi << "\n";
    // std::cout << "ALLOCATED " << n_vertices * dimension << " floats\n";

    vertex_index = 0;
    for (int i_lat = 0; i_lat <= n_latitude; i_lat++) {
        phi = 0;
        for (int i_lon = 0; i_lon < n_longitude; i_lon++) {
            vertex_index = add_sphere_vertex(phi, theta,
                                             coords, normals,
                                             vertex_index);
            // std::cout << "vertex_index: " << vertex_index << "\n";

            phi += d_phi;
        }
        theta += d_theta;
    }

    // std::cout << "Sphere. Vertices:\n";
    // for (int i = 0; i < n_vertices; i++) {
    //     int j = i * 4;
    //     std::cout << "vert[" << i << "] ("
    //               << coords[j] << " " << coords[j+1] << " " << coords[j+2]
    //               << ") N("
    //               << normals[j] << " " << normals[j+1] << " " << normals[j+2]
    //               << ")\n";
    // }

    int n_strip_indexes = (n_longitude + 1) * 2;
    GLuint *strip_indexes = new GLuint[n_strip_indexes];
    for (int i_lat = 0; i_lat <= n_latitude; i_lat++) {
        int bottom_strip_index = i_lat * n_longitude;
        int i_index = 0;
        for (int i_lon = 0; i_lon <= n_longitude; i_lon++) {
            strip_indexes[i_index++] = bottom_strip_index + i_lon;
            strip_indexes[i_index++] = bottom_strip_index + i_lon + n_longitude;
        }

        // std::cout << "Strip #" << i_lat << "\n";
        // for (int i = 0; i < n_strip_indexes; i++) {
        //     std::cout << "index[" << i << "] (" << strip_indexes[i] << ")\n";
        // }

        Mesh3d *strip = new Mesh3d(coords, normals, n_vertices, 4,
                                   strip_indexes, n_strip_indexes,
                                   n_strip_indexes,
                                   GL_TRIANGLE_STRIP, "Sphere");
        meshes.push_back(strip);
    }

    delete[] coords;
    delete[] normals;
    delete[] strip_indexes;

    return meshes;
}

std::vector<Mesh3d*> Shapes::cylinder(int n_slats) {
    // A cylinder with radius 1, extending from -1 to +1 on the z axis

    std::vector<Mesh3d*> meshes;

    int n_vertices = n_slats + 1; //for one side
    float* coords = new float[n_vertices * 9];
    float* normals = new float[n_vertices * 9];
    float d_phi = 2 * glm::pi<float>() / n_slats;
    GLuint* fan_indexes = new GLuint[n_vertices * 2];
    int d = 0;
    float x, y;

    // z = 1 top
    for (int i = 0; i < n_vertices; i++)
    {
        x = glm::cos(i * d_phi);
        y = glm::sin(i * d_phi);
        fan_indexes[i] = d / 4;
        d = Mesh3d::add_vertex_normal(x, y, 1, 0, 0, 1, coords, normals, d);
    }
    Mesh3d* triangleFan = new Mesh3d(coords, normals, n_vertices, 4,
        fan_indexes, n_vertices, n_vertices,
        GL_TRIANGLE_FAN, "Circle_Top");
    meshes.push_back(triangleFan);

    // z = -1 bottom
    int d2 = 0;
    float* coords2 = new float[n_vertices * 6];
    float* normals2 = new float[n_vertices * 6];
    for (int i = 0; i < n_vertices; i++)
    {
        fan_indexes[i] = d2 / 4;
        x = glm::cos(i * d_phi);
        y = glm::sin(i * d_phi);
        d = Mesh3d::add_vertex_normal(x, y, -1, 0, 0, -1, coords, normals, d); // for rounded to access those vertices
        d2 = Mesh3d::add_vertex_normal(x, y, -1, 0, 0, -1, coords2, normals2, d2); // only for drawing bot circle.
    }
    Mesh3d* triangleFan2 = new Mesh3d(coords2, normals2, n_vertices, 4,
        fan_indexes, n_vertices, n_vertices,
        GL_TRIANGLE_FAN, "Circle_Bot");
    meshes.push_back(triangleFan2);

    // Rounded side
    int roundedVertex = 2 * n_vertices;
    GLuint* strip_indexes = new GLuint[roundedVertex];
    int n = 0;
    for (int j = 0; j < n_vertices; j++)
    {
        strip_indexes[n++] = j;
        strip_indexes[n++] = j + n_vertices;
    }
    Mesh3d* roundSide = new Mesh3d(coords, normals, roundedVertex, 4,
        strip_indexes, roundedVertex, roundedVertex,
        GL_TRIANGLE_STRIP, "Rounded");
    meshes.push_back(roundSide);


    delete[] coords;
    delete[] coords2;
    delete[] normals;
    delete[] normals2;
    delete[] fan_indexes;
    delete[] strip_indexes;
    return meshes;
}

