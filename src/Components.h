#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <vector>

enum class ComponentId { Transformation, Render, Physics };
struct Component
{
};

struct TransformationComponent : public Component
{
    glm::quat orientation;
    glm::vec3 position;
    float scale = 1.0f;
};

enum class Role { Skybox, Terrain, Dynamic, Gui };

struct RenderComponent : public Component
{
    Role role;
    std::string shaderProgram = "default";
    std::string mesh;
    std::vector<std::string> textures;
};

struct PhysicsComponent : public Component
{
    std::string shape;
    float mass = 0.0f;
};

#endif // COMPONENTS_H
