#include "RenderSystem.h"

#include "ResourcesMgr.h"
#include "Terrain.h"

#include <array>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

RenderSystem::RenderSystem()
{
    m_camera                             = std::make_shared<Camera>();
    m_camera->transformation()->position = {-3.5f, 11.0f, 3.9f};
    setCamera(m_camera);
}

RenderSystem::~RenderSystem() {}

//------------------------------------------------------------------------------

void RenderSystem::loadCommonResources(const ResourcesMgr& resourcesMgr)
{
    m_normalsShader = resourcesMgr.getShaderProgram("normals");

    auto guiFont   = resourcesMgr.getFont("ubuntu");
    auto guiShader = resourcesMgr.getShaderProgram("font");

    auto text = std::make_shared<Text>(guiFont);
    text->setShaderProgram(guiShader);
    add(text);
    m_fpsCounter.setText(text);

    m_cameraText = std::make_shared<Text>(guiFont);
    m_cameraText->setShaderProgram(guiShader);
    m_cameraText->setPosition({0.5f, 0.f, 0.0f});
    add(m_cameraText);
}

//------------------------------------------------------------------------------

void RenderSystem::addActor(int id, TransformationComponent* tr, RenderComponent* rd,
                            const ResourcesMgr& resourcesMgr)
{
    std::shared_ptr<RenderNode> a;
    if (rd->role == Role::Dynamic) {
        if (!rd->mesh.empty()) {
            if (boost::starts_with(rd->mesh, "heightfield:")) {
                a = std::make_shared<Terrain>(id, tr, rd, *resourcesMgr.getHeightfield(rd->mesh));
            } else {
                a            = std::make_shared<RenderNode>(id, tr, rd);
                auto meshPtr = resourcesMgr.getMesh(rd->mesh);
                a->setMesh(meshPtr);
            }
        }
    } else if (rd->role == Role::Skybox) {
        auto skybox = std::make_shared<Skybox>(resourcesMgr.getTexture(rd->textures[0]));
        skybox->setShaderProgram(resourcesMgr.getShaderProgram(rd->shaderProgram));
        setSkybox(skybox);
        return;
    }

    for (const auto& texture : rd->textures) {
        auto texturePtr = resourcesMgr.getTexture(texture);
        a->addTexture(texturePtr);
    }

    a->setShaderProgram(resourcesMgr.getShaderProgram(rd->shaderProgram));

    if (!rd->transparent)
        add(id, a);
    else
        addTransparent(id, a);
}

void RenderSystem::removeActor(int id)
{
    remove(id);
    removeTransparent(id);
}

//------------------------------------------------------------------------------

void RenderSystem::update(float delta)
{
    using namespace boost;

    m_fpsCounter.update(delta);

    if (m_camera) {
        m_camera->update(delta);

        auto p = m_camera->transformation()->position;
        auto r = glm::eulerAngles(m_camera->transformation()->orientation) * 180.f / 3.14f;

        std::array<char, 64> buffer;
        std::fill(std::begin(buffer), std::end(buffer), '\0');
        iostreams::array_sink sink{buffer.data(), buffer.size()};
        iostreams::stream<iostreams::array_sink> oss{sink};
        oss.precision(1);
        oss.setf(std::ios::fixed, std::ios::floatfield);
        oss << "Cam pos: " << p.x << ' ' << p.y << ' ' << p.z;
        oss.precision(0);
        oss << "    Cam rot: " << r.x << ' ' << r.y << ' ' << r.z;
        buffer[buffer.size() - 1] = '\0';
        m_cameraText->setText(buffer.data());
    }

    for (const auto& node : m_nodes) {
        node.second->update(delta);
    }

    for (const auto& node : m_transparentNodes) {
        node.second->update(delta);
    }
}

//------------------------------------------------------------------------------

void RenderSystem::draw()
{
    draw(m_camera.get());

    if (m_drawNormals) {
        draw(m_normalsShader.get(), m_camera.get());
    }
}

void RenderSystem::draw(const Camera* camera) const
{
    if (m_polygonMode != GL_FILL) glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode);
    if (m_skybox) m_skybox->draw(camera);
    if (m_camera) m_camera->draw(camera);

    for (const auto& node : m_nodes) {
        node.second->draw(camera);
    }

    //glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);

    for (const auto& node : m_transparentNodes) {
        node.second->draw(camera);
    }

    glDepthMask(GL_TRUE);
    //glEnable(GL_CULL_FACE);
    
    if (m_polygonMode != GL_FILL) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    for (const auto& node : m_texts) {
        node->draw();
    }
}

void RenderSystem::draw(ShaderProgram* shaderProgram, const Camera* camera) const
{
    // Used for drawing normals
    for (const auto& node : m_nodes) {
        node.second->draw(shaderProgram, camera);
    }
}

//------------------------------------------------------------------------------

void RenderSystem::setDrawNormals(bool enable, float normalLength)
{
    m_normalsShader->use();
    m_normalsShader->setUniform("magnitude", normalLength);
    m_drawNormals = enable;
    glUseProgram(0);
}

//------------------------------------------------------------------------------

void RenderSystem::setNextPolygonMode()
{
    const std::array<GLenum, 3> modes{{GL_FILL, GL_LINE, GL_POINT}};
    static size_t curr = 0;
    ++curr %= modes.size();
    m_polygonMode = modes[curr];
}
