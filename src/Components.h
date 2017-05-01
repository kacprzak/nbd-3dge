#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>

#include <string>
#include <vector>

enum class ComponentId { Transformation, Render };
struct Component {};

struct TransformationComponent : public Component
{
    glm::vec3 position;
    glm::vec4 orientation;
    float scale;
};

enum class Role { Skybox, Terrain, Dynamic, Gui };

struct RenderComponent : public Component
{
    Role role;
    std::string shaderProgram;
    std::string mesh;
    std::vector<std::string> textures;
};



#endif // COMPONENTS_H
