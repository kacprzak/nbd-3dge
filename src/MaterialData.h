#ifndef MATERIALDATA_H
#define MATERIALDATA_H

#include <glm/glm.hpp>

#include <string>

#include "TextureData.h"

struct MaterialData
{
    glm::vec3 ambient{0.2f, 0.2f, 0.2f};
    glm::vec3 diffuse{0.8f, 0.8f, 0.8f};
    glm::vec3 specular;
    glm::vec3 emission;

    float shininess = 1.0f;

    std::vector<TextureData> textures;

    std::string name;
};

#endif /* MATERIALDATA_H */
