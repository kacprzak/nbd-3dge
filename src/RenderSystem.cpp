#include "RenderSystem.h"

#include "Framebuffer.h"
#include "Light.h"
#include "ResourcesMgr.h"
#include "Skybox.h"
#include "Terrain.h"

#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

RenderSystem::RenderSystem(glm::ivec2 windowSize)
    : m_windowSize{windowSize}
    , m_shadowMapSize{1024, 1024}
{
    m_shadowMapFB = std::make_unique<Framebuffer>(m_shadowMapSize);

    m_camera                             = std::make_shared<Camera>(m_windowSize);
    m_camera->transformation()->position = {-3.5f, 11.0f, 3.9f};
    setCamera(m_camera);
}

RenderSystem::~RenderSystem() {}

//------------------------------------------------------------------------------

void RenderSystem::loadCommonResources(const ResourcesMgr& resourcesMgr)
{
    m_normalsShader = resourcesMgr.getShaderProgram("normals");
    m_shadowShader  = resourcesMgr.getShaderProgram("shadow");

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
                            LightComponent* lt, const ResourcesMgr& resourcesMgr)
{
    // Render component
    if (rd) {
        if (rd->role == Role::Dynamic) {
            std::shared_ptr<RenderNode> node;
            if (!rd->mesh.empty()) {
                if (boost::starts_with(rd->mesh, "heightfield:")) {
                    node = std::make_shared<Terrain>(id, tr, rd,
                                                     *resourcesMgr.getHeightfield(rd->mesh));
                } else {
                    node         = std::make_shared<RenderNode>(id, tr, rd);
                    auto meshPtr = resourcesMgr.getMesh(rd->mesh);
                    node->setMesh(meshPtr);
                    if (!lt) node->setCastShadows(true);
                }
            }

            if (!rd->material.empty()) {
                auto materialPtr = resourcesMgr.getMaterial(rd->material);
                node->setMaterial(materialPtr);
            }

            node->setShaderProgram(resourcesMgr.getShaderProgram(rd->shaderProgram));
            if (!node->render()->transparent)
                m_nodes[id] = node;
            else
                m_transparentNodes[id] = node;

        } else if (rd->role == Role::Skybox) {
            auto skybox = std::make_shared<Skybox>();

            if (!rd->material.empty()) {
                auto materialPtr = resourcesMgr.getMaterial(rd->material);
                skybox->setMaterial(materialPtr);
            }
            skybox->setShaderProgram(resourcesMgr.getShaderProgram(rd->shaderProgram));
            setSkybox(skybox);
        }
    }

    // Light component
    if (lt) {
        auto light = std::make_shared<Light>(id, tr, rd, lt, m_windowSize);

        if (!lt->material.empty()) {
            auto materialPtr = resourcesMgr.getMaterial(lt->material);
            light->setMaterial(materialPtr);
        }

        m_lights[id] = light;
    }
}

void RenderSystem::removeActor(int id)
{
    m_nodes.erase(id);
    m_transparentNodes.erase(id);
    m_lights.erase(id);
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

    for (const auto& node : m_lights) {
        node.second->update(delta);
    }
}

//------------------------------------------------------------------------------

void RenderSystem::draw()
{
    std::array<Light*, 8> lights = {};

    Light* sun = m_lights.begin()->second.get();

    if (m_shadowMapFB) {
        sun->setOrtho(calcDirectionalLightProjection(*sun));

        m_shadowMapFB->bindForWriting();
        glViewport(0, 0, m_shadowMapSize.x, m_shadowMapSize.y);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        draw(m_shadowShader.get(), sun, lights);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_windowSize.x, m_windowSize.y);

        const int shadowTextureUnit = 7;
        m_shadowMapFB->bindDepthComponent(shadowTextureUnit);
        for (auto shader : getShaders()) {
            shader->use();
            shader->setUniform("shadowSampler", shadowTextureUnit);
        }
    }

    lights[0] = sun;

    // draw(sun, lights);
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
    // Used for drawing normals and shadows
    for (const auto& node : m_nodes) {
        node.second->draw(shaderProgram, camera, lights, nullptr);
    }
}

//------------------------------------------------------------------------------

std::set<ShaderProgram*> RenderSystem::getShaders() const
{
    std::set<ShaderProgram*> shaders;
    for (const auto& node : m_nodes) {
        shaders.insert(node.second->getShaderProgram());
    }
    return shaders;
}

//------------------------------------------------------------------------------

Aabb RenderSystem::calcDirectionalLightProjection(const Light& light) const
{
    Aabb ans;
    // All visible nodes for camera
    for (const auto& node : m_nodes) {
        if (!node.second->castsShadows()) continue;

        const auto& aabb = node.second->aabb();
        if (m_camera->isVisible(aabb)) {
            ans = ans.mbr(light.viewMatrix() * aabb);
        }
    }
    Aabb tmp          = ans;
    tmp.rightTopFar.z = 0.f;
    // Nodes between light and visible nodes
    for (const auto& node : m_nodes) {
        if (!node.second->castsShadows()) continue;

        const auto& aabb = light.viewMatrix() * node.second->aabb();
        if (tmp.intersects(aabb)) {
            ans = ans.mbr(aabb);
        }
    }
    return ans;
}

//------------------------------------------------------------------------------

void RenderSystem::resizeWindow(glm::ivec2 size)
{
    m_windowSize = size;
    m_camera->setWindowSize(size);
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
