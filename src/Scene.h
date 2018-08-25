#ifndef SCENE_H
#define SCENE_H

#include "Buffer.h"
#include "Camera.h"
#include "Mesh.h"
#include "RenderNode.h"

class Scene
{
  public:
    void load(const std::string& file);

    void update(float delta);
    void draw(ShaderProgram* shaderProgram, const Camera* camera, std::array<Light*, 8>& lights);

    Camera* currentCamera()
    {
        if (m_cameras.empty())
            return nullptr;
        else
            return &m_cameras.at(0);
    }

    Aabb aabb() const;

    RenderNode* findNode(const std::string& node);

  private:
    std::vector<std::shared_ptr<Buffer>> m_buffers;
    std::vector<Accessor> m_accessors;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<Camera> m_cameras;
    std::vector<RenderNode> m_nodes;
    std::vector<RenderNode*> m_scene;
};

#endif /* SCENE_H */
