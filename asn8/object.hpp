#if !defined(_OBJECT_H_)

#define _OBJECT_H_

#include "material.hpp"
#include "hit.hpp"

enum ObjectType {NO_OBJECT, SPHERE, TRIANGLE};

class Object {
public:
    Object(const Material& newColor);
    virtual bool intersects(const glm::vec3& start,
                            const glm::vec3& direction,
                            Hit& hit) = 0;
    Material get_material() {return color;};

//#protected:
    Material color;

};

#endif
