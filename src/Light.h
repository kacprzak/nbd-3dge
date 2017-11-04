#ifndef LIGHT_H
#define LIGHT_H

#include <glm/fwd.hpp>

struct Light
{
    glm::vec4 position{0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec3 ambient{0.2f, 0.2f, 0.2f};
    glm::vec3 diffuse{0.8f, 0.8f, 0.8f};
    glm::vec3 specular{1.0f, 1.0f, 1.0f};
};

#endif /* LIGHT_H */
