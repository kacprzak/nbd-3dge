#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <string>
#include <vector>

enum class ComponentId { Transformation, Render, Light, Physics, Control, Script };

struct Component
{
};

struct TransformationComponent : public Component
{
    glm::quat rotation{1.f, 0.f, 0.f, 0.f};
    glm::vec3 translation{};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    glm::mat4 toMatrix() const
    {
        const auto T = glm::translate(glm::mat4(1.f), translation);
        const auto R = glm::toMat4(rotation);
        const auto S = glm::scale(glm::mat4(1.f), scale);
        return T * R * S;
    }

    void fromMatrix(const glm::mat4& mtx)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(mtx, scale, rotation, translation, skew, perspective);
    }
};

enum class Role { Skybox, Dynamic };

struct RenderComponent : public Component
{
    Role role                 = Role::Dynamic;
    std::string shaderProgram = "default";
    std::string model;
    bool transparent     = false;
    bool backfaceCulling = true;
};

struct LightComponent : public Component
{
    enum class Type { Directional, Point, Spot };

    Type type         = Type::Directional;
    bool castsShadows = true;
    std::string material;
};

struct PhysicsComponent : public Component
{
    std::string shape;
    float mass         = 0.0f;
    glm::vec3 maxForce = glm::vec3{5000, 5000, 5000};
    glm::vec3 maxTorque{};
    glm::vec3 force{};
    glm::vec3 torque{};
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

    uint16_t actions = 0; //< Buttons
    glm::vec4 axes{};     //< Analog control
};

struct ScriptComponent : public Component
{
    std::string name;
};

#endif // COMPONENTS_H
