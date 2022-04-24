#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <glm/vec3.hpp>

#include "object.hpp"
#include "material.hpp"
#include "hit.hpp"

class Sphere : public virtual Object {
public:
    Sphere(const glm::vec3& center, float radius, const Material& color);
    bool intersects(const glm::vec3& start,
                    const glm::vec3& direction,
                    Hit& hit);

private:
    glm::vec3 center;
    float radius;
};

#endif
