#ifndef GFX_NODE_H
#define GFX_NODE_H

#include "../Components.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <algorithm>
#include <memory>

namespace gfx {

class Camera;
class Light;

class Node final
{
  public:
    explicit Node(int actorId, TransformationComponent* tr = nullptr,
                  RenderComponent* rd = nullptr);

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&& other) = default;

    ~Node() = default;

    void setMesh(const std::shared_ptr<Mesh>& mesh);

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
              const Camera* camera, const std::array<Light*, 8>& lights,
              const TexturePack& environment) const;

    void drawAabb(const glm::mat4& parentModelMatrix, ShaderProgram* shaderProgram,
                  const Camera* camera);

    void update(const glm::mat4& parentModelMatrix, float delta);

    TransformationComponent* transformation() { return m_tr; }
    const TransformationComponent* transformation() const { return m_tr; }

    RenderComponent* render() { return m_rd; }
    const RenderComponent* render() const { return m_rd; }

    Aabb aabb() const
    {
        Aabb aabb;

        if (m_mesh) aabb = aabb.mbr(m_modelMatrix * m_mesh->aabb());

        for (auto& n : m_children)
            aabb = aabb.mbr(m_modelMatrix * n->aabb());

        return aabb;
    }

    void setCastShadows(bool castsShadows) { m_castsShadows = castsShadows; }
    bool castsShadows() const { return m_castsShadows; }

    void addChild(Node* node) { m_children.push_back(node); }

    void removeChild(Node* node)
    {
        m_children.erase(std::remove(m_children.begin(), m_children.end(), node), m_children.end());
    }

    void setCamera(Camera* camera) { m_camera = camera; }
    void removeCamera() { m_camera = nullptr; }

    void setLight(Light* light) { m_light = light; }
    void removeLight() { m_light = nullptr; }

    std::string name;

  private:
    const glm::mat4& modelMatrix() const { return m_modelMatrix; }

    const int m_actorId;

    TransformationComponent* m_tr = nullptr;
    RenderComponent* m_rd         = nullptr;

    void rebuildModelMatrix();

    glm::quat m_rotation{1.f, 0.f, 0.f, 0.f};
    glm::vec3 m_translation{};
    glm::vec3 m_scale{1.0f, 1.0f, 1.0f};

    glm::mat4 m_modelMatrix{1.0f};

    std::vector<Node*> m_children;
    Camera* m_camera = nullptr;
    Light* m_light   = nullptr;

    std::shared_ptr<Mesh> m_mesh;

    bool m_castsShadows = false;
};

} // namespace gfx

#endif // GFX_NODE_H