#include "RenderSystem.h"

#include "Light.h"
#include "ResourcesMgr.h"
#include "Terrain.h"

#include <array>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

RenderSystem::RenderSystem()
{
    m_shadowMapFB = std::make_unique<Framebuffer>();

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
        auto skybox = std::make_shared<Skybox>(resourcesMgr.getMaterial(rd->material));
        skybox->setShaderProgram(resourcesMgr.getShaderProgram(rd->shaderProgram));
        setSkybox(skybox);
        return;
    } else if (rd->role == Role::Light) {
        auto light = std::make_shared<Light>();
        // todo
        // addLight(skybox);
        return;
    }

    if (!rd->material.empty()) {
        auto materialPtr = resourcesMgr.getMaterial(rd->material);
        a->setMaterial(materialPtr);
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
    std::array<Light*, 8> lights = {};

    Light sun;
    sun.setPosition({1, -1, -1, 0}); // Zero on end changes pos to direction
    sun.setAmbient({1.0, 0.8863, 0.8078});
    sun.setDiffuse({1.0, 0.8863, 0.8078});
    sun.setSpecular({1, 1, 1});

    lights[0] = &sun;

    if (m_shadowMapFB) {
        m_shadowMapFB->bindForWriting();
        glClear(GL_DEPTH_BUFFER_BIT);
        draw(m_camera.get(), lights);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    draw(m_camera.get(), lights);

    if (m_drawNormals) {
        draw(m_normalsShader.get(), m_camera.get(), lights);
    }
}

void RenderSystem::draw(const Camera* camera, std::array<Light*, 8>& lights) const
{
    if (m_polygonMode != GL_FILL) glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode);
    if (m_camera) m_camera->draw(camera, lights, nullptr);

    auto envirnoment = m_skybox->material()->textures.at(0).get();

    for (const auto& node : m_nodes) {
        node.second->draw(camera, lights, envirnoment);
    }

    if (m_polygonMode == GL_FILL && m_skybox) m_skybox->draw(camera);

    glDepthMask(GL_FALSE);
    for (const auto& node : m_transparentNodes) {
        node.second->draw(camera, lights, envirnoment);
    }
    glDepthMask(GL_TRUE);

    if (m_polygonMode != GL_FILL) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // UI
    for (const auto& node : m_texts) {
        node->draw();
    }
}

void RenderSystem::draw(ShaderProgram* shaderProgram, const Camera* camera,
                        std::array<Light*, 8>& lights) const
{
    // Used for drawing normals
    for (const auto& node : m_nodes) {
        node.second->draw(shaderProgram, camera, lights, nullptr);
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
