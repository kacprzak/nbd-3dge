#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <string>
#include <vector>

enum class ComponentId { Transformation, Render };
struct Component {};

struct TransformationComponent : public Component
{
    glm::vec3 position;
    glm::vec4 orientation;
    float scale;

    void moveForward(float distance)
    {
        glm::vec3 base(0.0f, 0.0f, 1.0f);

        base = glm::rotateX(base, orientation.x);
        base = glm::rotateY(base, orientation.y);
        base = glm::rotateZ(base, orientation.z);

        position += base * distance;
    }
    
    void moveRight(float distance)
    {
        glm::vec3 base(-1.0f, 0.0f, 0.0f);

        base = glm::rotateX(base, orientation.x);
        base = glm::rotateY(base, orientation.y);
        base = glm::rotateZ(base, orientation.z);

        position += base * distance;
    }
    
    void moveLeft(float distance)
    {
        glm::vec3 base(-1.0f, 0.0f, 0.0f);

        base = glm::rotateX(base, orientation.x);
        base = glm::rotateY(base, orientation.y);
        base = glm::rotateZ(base, orientation.z);

        position += base * (-distance);
    }
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
