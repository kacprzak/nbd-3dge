#include "GameClient.h"

#include "Texture.h"
#include "Skybox.h"
#include "Shader.h"
#include "Terrain.h"

//#define GLM_FORCE_RADIANS

#include <glm/gtc/type_ptr.hpp>

GameClient::GameClient(const Settings& settings)
    : SDLWindow{settings}, m_settings{settings}
{
    loadData(settings);
}

//------------------------------------------------------------------------------

void GameClient::resizeWindow(int width, int height)
{
    /* Protect against a divide by zero */
    if (height == 0)
        height = 1;

    GLfloat ratio = GLfloat(width) / GLfloat(height);

    m_camera->setPerspective(45.0f, ratio, 5.0f, 1250.0f);

    super::resizeWindow(width, height);
}

//------------------------------------------------------------------------------

void GameClient::loadData(const Settings& /*s*/)
{
    m_camera = std::make_shared<Camera>();
    m_camera->transformation()->position = {-35.f, 110.f, 39.f};
    m_camera->transformation()->orientation = {-0.01f, -0.77f, 0.0f, 0.0f};
    m_scene.setCamera(m_camera);
}

//------------------------------------------------------------------------------

void GameClient::loadResources(const std::string& xmlFile)
{
    m_resourcesMgr = std::make_unique<ResourcesMgr>(m_settings.dataFolder, m_settings.shadersFolder);
    m_resourcesMgr->load(xmlFile);

    auto text = std::make_shared<Text>(m_resourcesMgr->getFont("ubuntu"));
    text->setShaderProgram(m_resourcesMgr->getShaderProgram("font"));
    m_scene.add(text);
    m_fpsCounter.setText(text);
}

void GameClient::unloadResources()
{
    m_resourcesMgr.reset();
}

//------------------------------------------------------------------------------

void GameClient::addActor(int id, TransformationComponent* tr, RenderComponent* rd)
{
    std::shared_ptr<GfxNode> a;
    if (rd->role == Role::Dynamic) {
        a = std::make_shared<GfxNode>(id, tr);

        if (!rd->mesh.empty()) {
            auto meshPtr = m_resourcesMgr->getMesh(rd->mesh);
            a->setMesh(meshPtr);
        }
    } else if (rd->role == Role::Skybox) {
        auto skybox = std::make_shared<Skybox>(m_resourcesMgr->getTexture(rd->textures[0]));
        skybox->setShaderProgram(m_resourcesMgr->getShaderProgram(rd->shaderProgram));
        m_scene.setSkybox(skybox);
        return;
    } else if (rd->role == Role::Terrain) {
        a = std::make_shared<Terrain>(id, tr, m_settings.dataFolder + rd->mesh);
    }
    
    for (const auto& texture : rd->textures) {
        auto texturePtr = m_resourcesMgr->getTexture(texture);
        a->addTexture(texturePtr);
    }

    a->setShaderProgram(m_resourcesMgr->getShaderProgram(rd->shaderProgram));

    m_scene.add(id, a);
}
    
void GameClient::removeActor(int id)
{
    m_scene.remove(id);
}

//------------------------------------------------------------------------------

void GameClient::draw()
{
    preDraw();

    m_scene.draw(m_camera.get());

    if (m_normalsShader) {
        m_scene.draw(m_normalsShader.get(), m_camera.get());
    }

    postDraw();
}

//------------------------------------------------------------------------------

void GameClient::update(float delta)
{
    m_fpsCounter.update(delta);
    
    float cameraSpeedMultiplyer = 1.0f;
    if (m_shiftPressed)
        cameraSpeedMultiplyer = 5.0f;
    
    if (m_wPressed) {
        m_camera->transformation()->moveForward(-delta * m_cameraSpeed * cameraSpeedMultiplyer);
    }

    if (m_sPressed) {
        m_camera->transformation()->moveForward(delta * m_cameraSpeed * cameraSpeedMultiplyer);
    }

    if (m_dPressed) {
        m_camera->transformation()->moveRight(-delta * m_cameraSpeed * cameraSpeedMultiplyer);
    }

    if (m_aPressed) {
        m_camera->transformation()->moveLeft(-delta * m_cameraSpeed * cameraSpeedMultiplyer);
    }

    m_scene.update(delta);
}

//------------------------------------------------------------------------------

void GameClient::mouseMoved(const SDL_Event& event)
{
    float mouseSensity = 0.01f;

    if (m_leftMouseButtonPressed)
        m_camera->transformation()->orientation += glm::vec4{-event.motion.yrel * mouseSensity,
                -event.motion.xrel * mouseSensity, 0.0f, 0.0f};
}

void GameClient::mouseButtonPressed(const SDL_Event& event)
{
    switch(event.button.button) {
    case SDL_BUTTON_LEFT:
        m_leftMouseButtonPressed = true;
        setMouseRelativeMode(true);
        break;
    }
}

void GameClient::mouseButtonReleased(const SDL_Event& event)
{
    switch(event.button.button) {
    case SDL_BUTTON_LEFT:
        m_leftMouseButtonPressed = false;
        setMouseRelativeMode(false);
        break;
    }
}

//------------------------------------------------------------------------------

void GameClient::keyPressed(const SDL_Event& event)
{
    switch(event.key.keysym.sym) {
    case SDLK_w:
        m_wPressed = true;
        break;
    case SDLK_s:
        m_sPressed = true;
        break;
    case SDLK_a:
        m_aPressed = true;
        break;
    case SDLK_d:
        m_dPressed = true;
        break;
    }

    switch(event.key.keysym.scancode) {
    case SDL_SCANCODE_LSHIFT:
        m_shiftPressed = true;
        break;
    default:
        break;
    }

}

void GameClient::keyReleased(const SDL_Event& event)
{
    switch(event.key.keysym.sym) {
    case SDLK_w:
        m_wPressed = false;
        break;
    case SDLK_s:
        m_sPressed = false;
        break;
    case SDLK_a:
        m_aPressed = false;
        break;
    case SDLK_d:
        m_dPressed = false;
        break;
    }


    switch(event.key.keysym.scancode) {
    case SDL_SCANCODE_LSHIFT:
        m_shiftPressed = false;
        break;
    case SDL_SCANCODE_SPACE:
        {
            //auto s = m_resourcesMgr->getScript("rotationScript");
            //std::dynamic_pointer_cast<RotationScript>(s)->togglePause();
        }
        break;
    case SDL_SCANCODE_Z:
        {
            GLint polygonMode;
            glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
            polygonMode = polygonMode == GL_FILL ? GL_LINE : GL_FILL;
            glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
        }
        break;
    case SDL_SCANCODE_N:
        {
            static bool showNormals = false;
            static int magnitude = 2;
            
            const auto& shader = m_resourcesMgr->getShaderProgram("normals");
            if (event.key.keysym.mod & KMOD_SHIFT) {
                magnitude = std::abs(++magnitude);
                shader->use();
                shader->setUniform("magnitude", 0.5f * magnitude);
            }
            else if (event.key.keysym.mod & KMOD_CTRL) {
                magnitude = std::abs(--magnitude);
                if (magnitude == 0)
                    magnitude = 1;
                shader->use();
                shader->setUniform("magnitude", 0.5f * magnitude);
            }
            else {
                showNormals = !showNormals;
                if (showNormals)
                    m_normalsShader = shader;               
                else
                    m_normalsShader.reset();
            }
        }
        break;
    case SDL_SCANCODE_V:
        toggleVSync();
        break;
    default:
        break;
    }
}
