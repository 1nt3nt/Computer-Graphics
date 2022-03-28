#if !defined(_TRIANGLE_H_)

#define _TRIANGLE_H_

#include "object.hpp"
#include "hit.hpp"

class Triangle : public virtual Object {
public:
    Triangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
             const glm::vec3& color);
    bool intersects(const glm::vec3& start,
                    const glm::vec3& direction, Hit& hit);

private:
    glm::vec3 v1,v2,v3;
};

#endif
