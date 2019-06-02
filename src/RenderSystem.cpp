#include "RenderSystem.h"

#include "Logger.h"
#include "ResourcesMgr.h"
#include "Terrain.h"
#include "gfx/Framebuffer.h"
#include "gfx/Light.h"
#include "gfx/Skybox.h"
#include "gfx/Text.h"

#include <fmt/format.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <array>
#include <boost/algorithm/string/predicate.hpp>

namespace gfx {

#define SHADOW_MAP_SIZE 1024

RenderSystem::RenderSystem(glm::ivec2 windowSize)
    : m_shadowCascadesSize{Camera::s_shadowCascadesMax}
    , m_windowSize{windowSize}
    , m_shadowMapSize{SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, m_shadowCascadesSize}
{
    glGenVertexArrays(1, &m_emptyVao);

    // m_shadowMapFB = std::make_unique<Framebuffer>(m_shadowMapSize);

    // float ratio = m_windowSize.x / float(m_windowSize.y);
    // Add player camera
    // m_cameras.emplace_back();

    // Add free camera
    // m_cameras.emplace_back();

    // m_camera = &m_cameras.at(Player);
}

//------------------------------------------------------------------------------

RenderSystem::~RenderSystem() { glDeleteVertexArrays(1, &m_emptyVao); }

//------------------------------------------------------------------------------

void RenderSystem::loadCommonResources(const ResourcesMgr& resourcesMgr)
{
    m_defaultShader = resourcesMgr.getShaderProgram("default");
    m_shadowShader  = resourcesMgr.getShaderProgram("shadow");
    m_normalsShader = resourcesMgr.getShaderProgram("normals");
    m_aabbShader    = resourcesMgr.getShaderProgram("aabb");
    m_frustumShader = resourcesMgr.getShaderProgram("frustum");

    auto guiFont   = resourcesMgr.getFont("ubuntu");
    auto guiShader = resourcesMgr.getShaderProgram("font");

    m_cameraText = std::make_shared<Text>(guiFont);
    m_cameraText->setShaderProgram(guiShader);
    m_cameraText->setPosition({0.5f, 0.f, 0.0f});
    add(m_cameraText);

    auto skybox    = std::make_shared<Skybox>();
    auto skyboxMtl = resourcesMgr.getMaterial("skybox_mtl");

    auto sampler = std::make_shared<gfx::Sampler>();
    sampler->setClampToEdge();
    std::array<std::shared_ptr<Texture>, TextureUnit::Size> textures;
    textures[TextureUnit::BaseColor]  = skyboxMtl->textures[0];
    textures[TextureUnit::Irradiance] = skyboxMtl->textures[1];
    textures[TextureUnit::Radiance]   = skyboxMtl->textures[2];
    textures[TextureUnit::BrdfLUT]    = skyboxMtl->textures[3];
    for (auto& tex : textures)
        if (tex) tex->setSampler(sampler);

    skybox->setTextures(textures);
    skybox->setShaderProgram(resourcesMgr.getShaderProgram("skybox"));

    setSkybox(skybox);

    // static LightComponent lt;
    // addActor(0, nullptr, nullptr, &lt, resourcesMgr);

    // auto node = std::make_shared<gfx::Node>(99);
    // node->setTranslation(glm::vec3{1.0f});
    // node->setLight(m_lights.begin()->second.get());

    // m_nodes[99] = node;
}

//------------------------------------------------------------------------------

void RenderSystem::addActor(int id, TransformationComponent* tr, RenderComponent* rd,
                            LightComponent* lt, const ResourcesMgr& resourcesMgr)
{
    Actor actor;
    actor.id = id;
    actor.tr = tr;
    actor.rd = rd;
    actor.lt = lt;

    auto model = findModel(actor.rd->model);
    if (model) {
        actor.model = model;
    } else {
        LOG_WARNING("No model named {} found for actor {}", actor.rd->model, id);
    }

    m_actors.push_back(actor);

    lookAtAll();
}

//------------------------------------------------------------------------------

void RenderSystem::removeActor(int id)
{
    auto it =
        std::find_if(m_actors.begin(), m_actors.end(), [id](const auto& a) { return a.id == id; });
    m_actors.erase(it);
}

//------------------------------------------------------------------------------

void RenderSystem::update(float delta)
{
    const glm::mat4 identity{1.0f};

    if (m_camera && m_cameraText) updateCameraText();

    for (auto& model : m_models)
        model->update(delta);
}

//------------------------------------------------------------------------------

void RenderSystem::draw()
{
    if (!m_camera) return;

    std::array<Light*, 8> lights = {};
    // Light* sun                   = m_lights.begin()->second.get();

    // drawShadows(m_shadowShader.get(), m_camera, sun);
    // drawShadows(m_shadowShader.get(), &m_cameras[Player], sun);

    // lights[0] = sun;

    // sun->setCascade(0);
    // draw(sun, lights);
    draw(m_defaultShader.get(), m_camera, lights);

    if (m_drawDebug) {
        drawNormals(m_normalsShader.get(), m_camera);
        drawAabb(m_aabbShader.get(), m_camera);
        drawFrustum(m_frustumShader.get(), m_camera);
    }
}

void RenderSystem::draw(ShaderProgram* shaderProgram, const Camera* camera,
                        std::array<Light*, 8>& lights) const
{
    shaderProgram->use();
    camera->applyTo(shaderProgram);

    const glm::mat4 identity{1.0f};

    if (m_polygonMode != GL_FILL) glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode);

