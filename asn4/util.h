#ifndef UTIL_H
#define UTIL_H

#include "GeomLib.h"

class Util {
public:
    static void set_material(const Float4& color);
    static void draw_arrow(const Point4 &p, const Vector4 &v,
                           const Float4 &color,
                           double thickness, char *label);
    static void draw_basis(float length);
    static void init_lights(void);
    static void draw_cube(double side);
    static void draw_sphere(double r, int lats, int longs);
    static void draw_cone(double r, double h, int n_triangs);

    static void draw_cylinder(double r, double h, int n_staves);
    static void draw_torus(double r1, double r2, int n_lat, int n_lon);
    // Some conveniently-named colors
    static Float4 red;
    static Float4 green;
    static Float4 blue;
    static Float4 beige;
    static Float4 purple;
    static Float4 lime;
};

#endif
