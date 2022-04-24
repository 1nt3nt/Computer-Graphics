#if !defined(_MATERIAL_H_)
#define _MATERIAL_H_

#include <glm/vec3.hpp>

class Material {
public:
    Material();
    Material(const glm::vec3& ambient,
             const glm::vec3& diffuse,
             const glm::vec3& specular,
             int shininess);
    void set(const glm::vec3& ambient,
             const glm::vec3& diffuse,
             const glm::vec3& specular,
             int shininess);
    glm::vec3 get_ambient() const {return ka;};
    glm::vec3 get_diffuse() const {return kd;};
    glm::vec3 get_specular() const {return ks;};
    int get_shininess() const {return n;};

private:
    glm::vec3 ka,kd,ks;
    int n;
};

#endif
