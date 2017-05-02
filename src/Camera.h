#ifndef CAMERA_H
#define CAMERA_H

#include "GfxNode.h"
#include <glm/glm.hpp>

class Camera : public GfxNode
{
 public:
    Camera();
    ~Camera();

    void draw(const Camera* /*c*/) const override {}
    void update(float delta) override;
    void setPerspective(float angle, float ratio, float near, float far);

    const glm::mat4& viewMatrix() const;
    const glm::mat4& projectionMatrix() const;
    
 private:
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;
};

#endif // CAMERA_H
