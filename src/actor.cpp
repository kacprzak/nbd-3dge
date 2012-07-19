#include "actor.h"

Actor::Actor(boost::shared_ptr<Mesh> mesh)
    : m_mesh(mesh)
    , m_state(Idle)
    , m_hasTexture(false)
{
    m_scale[0] = 1.0f;
    m_scale[1] = 1.0f;
    m_scale[2] = 1.0f;

    m_color[0] = 1.0f;
    m_color[1] = 1.0f;
    m_color[2] = 1.0f;
}

void Actor::setTexture(boost::shared_ptr<Texture> tex)
{
    m_texture = tex;
    m_hasTexture = true;
}

void Actor::move(const Vector<float> &posDelta)
{
    m_position += posDelta;
}

void Actor::moveTo(const Vector<float> &pos)
{
    m_position = pos;
}

void Actor::draw() const
{
    glPushMatrix();

    glColor3f(m_color[0], m_color[1], m_color[2]);

    glTranslatef(m_position[0], m_position[1], m_position[2]);

    glRotatef(m_orientation[0], 1.0f, 0.0f, 0.0f);
    glRotatef(m_orientation[1], 0.0f, 1.0f, 0.0f);
    glRotatef(m_orientation[2], 0.0f, 0.0f, 1.0f);

    glScalef(m_scale[0], m_scale[1], m_scale[2]);

    if (m_hasTexture) {
        glEnable(GL_TEXTURE_2D);
        m_texture->bind();
    }

    m_mesh->draw();

    if (m_hasTexture) {
        glDisable(GL_TEXTURE_2D);
    }

    glPopMatrix();
}

void Actor::update(float delta)
{
    rotate(0.0f, delta * 100.0f, 0.0f);
}

void Actor::setOrientation(float x, float y, float z)
{
    m_orientation[0] = x;
    m_orientation[1] = y;
    m_orientation[2] = z;
}

void Actor::rotate(float x, float y, float z)
{
    m_orientation[0] += x;
    m_orientation[1] += y;
    m_orientation[2] += z;
}

void Actor::setScale(float s)
{
    m_scale[0] = s;
    m_scale[1] = s;
    m_scale[2] = s;
}

void Actor::setScale(float x, float y, float z)
{
    m_scale[0] = x;
    m_scale[1] = y;
    m_scale[2] = z;
}
