#ifndef MATERIALDATA_H
#define MATERIALDATA_H

#include <glm/glm.hpp>

#include <string>

struct MaterialData
{
    glm::vec3 ambient{0.2f, 0.2f, 0.2f};
    glm::vec3 diffuse{0.8f, 0.8f, 0.8f};
    glm::vec3 specular;
    glm::vec3 emission;

    int shininess = 0;

    std::string ambientTex;
    std::string diffuseTex;
    std::string specularTex;
    std::string normalsTex;

    std::string name;
};


#endif /* MATERIALDATA_H */
