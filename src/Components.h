#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <vector>

enum class ComponentId { Transformation, Render, Physics, Control };
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
    glm::vec3 maxForce = glm::vec3{5000, 5000, 5000};
    glm::vec3 maxMomentum;
    glm::vec3 force;
    glm::vec3 momentum;
};

struct ControlComponent : public Component
{
    enum Action {
        Forward     = (1 << 0),
        Back        = (1 << 1),
        Left        = (1 << 2),
        Right       = (1 << 3),
        Up          = (1 << 4),
        Down        = (1 << 5),
        StrafeLeft  = (1 << 6),
        StrafeRight = (1 << 7),
        Fire        = (1 << 8),
        AltFire     = (1 << 9),
    };

    uint16_t actions = 0;
};

#endif // COMPONENTS_H
