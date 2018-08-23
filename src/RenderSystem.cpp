#include "RenderSystem.h"

#include "Framebuffer.h"
#include "Light.h"
#include "Logger.h"
#include "ResourcesMgr.h"
#include "Skybox.h"
#include "Terrain.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <array>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

#define SHADOW_MAP_SIZE 1024

RenderSystem::RenderSystem(glm::ivec2 windowSize)
    : m_shadowCascadesSize{Camera::s_shadowCascadesMax}
    , m_windowSize{windowSize}
    , m_shadowMapSize{SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, m_shadowCascadesSize}
{
    glGenVertexArrays(1, &m_emptyVao);

    //m_shadowMapFB = std::make_unique<Framebuffer>(m_shadowMapSize);

    // Add player camera
    m_cameras.emplace_back(m_windowSize);

    // Add free camera
    m_cameras.emplace_back(m_windowSize);

    m_camera = &m_cameras.at(Player);
}

//------------------------------------------------------------------------------

RenderSystem::~RenderSystem() { glDeleteVertexArrays(1, &m_emptyVao); }

//------------------------------------------------------------------------------

void RenderSystem::loadCommonResources(const ResourcesMgr& resourcesMgr)
{
    m_defaultShader  = resourcesMgr.getShaderProgram("default");
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

    auto skybox = std::make_shared<Skybox>();
    skybox->setMaterial(resourcesMgr.getMaterial("skybox_mtl"));
    skybox->setShaderProgram(resourcesMgr.getShaderProgram("skybox"));

    setSkybox(skybox);
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
                    // node->setCastShadows(true);
                } else {
                    node         = std::make_shared<RenderNode>(id, tr, rd);
                    auto meshPtr = resourcesMgr.getMesh(rd->mesh);
                    node->setMesh(meshPtr);
                    if (!lt) node->setCastShadows(true);
                }
            }

            // if (!rd->material.empty()) {
            //     auto materialPtr = resourcesMgr.getMaterial(rd->material);
            //     node->setMaterial(materialPtr);
            // }

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

        // if (!lt->material.empty()) {
        //     auto materialPtr = resourcesMgr.getMaterial(lt->material);
        //     light->setMaterial(materialPtr);
        // }

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
    const glm::mat4 identity{1.0f};

    m_fpsCounter.update(delta);

//    for (auto& camera : m_cameras) {
//        camera.update(identity, delta);
//    }

    if (m_camera && m_cameraText) {
        updateCameraText();
    }

    m_scene->update(delta);

    for (const auto& node : m_nodes) {
        node.second->update(identity, delta);
    }

    for (const auto& node : m_transparentNodes) {
        node.second->update(identity, delta);
    }

    for (const auto& node : m_lights) {
        node.second->update(identity, delta);
    }
}

//------------------------------------------------------------------------------

void RenderSystem::draw()
{
    std::array<Light*, 8> lights = {};
    Light* sun                   = m_lights.begin()->second.get();

    drawShadows(m_shadowShader.get(), m_camera, sun);
    // drawShadows(m_shadowShader.get(), &m_cameras[Player], sun);

    // lights[0] = sun;

    // sun->setCascade(0);
    // draw(sun, lights);
    draw(m_camera, lights);

    if (m_drawNormals) {
        drawNormals(m_normalsShader.get(), m_camera);
        drawAabb(m_aabbShader.get(), m_camera);
        drawFrustum(m_frustumShader.get(), m_camera);
    }
}

void RenderSystem::draw(const Camera* camera, std::array<Light*, 8>& lights) const
{
    const glm::mat4 identity{1.0f};

    if (m_polygonMode != GL_FILL) glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode);

    Texture* environment = nullptr;
    if (m_skybox) environment = m_skybox->material()->textures.at(0).get();

    m_scene->draw(m_defaultShader.get(), camera, lights);

    for (const auto& node : m_nodes) {
        node.second->draw(identity, camera, lights, environment);
    }

    if (m_polygonMode == GL_FILL && m_skybox) m_skybox->draw(camera);

    glDepthMask(GL_FALSE);
    for (const auto& node : m_transparentNodes) {
        node.second->draw(identity, camera, lights, environment);
    }
    glDepthMask(GL_TRUE);

    if (m_polygonMode != GL_FILL) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // UI
    for (const auto& node : m_texts) {
        node->draw();
    }
}

void RenderSystem::drawNormals(ShaderProgram* shaderProgram, const Camera* camera) const
{
    const glm::mat4 identity{1.0f};
    std::array<Light*, 8> lights = {};

    for (const auto& node : m_nodes) {
        node.second->draw(identity, shaderProgram, camera, lights, nullptr);
    }
}

