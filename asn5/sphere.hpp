#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <glm/vec3.hpp>
#include "object.hpp"
#include "hit.hpp"

class Sphere : public virtual Object {
public:
    Sphere(const glm::vec3& center, float radius,
           const glm::vec3& color);

    bool intersects(const glm::vec3& start,
                    const glm::vec3& direction, Hit& hit);

    glm::vec3 c;
    float r;

};

#endif
