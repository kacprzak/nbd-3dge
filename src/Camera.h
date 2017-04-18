#ifndef CAMERA_H
#define CAMERA_H

#include "Actor.h"
#include <glm/glm.hpp>

class Camera : public Actor
{
 public:
    Camera();

    void draw() const {}
    void setPerspective(float angle, float ratio, float near, float far);

    const glm::mat4& viewMatrix() const;
    const glm::mat4& projectionMatrix() const;

 protected:
    void refresh() override;
    
 private:   
    glm::mat4 m_projectionMatrix;
    mutable glm::mat4 m_viewMatrix;
};

#endif // CAMERA_H
