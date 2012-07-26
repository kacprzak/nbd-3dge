#ifndef ACTOR_H
#define ACTOR_H

#include "mesh.h"
#include "texture.h"
#include <glm/glm.hpp>
#include "shaderprogram.h"

class Actor
{
public:
    Actor(const std::string& name, MeshPtr mesh);

    const std::string& name() { return m_name; }

    void setTexture(TexturePtr tex);

    void move(float x, float y = 0.0f, float z = 0.0f);
    void moveTo(float x, float y = 0.0f, float z = 0.0f);

    //Vector3<float> position() { return m_position ;}

    void virtual draw() const;
    void virtual draw(ShaderProgram *program) const;
    void virtual update(float delta);

    void setOrientation(float x, float y, float z);
    void rotate(float x, float y, float z);

    void setScale(float s);

    bool isIdle() { return m_state == Idle; }
    bool isActive() { return m_state == Active; }
    bool isDestroyed() { return m_state == Destroyed; }

    //const glm::mat4& modelMatrix() { return m_modelMatrix; }

protected:
    Actor(const std::string& name);

    enum State {
        Idle,
        Active,
        Destroyed
    };

    void setMesh(MeshPtr mesh);
    void setState(State state) { m_state = state; }

private:
    void rebuildModelMatrix();

    const std::string m_name;

    MeshPtr m_mesh;
    TexturePtr m_texture;

    State m_state;
    bool m_hasTexture;

    glm::vec3 m_position;
    glm::vec3 m_orientation;
    glm::vec3 m_scale;

    glm::mat4 m_modelMatrix;
};

#endif // ACTOR_H
