#include "GfxScene.h"

GfxScene::GfxScene()
{
}

GfxScene::~GfxScene()
{
}

void GfxScene::draw(const Camera* camera) const
{
    if (m_skybox)
        m_skybox->draw(camera);

    if (m_camera)
        m_camera->draw(camera);

    for (const auto& a : m_actors) {
        a->draw(camera);
    }

    for (const auto& a : m_texts) {
        a->draw();
    }
}

void GfxScene::draw(ShaderProgram* shaderProgram, const Camera* camera) const
{
    //if (m_skybox)
    //    m_skybox->draw(shaderProgram, camera);

    //if (m_camera)
    //    m_camera->draw(shaderProgram, camera);

    for (const auto& a : m_actors) {
        a->draw(shaderProgram, camera);
    }

    for (const auto& a : m_texts) {
        a->draw();
    }
}

void GfxScene::update(float delta)
{  
    if (m_camera) {
        m_camera->update(delta);
    }

    for (const auto& a : m_actors) {
        a->update(delta);
    }
}
