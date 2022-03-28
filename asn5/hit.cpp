#include "hit.hpp"

Hit::Hit() {
    p = glm::vec3(0,0,0);
    obj = 0;
    dist = 0;
}

Hit::Hit(const glm::vec3& hitpoint, Object* obj, double d) {
    set(hitpoint, obj, d);
}

void Hit::set(const glm::vec3& hitpoint, Object* obj, double d) {
    p = hitpoint;
    this->obj = obj;
    dist = d;
}