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
    : m_shadowCascadesSize{Camera::s_shadowCascadesMax}
    , m_windowSize{windowSize}
    , m_shadowMapSize{2048, 2048, m_shadowCascadesSize}
{
    glGenVertexArrays(1, &m_emptyVao);

    m_shadowMapFB = std::make_unique<Framebuffer>(m_shadowMapSize);

    // Add player camera
    m_cameras.emplace_back(m_windowSize);
    m_cameras[Player].transformation()->position = {-3.5f, 11.0f, 3.9f};

    // Add free camera
    m_cameras.emplace_back(m_windowSize);
    m_cameras[Free].transformation()->position = {-3.5f, 11.0f, 3.9f};

    m_camera = &m_cameras.at(Player);
}

//------------------------------------------------------------------------------

RenderSystem::~RenderSystem() { glDeleteVertexArrays(1, &m_emptyVao); }

//------------------------------------------------------------------------------

void RenderSystem::loadCommonResources(const ResourcesMgr& resourcesMgr)
{
    m_shadowShader  = resourcesMgr.getShaderProgram("shadow");
    m_normalsShader = resourcesMgr.getShaderProgram("normals");
    m_aabbShader    = resourcesMgr.getShaderProgram("aabb");
    m_frustumShader = resourcesMgr.getShaderProgram("frustum");

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
    m_fpsCounter.update(delta);

    if (m_camera) {
        m_camera->update(delta);
        updateCameraText();
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

    drawShadows(m_shadowShader.get(), sun, lights);

    lights[0] = sun;

    // draw(sun, lights);
    draw(m_camera, lights);

    if (m_drawNormals) {
        drawNormals(m_normalsShader.get(), m_camera, lights);
        drawAabb(m_aabbShader.get(), m_camera);
        drawFrustum(m_frustumShader.get(), m_camera);
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

void RenderSystem::drawNormals(ShaderProgram* shaderProgram, const Camera* camera,
                               std::array<Light*, 8>& lights) const
{
    for (const auto& node : m_nodes) {
        node.second->draw(shaderProgram, camera, lights, nullptr);
    }
}

void RenderSystem::drawShadows(ShaderProgram* shaderProgram, Camera* camera,
                               std::array<Light*, 8>& lights) const
{
    if (m_shadowMapFB) {
        glViewport(0, 0, m_shadowMapSize.x, m_shadowMapSize.y);
        // glCullFace(GL_FRONT);

        glm::mat4 cascadeProj[Camera::s_shadowCascadesMax];

        for (int cascadeIndex = 0; cascadeIndex < m_shadowCascadesSize; ++cascadeIndex) {
            camera->setOrtho(calcDirectionalLightProjection(*camera, cascadeIndex));
            cascadeProj[cascadeIndex] = camera->projectionMatrix();

            m_shadowMapFB->bindForWriting(cascadeIndex);
            glClear(GL_DEPTH_BUFFER_BIT);

            for (const auto& node : m_nodes) {
                if (node.second->castsShadows())
                    node.second->draw(shaderProgram, camera, lights, nullptr);
            }
        }

        // glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_windowSize.x, m_windowSize.y);

        const int shadowTextureUnit = 7;
        m_shadowMapFB->bindDepthComponent(shadowTextureUnit);
        for (auto shaderProgram : getShaders()) {
            shaderProgram->use();
            shaderProgram->setUniform("shadowSampler", shadowTextureUnit);

            for (int cascadeIndex = 0; cascadeIndex < m_shadowCascadesSize; ++cascadeIndex) {
                const auto far              = m_camera->cascadeIdx2NearFar(cascadeIndex).y;
                const auto& cascadeFarIndex = "cascadeFar[" + std::to_string(cascadeIndex) + "]";
                shaderProgram->setUniform(cascadeFarIndex.c_str(), far);

                const auto& cascadeVPIndex = "cascadeVP[" + std::to_string(cascadeIndex) + "]";
                shaderProgram->setUniform(cascadeVPIndex.c_str(),
                                          cascadeProj[cascadeIndex] * camera->viewMatrix());
            }
        }
    }
}

void RenderSystem::drawAabb(ShaderProgram* shaderProgram, const Camera* camera) const
{
    glBindVertexArray(m_emptyVao);

    for (const auto& node : m_nodes) {
        node.second->drawAabb(shaderProgram, camera);
    }
}

void RenderSystem::drawFrustum(ShaderProgram* shaderProgram, const Camera* camera) const
{
    glBindVertexArray(m_emptyVao);

    m_camera->drawFrustum(shaderProgram, camera);

    for (const auto& light : m_lights) {
        // light.second->setPerspective(45, 1, 100);
        // light.second->setOrtho(Aabb{{-10, -10, 1}, {10, 10, 100}});
        light.second->drawFrustum(shaderProgram, camera);
        // light.second->setOrtho();
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

Aabb RenderSystem::calcDirectionalLightProjection(const Camera& light, int cascadeIndex) const
{
    auto lightViewMatrix = light.viewMatrix();
    auto frustum         = m_camera->frustum(cascadeIndex);
    for (auto& p : frustum)
        p = lightViewMatrix * p;

    Aabb ans = Aabb{frustum};

    Aabb tmp      = ans;
    tmp.maximum.z = std::numeric_limits<float>::max();
    // Nodes between light and visible nodes
    for (const auto& node : m_nodes) {
        if (!node.second->castsShadows()) continue;

        const auto& aabb = light.viewMatrix() * node.second->aabb();
        if (tmp.intersects(aabb)) {
            ans.maximum.z = std::max(ans.maximum.z, aabb.maximum.z);
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

//------------------------------------------------------------------------------

void RenderSystem::updateCameraText()
{
    using namespace boost;

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
