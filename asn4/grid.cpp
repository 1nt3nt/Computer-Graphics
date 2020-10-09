#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate
#include <iostream>

#include "grid.hpp"
#include "gl_error.hpp"

extern void print_vec3(const glm::vec3& v);
extern void print_vec4(const glm::vec4& v);
extern void print_mat4(const glm::mat4& m);

Grid::Grid() {
}

Grid::Grid(float BLx, float BLy,
           float width, float height,
           float major_spacing,
           float minor_spacing,
           Diffuse_Lighter& shader) {
    this->BLx = BLx;
    this->BLy = BLy;
    this->width = width;
    this->height = height;
    this->major_spacing = major_spacing;
    this->minor_spacing = minor_spacing;
    this->lighter = &shader;

    make_line_meshes();
}

void Grid::make_line_meshes() {

    GLuint *line_indexes = new GLuint[2];
    line_indexes[0] = 0;
    line_indexes[1] = 1;

    float *line_coords = new float[8];
    float *normals = new float[8];

    int i = 0;
    i = Mesh3d::add_vertex_normal(BLx - 1, 0, 0,  0, 0, 1,
                                  line_coords, normals, i);
    i = Mesh3d::add_vertex_normal(BLx + width + 1, 0, 0,  0, 0, 1,
                                  line_coords, normals, i);
    h_line = new Mesh3d(line_coords, normals, 2, 4,
                        line_indexes, 2, 2,
                        GL_LINES, "Horizontal Line");


    i = 0;
    i = Mesh3d::add_vertex_normal(0, BLy - 1, 0,  0, 0, 1,
                                  line_coords, normals, i);
    i = Mesh3d::add_vertex_normal(0, BLy + height + 1, 0,  0, 0, 1,
                                  line_coords, normals, i);
    v_line = new Mesh3d(line_coords, normals, 2, 4,
                        line_indexes, 2, 2,
                        GL_LINES, "Vertical Line");

    delete[] line_coords;
    delete[] normals;
    delete[] line_indexes;
}

void Grid::draw() {
    float left   = BLx - major_spacing;
    float right  = BLx + width + major_spacing;
    float bottom = BLy - major_spacing;
    float top    = BLy + height + major_spacing;

    glm::vec4 minor_color(0.3, 0.2, 0.0, 1); // dark grey-orange
    glm::vec4 major_color(0.5, 0.4, 0.0, 1); // middle grey-orange
    glm::vec4 axis_color(0.9, 0.9, 0.9, 1);  // bright grey

    lighter->set_color(minor_color);

    // the minor vertical grid lines
    for (float x = left; x <= right; x += minor_spacing)
    {
        glm::mat4 move = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0, 0));
        lighter->set_model_transform(move);
        v_line->draw();
    }

    // the minor horizontal grid lines
    for (float y = bottom; y <= top; y += minor_spacing)
    {
        glm::mat4 move = glm::translate(glm::mat4(1.0f), glm::vec3(0, y, 0));
        lighter->set_model_transform(move);
        h_line->draw();
    }

    // the major vertical grid lines
    lighter->set_color(major_color);

    for (float x = left; x <= right; x += major_spacing)
    {
        glm::mat4 move = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0, 0));
        lighter->set_model_transform(move);
        v_line->draw();
    }

    // the major horizontal grid lines
    for (float y = bottom; y <= top; y += major_spacing)
    {
        glm::mat4 move = glm::translate(glm::mat4(1.0f), glm::vec3(0, y, 0));
        lighter->set_model_transform(move);
        h_line->draw();
    }

    // the coordinate axes
    lighter->set_color(axis_color);

    glm::mat4 move = glm::mat4(1.0f);
    lighter->set_model_transform(move);

    h_line->draw();
    v_line->draw();
}