    TexturePack environment;
    if (m_skybox) environment = m_skybox->textures();

    for (const auto& a : m_actors) {
        if (auto model = a.model.lock()) {
            model->draw(camera->viewMatrix() * a.transformation(), shaderProgram, lights,
                        environment);
        }
    }

    /*
for (const auto& node : m_nodes) {
    node.second->draw(identity, camera, lights, environment);
}
    */

    if (m_polygonMode == GL_FILL && m_skybox) m_skybox->draw(camera);

    /*
glDepthMask(GL_FALSE);
for (const auto& node : m_transparentNodes) {
    node.second->draw(identity, camera, lights, environment);
}
glDepthMask(GL_TRUE);
    */

    if (m_polygonMode != GL_FILL) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // UI
    for (const auto& node : m_texts) {
        node->draw();
    }
}

void RenderSystem::drawNormals(ShaderProgram* shaderProgram, const Camera* camera) const
{
    shaderProgram->use();
    camera->applyTo(shaderProgram);

    const glm::mat4 identity{1.0f};
    std::array<Light*, 8> lights = {};

    for (const auto& a : m_actors) {
        if (auto model = a.model.lock()) {
            model->draw(camera->viewMatrix() * a.transformation(), shaderProgram, lights, {});
        }
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
            /*
            for (const auto& node : m_nodes) {
                if (node.second->castsShadows()) light->applyTo(shaderProgram);
                node.second->draw(glm::mat4{}, shaderProgram, lights);
            }*/
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
                const auto zFar             = camera->cascadeIdx2NearFar(ci).y;
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
    shaderProgram->use();
    camera->applyTo(shaderProgram);

    shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
    shaderProgram->setUniform("viewMatrixInv", glm::inverse(camera->viewMatrix()));

    glBindVertexArray(m_emptyVao);

    for (const auto& a : m_actors) {
        if (auto model = a.model.lock()) {
            model->drawAabb(camera->viewMatrix() * a.transformation(), shaderProgram);
        }
    }
}

void RenderSystem::drawFrustum(ShaderProgram* shaderProgram, const Camera* camera) const
{
    shaderProgram->use();
    camera->applyTo(shaderProgram);

    glBindVertexArray(m_emptyVao);

    // m_cameras.at(Player).drawFrustum(shaderProgram, camera);

    // for (const auto& light : m_lights) {
    // light.second->setPerspective(45, 1, 100);
    // light.second->setOrtho(Aabb{{-10, -10, -100}, {10, 10, -1}});
    // light.second->drawFrustum(shaderProgram, camera);
    // light.second->setOrtho();
    //}
}

//------------------------------------------------------------------------------

std::set<ShaderProgram*> RenderSystem::getShaders() const
{
    std::set<ShaderProgram*> shaders;
    // for (const auto& node : m_nodes) {
    //    shaders.insert(node.second->getShaderProgram());
    //}
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
    /*
    for (const auto& node : m_nodes) {
        if (!node.second->castsShadows()) continue;

        const auto& aabb = light.viewMatrix() * node.second->aabb();
        if (tmp.intersects(aabb)) {
            ans.maximum.z = std::max(ans.maximum.z, aabb.maximum.z);
        }
    }
        */

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

    if (m_camera) m_camera->setAspectRatio(size.x / float(size.y));
    /*
for (auto& camera : m_cameras) {
    camera.setAspectRatio(size.x / float(size.y));
}
    */
}

//------------------------------------------------------------------------------

void RenderSystem::setDrawDebug(bool enable, glm::vec3 ntbLengths)
{
    if (m_normalsShader) {
        m_normalsShader->use();
        m_normalsShader->setUniform("lengths", ntbLengths);
        m_drawDebug = enable;
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

    std::array<char, 64> buffer{};
    fmt::format_to_n(std::begin(buffer), buffer.size() - 1,
                     "Cam pos: {:.2f} {:.2f} {:.2f}    Cam rot: {:.0f} {:.0f} {:.0f}", p.x, p.y,
                     p.z, r.x, r.y, r.z);
    m_cameraText->setText(buffer.data());
}

//------------------------------------------------------------------------------

void RenderSystem::lookAtAll()
{
    Aabb aabb;

    for (const auto& a : m_actors) {
        if (auto model = a.model.lock()) {
            aabb = aabb.mbr(a.transformation() * model->aabb());
        }
    }

    glm::vec3 pos = aabb.maximum + glm::vec3{m_camera->zNear()};
    m_camera->update(glm::inverse(glm::lookAt(pos, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f})), 0);
}

//------------------------------------------------------------------------------

void RenderSystem::addModel(std::shared_ptr<Model> model) { m_models.insert(model); }

//------------------------------------------------------------------------------

std::shared_ptr<Model> RenderSystem::findModel(const std::string& name) const
{
    auto it = std::find_if(m_models.cbegin(), m_models.cend(),
                           [&](const auto& m) { return m->name == name; });

    if (it != m_models.cend())
        return *it;
    else
        return std::shared_ptr<Model>();
}

//------------------------------------------------------------------------------

glm::mat4 RenderSystem::Actor::transformation() const
{
    if (tr) {
        return tr->toMatrix();
    } else {
        return glm::mat4{1.0f};
    }
}

} // namespace gfx
