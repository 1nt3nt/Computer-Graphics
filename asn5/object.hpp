#if !defined(_OBJECT_H_)

#define _OBJECT_H_

#include <glm/vec3.hpp>

#include "hit.hpp"
#include "object.hpp"

enum ObjectType {NO_OBJECT, SPHERE, TRIANGLE};

class Object {
public:
    Object(const glm::vec3& newColor);
    virtual bool intersects(const glm::vec3& start,
                            const glm::vec3& direction, Hit& hit) = 0;
    glm::vec3 getColor() { return color; };

    glm::vec3 color;

};

#endif
