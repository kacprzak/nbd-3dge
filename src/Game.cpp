#include "Game.h"

#include "Texture.h"
#include "Skybox.h"
#include "Shader.h"
#include "Terrain.h"

//#define GLM_FORCE_RADIANS

#include <glm/gtc/type_ptr.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

class RotationScript : public Script
{
public:
    void execute(float delta, Actor *a) override
    {
        if (m_paused)
            return;
        
        a->rotate(0.0f, delta * 0.5f, 0.0f);
        a->moveForward(delta * 20.0f);
    }

    void togglePause()
    {
        m_paused = !m_paused;
    }

private:
    bool m_paused = false;
};

//==============================================================================

Game::Game(const Settings& settings)
    : SDLWindow{settings}
{
    loadData(settings);
}

//------------------------------------------------------------------------------

void Game::resizeWindow(int width, int height)
{
    /* Protect against a divide by zero */
    if (height == 0)
        height = 1;

    GLfloat ratio = GLfloat(width) / GLfloat(height);

    m_camera->setPerspective(45.0f, ratio, 5.0f, 1250.0f);

    super::resizeWindow(width, height);
}

//------------------------------------------------------------------------------

void Game::loadData(const Settings& s)
{
    // Load scene
    using boost::property_tree::ptree;
    ptree pt;

    read_xml(s.dataFolder + "scene.xml", pt);
    const auto& assetsXml = pt.get<std::string>("scene.assets");

    m_resourcesMgr = std::make_unique<ResourcesMgr>(s.dataFolder, s.shadersFolder);
    m_resourcesMgr->load(assetsXml);
    
    auto rotationScript = std::make_shared<RotationScript>();
    m_resourcesMgr->addScript("rotationScript", rotationScript);
    
    auto text = std::make_shared<Text>(m_resourcesMgr->getFont("ubuntu"));
    text->setShaderProgram(m_resourcesMgr->getShaderProgram("font"));
    gameObjectManager().add(text);
    m_fpsCounter.setText(text);
    
    for (ptree::value_type& v : pt.get_child("scene")) {
        const std::string& actorType  = v.first;
        ptree& actorTree = v.second;

        if (actorType == "camera") {
            float x = actorTree.get("position.x", 0.0f);
            float y = actorTree.get("position.y", 0.0f);
            float z = actorTree.get("position.z", 0.0f);

            m_camera = std::make_shared<Camera>();
            m_camera->moveTo(x, y, z);
            gameObjectManager().setCamera(m_camera);
        }
        else if (actorType == "skybox") {
            const std::string& texture = actorTree.get<std::string>("texture");
            const std::string& shaderProgram = actorTree.get("shaderProgram", "default");

            auto skybox = std::make_shared<Skybox>(m_resourcesMgr->getTexture(texture));
            skybox->setShaderProgram(m_resourcesMgr->getShaderProgram(shaderProgram));

            gameObjectManager().setSkybox(skybox);
        }
        else if (actorType == "terrain") {
            const std::string& name = actorTree.get<std::string>("name");
            const std::string& map = actorTree.get<std::string>("heightMap");
            const std::string& shaderProgram = actorTree.get("shaderProgram", "default");
            float scale = actorTree.get("scale", 1.0f);
            float x = actorTree.get("position.x", 0.0f);
            float y = actorTree.get("position.y", 0.0f);
            float z = actorTree.get("position.z", 0.0f);

            auto a = std::make_shared<Terrain>(name, s.dataFolder + map);
            a->setScale(scale);
            a->moveTo(x, y, z);

            for (ptree::value_type& v : actorTree.get_child("textures")) {
                const std::string& texture = v.second.data();
                auto texturePtr = m_resourcesMgr->getTexture(texture);
                a->addTexture(texturePtr);
            }
            
            a->setShaderProgram(m_resourcesMgr->getShaderProgram(shaderProgram));

            gameObjectManager().add(a);
        }
        else if (actorType == "actor") {
            const std::string& name = actorTree.get<std::string>("name");
            const std::string& mesh = actorTree.get("mesh", "");
            const std::string& texture = actorTree.get("texture", "");
            const std::string& shaderProgram = actorTree.get("shaderProgram", "default");
            float scale = actorTree.get("scale", 1.0f);
            float x = actorTree.get("position.x", 0.0f);
            float y = actorTree.get("position.y", 0.0f);
            float z = actorTree.get("position.z", 0.0f);
            
            auto a = std::make_shared<Actor>(name);
            a->setScale(scale);
            a->moveTo(x, y, z);

            if (!mesh.empty()) {
                auto meshPtr = m_resourcesMgr->getMesh(mesh);
                a->setMesh(meshPtr);
            }
 
            if (!texture.empty()) {
                auto texturePtr = m_resourcesMgr->getTexture(texture);
                a->addTexture(texturePtr);
            }

            a->setShaderProgram(m_resourcesMgr->getShaderProgram(shaderProgram));

            if (a->name() != "floor")
                a->setScript(rotationScript);
           
            gameObjectManager().add(a);
        }
    }
}

//------------------------------------------------------------------------------

void Game::draw()
{
    preDraw();

    //super::draw();
    gameObjectManager().draw(m_camera.get());

    if (m_normalsShader) {
        gameObjectManager().draw(m_normalsShader.get(), m_camera.get());
    }

    postDraw();
}

//------------------------------------------------------------------------------

void Game::update(float delta)
{
    m_fpsCounter.update(delta);
    
    float cameraSpeedMultiplyer = 1.0f;
    if (m_shiftPressed)
        cameraSpeedMultiplyer = 5.0f;
    
    if (m_wPressed) {
        m_camera->moveForward(-delta * m_cameraSpeed * cameraSpeedMultiplyer);
    }

    if (m_sPressed) {
        m_camera->moveForward(delta * m_cameraSpeed * cameraSpeedMultiplyer);
    }

    if (m_dPressed) {
        m_camera->moveRight(-delta * m_cameraSpeed * cameraSpeedMultiplyer);
    }

    if (m_aPressed) {
        m_camera->moveLeft(-delta * m_cameraSpeed * cameraSpeedMultiplyer);
    }

    super::update(delta);
}

//------------------------------------------------------------------------------

void Game::mouseMoved(const SDL_Event& event)
{
    float mouseSensity = 0.01f;

    if (m_leftMouseButtonPressed)
        m_camera->rotate(-event.motion.yrel * mouseSensity,
                         -event.motion.xrel * mouseSensity, 0.0f);
}

void Game::mouseButtonPressed(const SDL_Event& event)
{
    switch(event.button.button) {
    case SDL_BUTTON_LEFT:
        m_leftMouseButtonPressed = true;
        setMouseRelativeMode(true);
        break;
    }
}

void Game::mouseButtonReleased(const SDL_Event& event)
{
    switch(event.button.button) {
    case SDL_BUTTON_LEFT:
        m_leftMouseButtonPressed = false;
        setMouseRelativeMode(false);
        break;
    }
}

//------------------------------------------------------------------------------

void Game::keyPressed(const SDL_Event& event)
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

void Game::keyReleased(const SDL_Event& event)
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
            auto s = m_resourcesMgr->getScript("rotationScript");
            std::dynamic_pointer_cast<RotationScript>(s)->togglePause();
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
    default:
        break;
    }
}
