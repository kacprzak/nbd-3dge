#ifndef ACTOR_H
#define ACTOR_H

#include "mesh.h"
#include <boost/shared_ptr.hpp>
#include "texture.h"
#include <glm/glm.hpp>
#include "shaderprogram.h"

class Actor
{
public:
    Actor(boost::shared_ptr<Mesh> mesh);
    void setTexture(boost::shared_ptr<Texture> tex);

    void move(const glm::vec3& posDelta);
    void moveTo(const glm::vec3& pos);
    //Vector3<float> position() { return m_position ;}

    void virtual draw() const;
    void virtual draw(ShaderProgram *program) const;
    void virtual update(float delta);

    void setOrientation(float x, float y, float z);
    void rotate(float x, float y, float z);

    void setScale(float s);
    void setScale(float x, float y, float z);

    bool isIdle() { return m_state == Idle; }
    bool isActive() { return m_state == Active; }
    bool isDestroyed() { return m_state == Destroyed; }

    const glm::mat4& modelMatrix() { return m_modelMatrix; }

protected:
    Actor();

    enum State {
        Idle,
        Active,
        Destroyed
    };

    void setMesh(boost::shared_ptr<Mesh> mesh);
    void setState(State state) { m_state = state; }

private:
    boost::shared_ptr<Mesh> m_mesh;
    boost::shared_ptr<Texture> m_texture;

    State m_state;
    bool m_hasTexture;

    float m_color[3];
    glm::mat4 m_modelMatrix;
};

#endif // ACTOR_H
