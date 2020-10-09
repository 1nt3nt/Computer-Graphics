#include <iostream>
#include <ostream>
#include <string>
#include <glad/glad.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>

// Some code to print glm vectors and matrices.
// It's a bit prettier than glm's to_string() function, I think.

const float EPSILON = 0.00001f;

std::string format_number(float x, int width, int precision) {
    // Return a stringified fixed-point version of a number.
    // if the number is close to an int, stringify the int instead.
    if (width > 99)
        width = 99;
    char output[100];
    char format[30];
    int i_x = int(x);
    if (fabs(i_x - x) < EPSILON) {
        sprintf(format, "%%%dd", width);
        sprintf(output, format, i_x);
    }
    else {
        sprintf(format, "%%%d.%df", width, precision);
        sprintf(output, format, x);
    }
    return std::string(output);
}

void out_delimited(std::ostream& os, const glm::vec4& v,
                   char l_delim, char r_delim) {
    os << l_delim << format_number(v.x, 6, 3);
    os << " " << format_number(v.y, 6, 3);
    os << " " << format_number(v.z, 6, 3);
    os << " " << format_number(v.w, 6, 3) << r_delim;
}

void print_vec4(const glm::vec4& v) {
    out_delimited(std::cout, v, '[', ']');
}

void print_vec3(const glm::vec3& v) {
    glm::vec4 v4(v.x, v.y, v.z, 0);
    out_delimited(std::cout, v4, '[', ']');
}

void print_mat4(const glm::mat4& m) {
    out_delimited(std::cout,
                  glm::vec4(m[0][0], m[1][0], m[2][0], m[3][0]), '/', '\\');
    std::cout << "\n";
    out_delimited(std::cout,
                  glm::vec4(m[0][1], m[1][1], m[2][1], m[3][1]), '|', '|');
    std::cout << "\n";
    out_delimited(std::cout,
                  glm::vec4(m[0][2], m[1][2], m[2][2], m[3][2]), '|', '|');
    std::cout << "\n";
    out_delimited(std::cout,
                  glm::vec4(m[0][3], m[1][3], m[2][3], m[3][3]), '\\', '/');
    std::cout << "\n";
}

// void print_vec3(const glm::vec3& v) {
//     std::cout << glm::to_string(v) << "\n";
// }

// void print_vec4(const glm::vec4& v) {
//     std::cout << glm::to_string(v) << "\n";
// }

// void print_mat4(const glm::mat4& m) {
//     std::cout << glm::to_string(m) << "\n";
// }

int add_vertex(float x, float y, float z,
               float *coords, GLuint *indexes, int vert_index) {
    // Add an xyz vertex, and an index, to the data that defines
    // both a triangle fan and a line strip.
    int i = vert_index / 4;
    indexes[i] = (GLuint)i;
    coords[vert_index++] = x;
    coords[vert_index++] = y;
    coords[vert_index++] = z;
    coords[vert_index++] = 1;
    return vert_index;
}

