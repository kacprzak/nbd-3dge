#ifndef CAMERA_H
#define CAMERA_H

#include "actor.h"
#include <glm/glm.hpp>

class Camera : public Actor
{
public:
    Camera() : Actor("camera") {
        float ratio = 800 / float(600);
        setPerspective(45.0f, ratio, 1.0f, 200.0f);
    }

    void draw() const {}
    void setPerspective(float angle, float ratio, float near, float far);

    const glm::mat4& viewMatrix();
    const glm::mat4& projectionMatrix() const;

private:   
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;
};

#endif // CAMERA_H
