#include "GfxScene.h"

GfxScene::GfxScene() {}

GfxScene::~GfxScene() {}

void GfxScene::draw(const Camera* camera) const
{
    if (m_polygonMode != GL_FILL)
        glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode);

    if (m_skybox)
        m_skybox->draw(camera);

    if (m_camera)
        m_camera->draw(camera);

    for (const auto& node : m_nodes) {
        node.second->draw(camera);
    }

    if (m_polygonMode != GL_FILL)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (const auto& node : m_texts) {
        node->draw();
    }
}

void GfxScene::draw(ShaderProgram* shaderProgram, const Camera* camera) const
{
    // if (m_skybox)
    //    m_skybox->draw(shaderProgram, camera);

    // if (m_camera)
    //    m_camera->draw(shaderProgram, camera);

    for (const auto& node : m_nodes) {
        node.second->draw(shaderProgram, camera);
    }

    // for (const auto& node : m_texts) {
    //    node->draw();
    //}
}

void GfxScene::update(float delta)
{
    if (m_camera) {
        m_camera->update(delta);
    }

    for (const auto& node : m_nodes) {
        node.second->update(delta);
    }
}

void GfxScene::setNextPolygonMode()
{
    const std::array<GLenum, 3> modes{{GL_FILL, GL_LINE, GL_POINT}};
    static size_t curr = 0;
    ++curr %= modes.size();
    m_polygonMode = modes[curr];
}
