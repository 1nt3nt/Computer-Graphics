#if !defined(_HIT_H_)
#define _HIT_H_

#include <glm/vec3.hpp>

class Object;

class Hit {
public:
    Hit();
    Hit(const glm::vec3& hitpoint,
        const glm::vec3& normal, Object* obj, double d);
    void set(const glm::vec3& hitpoint,
             const glm::vec3& normal, Object* obj, double d);
    glm::vec3 hitPoint() {return p;};
    glm::vec3 normal() {return N;};
    Object* getObject() {return obj;};
    double getDistance() {return dist;};

    // private:
    glm::vec3 p;
    glm::vec3 N;
    Object* obj;
    double dist;
};

#endif
