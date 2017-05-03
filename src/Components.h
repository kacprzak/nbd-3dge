#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <vector>

enum class ComponentId { Transformation, Render };
struct Component {};

struct TransformationComponent : public Component
{
    glm::quat orientation;
    glm::vec3 position;
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
