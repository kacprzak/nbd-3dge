#ifndef ACTOR_H
#define ACTOR_H

#include "mesh.h"
#include "vector.h"
#include <boost/shared_ptr.hpp>
#include "texture.h"

class Actor
{
public:
    Actor(boost::shared_ptr<Mesh> mesh);
    void setTexture(boost::shared_ptr<Texture> tex);

    void move(const Vector<float>& posDelta);
    void moveTo(const Vector<float>& pos);
    Vector<float> position() { return m_position ;}

    void virtual draw() const;
    void virtual update(float delta);

    void setOrientation(float x, float y, float z);
    void rotate(float x, float y, float z);

    void setScale(float s);
    void setScale(float x, float y, float z);

    bool isIdle() { return m_state == Idle; }
    bool isActive() { return m_state == Active; }
    bool isDestroyed() { return m_state == Destroyed; }

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

    Vector<float> m_position;
    Vector<float> m_orientation;
    Vector<float> m_scale;
    float m_color[3];
};

#endif // ACTOR_H
