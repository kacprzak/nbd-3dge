#ifndef GFX_NODE_H
#define GFX_NODE_H

#include "Aabb.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <algorithm>
#include <memory>

namespace gfx {

class Model;
class Camera;
class Light;

class Node final
{
  public:
    explicit Node();

    Node(const Node&) = default;
    Node& operator=(const Node&) = default;

    Node(Node&& other) = default;

    ~Node() = default;

    void setTranslation(glm::vec3 translation) { m_translation = translation; }
    glm::vec3 getTranslation() const { return m_translation; }

    void setRotation(glm::quat rotation) { m_rotation = rotation; }
    glm::quat getRotation() const { return m_rotation; }

    void setScale(glm::vec3 scale) { m_scale = scale; }
    glm::vec3 getScale() const { return m_scale; }

    void setModelMatrix(glm::mat4 mtx)
    {
        m_modelMatrix = mtx;

        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(mtx, m_scale, m_rotation, m_translation, skew, perspective);
    }

    void draw(const glm::mat4& parentModelMatrix, ShaderProgram* shaderProgram,
              std::array<Light*, 8>& lights) const;

    void drawAabb(const glm::mat4& parentModelMatrix, ShaderProgram* shaderProgram) const;

    void update(const glm::mat4& parentModelMatrix, float delta);

    Aabb aabb() const;

    void setCastShadows(bool castsShadows) { m_castsShadows = castsShadows; }
    bool castsShadows() const { return m_castsShadows; }

    void addChild(int node) { m_children.push_back(node); }

    void removeChild(int node)
    {
        m_children.erase(std::remove(m_children.begin(), m_children.end(), node), m_children.end());
    }

    void setModel(Model* model)
    {
        m_model = model;
        //m_mesh = m_camera = m_light = -1;
    }
    Model* getModel() { return m_model; }

    void setMesh(int mesh) { m_mesh = mesh; }
    void removeMesh(int mesh) { m_mesh = -1; }

    void setCamera(int camera) { m_camera = camera; }
    void removeCamera() { m_camera = -1; }

    void setLight(int light) { m_light = light; }
    void removeLight() { m_light = -1; }

    std::string name;

  private:
    const glm::mat4& modelMatrix() const { return m_modelMatrix; }

    void rebuildModelMatrix();

    glm::quat m_rotation{1.f, 0.f, 0.f, 0.f};
    glm::vec3 m_translation{};
    glm::vec3 m_scale{1.0f, 1.0f, 1.0f};

    glm::mat4 m_modelMatrix{1.0f};

    Model* m_model = nullptr;
    int m_mesh     = -1;
    int m_camera   = -1;
    int m_light    = -1;
    std::vector<int> m_children;

    bool m_castsShadows = false;
};

} // namespace gfx

#endif // GFX_NODE_H
