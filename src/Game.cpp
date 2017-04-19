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

//------------------------------------------------------------------------------

Game::Game()
{
    init();
    loadData();
}

void Game::resizeWindow(int width, int height)
{
    /* Protect against a divide by zero */
    if (height == 0)
        height = 1;

    GLfloat ratio = GLfloat(width) / GLfloat(height);

    m_camera->setPerspective(45.0f, ratio, 5.0f, 500.0f);

    super::resizeWindow(width, height);
}

void Game::init()
{
    // glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);
}

void Game::loadData()
{
    // Load scene
    using boost::property_tree::ptree;
    ptree pt;

    read_xml("config.xml", pt);    
    const std::string& dataFolder = pt.get<std::string>("config.assets.dataFolder");
    const std::string& shadersFolder = pt.get<std::string>("config.assets.shadersFolder");

    m_resourcesMgr = std::make_unique<ResourcesMgr>(dataFolder, shadersFolder);

    auto rotationScript = std::make_shared<RotationScript>();
    m_resourcesMgr->addScript("rotationScript", rotationScript);
    
    for (ptree::value_type &v : pt.get_child("config.assets")) {
        const std::string& assetType  = v.first;
        ptree& assetTree = v.second;

        if (assetType == "shaderProgram") {
            const std::string& name = assetTree.get<std::string>("name");
            const std::string& vertexShaderFile = assetTree.get<std::string>("vertexShader");
            const std::string& fragmentShaderFile = assetTree.get<std::string>("fragmentShader");           
            m_resourcesMgr->addShaderProgram(name, vertexShaderFile, fragmentShaderFile);          
        }
        else if (assetType == "texture") {
            const std::string& name = assetTree.get<std::string>("name");
            const std::string& wrap = assetTree.get<std::string>("wrap", "GL_REPEAT");
            if (assetTree.get_child("file").size() == 0) {
                const std::string& file = assetTree.get<std::string>("file");
                m_resourcesMgr->addTexture(name, file, wrap);
            } else {
                std::array<std::string, 6> files;
                files[0] = assetTree.get<std::string>("file.right");
                files[1] = assetTree.get<std::string>("file.left");
                files[2] = assetTree.get<std::string>("file.top");
                files[3] = assetTree.get<std::string>("file.bottom");
                files[4] = assetTree.get<std::string>("file.back");
                files[5] = assetTree.get<std::string>("file.front");
                m_resourcesMgr->addTexture(name, files, wrap);
            }
        } else if (assetType == "mesh") {
            const std::string& name = assetTree.get<std::string>("name");
            const std::string& file = assetTree.get<std::string>("file");
            m_resourcesMgr->addMesh(name, file);
        }
    }

    for (ptree::value_type &v : pt.get_child("config.scene")) {
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
            const std::string& texture = actorTree.get("texture", "");
            const std::string& shaderProgram = actorTree.get("shaderProgram", "default");
            float scale = actorTree.get("scale", 1.0f);
            float x = actorTree.get("position.x", 0.0f);
            float y = actorTree.get("position.y", 0.0f);
            float z = actorTree.get("position.z", 0.0f);

            auto a = std::make_shared<Terrain>(name, dataFolder + map);
            a->setScale(scale);
            a->moveTo(x, y, z);

            if (!texture.empty()) {
                auto texturePtr = m_resourcesMgr->getTexture(texture);
                a->setTexture(texturePtr);
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
                a->setTexture(texturePtr);
            }

            a->setShaderProgram(m_resourcesMgr->getShaderProgram(shaderProgram));

            if (a->name() != "floor")
                a->setScript(rotationScript);
           
            gameObjectManager().add(a);
        }
    }
}

void Game::draw()
{
    /* Clear The Screen And The Depth Buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //super::draw();
    gameObjectManager().draw(m_camera.get());
}

void Game::update(float delta)
{
    static sf::Vector2i lastMousePos = sf::Mouse::getPosition(getWindow());
    sf::Vector2i currentMousePos = sf::Mouse::getPosition(getWindow());

    int dx = currentMousePos.x - lastMousePos.x;
    int dy = currentMousePos.y - lastMousePos.y;

    lastMousePos = currentMousePos;

    float mouseSensity = 0.01f;

    if ((dx || dy) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        m_camera->rotate(-dy * mouseSensity, -dx * mouseSensity, 0.0f);
    }

    float cameraSpeed = 50.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        m_camera->moveForward(-delta * cameraSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        m_camera->moveForward(delta * cameraSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        m_camera->moveRight(-delta * cameraSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        m_camera->moveLeft(-delta * cameraSpeed);
    }

    super::update(delta);
}

void Game::mouseWheelMoved(int wheelDelta)
{
    m_camera->moveForward(-wheelDelta);
}

void Game::keyPressed(const sf::Event::KeyEvent& /*e*/)
{}

void Game::keyReleased(const sf::Event::KeyEvent& e)
{
    switch (e.code) { 
    case sf::Keyboard::Space:
        {
            auto s = m_resourcesMgr->getScript("rotationScript");
            std::dynamic_pointer_cast<RotationScript>(s)->togglePause();
        }
        break;
    case sf::Keyboard::Z:
        {
            GLint polygonMode;
            glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
            polygonMode = polygonMode == GL_FILL ? GL_LINE : GL_FILL;
            glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
        }
    default:
        break;
    }
}