void RenderSystem::drawShadows(ShaderProgram* shaderProgram, Camera* camera, Light* light) const
{
    std::array<Light*, 8> lights = {};

    if (m_shadowMapFB) {
        glViewport(0, 0, m_shadowMapSize.x, m_shadowMapSize.y);
        // glDisable(GL_CULL_FACE);
        // glCullFace(GL_FRONT);

        glm::mat4 cascadeProj[Camera::s_shadowCascadesMax];

        for (int ci = 0; ci < m_shadowCascadesSize; ++ci) {
            const auto& proj = calcDirectionalLightProjection(*camera, *light, ci);
            light->setOrtho(ci, proj);
            light->setCascade(ci);
            cascadeProj[ci] = light->projectionMatrix();

            m_shadowMapFB->bindForWriting(ci);
            glClear(GL_DEPTH_BUFFER_BIT);

            for (const auto& node : m_nodes) {
                if (node.second->castsShadows())
                    node.second->draw(glm::mat4{}, shaderProgram, light, lights, nullptr);
            }
        }

        // glCullFace(GL_BACK);
        // glEnable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_windowSize.x, m_windowSize.y);

        const int shadowTextureUnit = 7;
        m_shadowMapFB->bindDepthComponent(shadowTextureUnit);
        for (auto shaderProgram : getShaders()) {
            shaderProgram->use();
            shaderProgram->setUniform("shadowSampler", shadowTextureUnit);

            for (int ci = 0; ci < m_shadowCascadesSize; ++ci) {
                const auto zFar              = camera->cascadeIdx2NearFar(ci).y;
                const auto& cascadeFarIndex = "cascadeFar[" + std::to_string(ci) + "]";
                shaderProgram->setUniform(cascadeFarIndex.c_str(), zFar);

                const auto& cascadeVPIndex = "cascadeVP[" + std::to_string(ci) + "]";
                shaderProgram->setUniform(cascadeVPIndex.c_str(),
                                          cascadeProj[ci] * light->viewMatrix());
            }
        }
    }
}

void RenderSystem::drawAabb(ShaderProgram* shaderProgram, const Camera* camera) const
{
    glBindVertexArray(m_emptyVao);

    for (const auto& node : m_nodes) {
        node.second->drawAabb(glm::mat4{}, shaderProgram, camera);
    }
}

void RenderSystem::drawFrustum(ShaderProgram* shaderProgram, const Camera* camera) const
{
    glBindVertexArray(m_emptyVao);

    m_cameras.at(Player).drawFrustum(shaderProgram, camera);

    for (const auto& light : m_lights) {
        // light.second->setPerspective(45, 1, 100);
        // light.second->setOrtho(Aabb{{-10, -10, -100}, {10, 10, -1}});
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

Aabb RenderSystem::calcDirectionalLightProjection(const Camera& camera, const Camera& light,
                                                  int cascadeIndex) const
{
    auto lightViewMatrix = light.viewMatrix();
    auto frustum         = camera.frustum(cascadeIndex);
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

    // Shimmering edges
    glm::vec2 worldUnitsPerTexel;
    worldUnitsPerTexel.x = (ans.maximum.x - ans.minimum.x) / SHADOW_MAP_SIZE;
    worldUnitsPerTexel.y = (ans.maximum.y - ans.minimum.y) / SHADOW_MAP_SIZE;

    ans.minimum.x = std::floor(ans.minimum.x / worldUnitsPerTexel.x) * worldUnitsPerTexel.x;
    ans.minimum.y = std::floor(ans.minimum.y / worldUnitsPerTexel.y) * worldUnitsPerTexel.y;

    ans.maximum.x = std::ceil(ans.maximum.x / worldUnitsPerTexel.x) * worldUnitsPerTexel.x;
    ans.maximum.y = std::ceil(ans.maximum.y / worldUnitsPerTexel.y) * worldUnitsPerTexel.y;

    return ans;
}

//------------------------------------------------------------------------------

void RenderSystem::resizeWindow(glm::ivec2 size)
{
    m_windowSize = size;
    for (auto& camera : m_cameras) {
        camera.setWindowSize(size);
    }
}

//------------------------------------------------------------------------------

void RenderSystem::setDrawNormals(bool enable, float normalLength)
{
    if (m_normalsShader) {
        m_normalsShader->use();
        m_normalsShader->setUniform("magnitude", normalLength);
        m_drawNormals = enable;
        glUseProgram(0);
    } else {
        LOG_INFO("Shader for drawing normal vectors is not loaded");
    }
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

    auto m_worldMatrix = glm::inverse(m_camera->viewMatrix());

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(m_worldMatrix, scale, rotation, translation, skew, perspective);

    auto p = translation;
    auto r = glm::eulerAngles(glm::conjugate(rotation)) * 180.f / 3.14f;

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
