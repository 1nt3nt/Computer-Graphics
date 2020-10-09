#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <glad/glad.h>
#include <vector>

#include "mesh3d.hpp"
#include "diffuse_lighter.hpp"

class Shapes {
public:
    Shapes();
    void init();
    std::vector<Mesh3d*> cube();
    std::vector<Mesh3d*> pyramid();
    std::vector<Mesh3d*> sphere(int n_latitude,
                             int n_longitude);
    std::vector<Mesh3d*> cylinder(int n_slats);

private:
    bool meshes_allocated;
    std::vector<Mesh3d*> my_sphere, my_cube, my_pyramid;
};

#endif

