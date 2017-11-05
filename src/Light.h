#ifndef LIGHT_H
#define LIGHT_H

#include <glm/fwd.hpp>

class Light
{
  public:
    void setPosition(const glm::vec4& position) { m_position = position; }
    void setAmbient(const glm::vec3& ambient) { m_ambient = ambient; }
    void setDiffuse(const glm::vec3& diffuse) { m_diffuse = diffuse; }
    void setSpecular(const glm::vec3& specular) { m_specular = specular; }

    glm::vec4 position() const { return m_position; }
    glm::vec3 ambient() const { return m_ambient; }
    glm::vec3 diffuse() const { return m_diffuse; }
    glm::vec3 specular() const { return m_specular; }

  private:
    glm::vec4 m_position{0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec3 m_ambient{0.2f, 0.2f, 0.2f};
    glm::vec3 m_diffuse{0.8f, 0.8f, 0.8f};
    glm::vec3 m_specular{1.0f, 1.0f, 1.0f};
};

#endif /* LIGHT_H */
