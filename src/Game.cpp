/* -*- c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include "Game.h"

#include "Texture.h"
#include "Skybox.h"
#include "Shader.h"

//#define GLM_FORCE_RADIANS

#include <glm/gtc/type_ptr.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

Game::Game()
    : m_sp(0)
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

    GameCore::resizeWindow(width, height);
}

void Game::init()
{
    //    // Create light components
    //    GLfloat ambientLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    //    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
    //    GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    //    GLfloat position[] = { 50.0f, 50.0f, 50.0f, 1.0f };

    //    // Assign created components to GL_LIGHT0
    //    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    //    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    //    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    //    glLightfv(GL_LIGHT0, GL_POSITION, position);

    //    glEnable(GL_LIGHTING);
    //    glEnable(GL_LIGHT0);
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
    
    for (ptree::value_type &v : pt.get_child("config.assets")) {
        const std::string& assetType  = v.first;
        ptree& assetTree = v.second;

        if (assetType == "shaderProgram") {
            const std::string& name = assetTree.get<std::string>("name");
            const std::string& vertexShaderFile = assetTree.get<std::string>("vertexShader");
            const std::string& fragmentShaderFile = assetTree.get<std::string>("fragmentShader");
            
            m_resourcesMgr->addShaderProgram(name, vertexShaderFile, fragmentShaderFile);          
            m_sp = m_resourcesMgr->getShaderProgram(name);
        }
        else if (assetType == "texture") {
            const std::string& textureName = assetTree.get<std::string>("name");
            const std::string& textureFile = assetTree.get<std::string>("file");

            m_resourcesMgr->addTexture(textureName, textureFile);
        }
    }

    for (ptree::value_type &v : pt.get_child("config.scene")) {
        const std::string& actorType  = v.first;
        ptree& actorTree = v.second;

        if (actorType == "camera") {
            float x = actorTree.get("position.x", 0.0f);
            float y = actorTree.get("position.y", 0.0f);
            float z = actorTree.get("position.z", 0.0f);

            m_camera = new Camera;
            m_camera->moveTo(x, y, z);
            gameObjectManager().setCamera(m_camera);
        }
        else if (actorType == "skybox") {
            const std::string& front = actorTree.get<std::string>("front");
            const std::string& right = actorTree.get<std::string>("right");
            const std::string& back = actorTree.get<std::string>("back");
            const std::string& left = actorTree.get<std::string>("left");
            const std::string& top = actorTree.get<std::string>("top");
            const std::string& bottom = actorTree.get<std::string>("bottom");

            // SKYBOX
            TexturePtr sb_front(Texture::create(front, true));
            TexturePtr sb_right(Texture::create(right, true));
            TexturePtr sb_back(Texture::create(back, true));
            TexturePtr sb_left(Texture::create(left, true));
            TexturePtr sb_top(Texture::create(top, true));
            
            Skybox *skybox;

            if (bottom.empty()) {            
                skybox = new Skybox(sb_front, sb_right, sb_back, sb_left, sb_top);
            } else {
                TexturePtr sb_bottom(Texture::create(bottom, true));
                skybox = new Skybox(sb_front, sb_right, sb_back, sb_left, sb_top, sb_bottom);
            }

            gameObjectManager().setSkybox(skybox);
        }
        else if (actorType == "actor") {
            const std::string& name = actorTree.get<std::string>("name");
            const std::string& meshFile = actorTree.get<std::string>("mesh");
            const std::string& meshShadingStr = actorTree.get("mesh.shading", "GL_SMOOTH");
 
            GLenum shading = GL_SMOOTH;
            if (meshShadingStr == "GL_FLAT")
                shading = GL_FLAT;
            
            const std::string& textureFile = actorTree.get("texture", "");
            float scale = actorTree.get("scale", 1.0f);
            float x = actorTree.get("position.x", 0.0f);
            float y = actorTree.get("position.y", 0.0f);
            float z = actorTree.get("position.z", 0.0f);
            
            MeshPtr mesh(Mesh::create(meshFile, shading));
            Actor *a = new Actor(name, mesh);
            a->setScale(scale);
            a->moveTo(x, y, z);
 
            if (!textureFile.empty()) {
                TexturePtr texturePtr(Texture::create(textureFile));
                a->setTexture(texturePtr);
            }
           
            gameObjectManager().add(a);
        }
    }
}

void Game::draw()
{
    /* Clear The Screen And The Depth Buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_sp->use();

    GLint projectionMatrixUnif = glGetUniformLocation(m_sp->id(), "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixUnif, 1, GL_FALSE,
                       glm::value_ptr(m_camera->projectionMatrix()));

    GLint viewMatrixUnif = glGetUniformLocation(m_sp->id(), "viewMatrix");
    glUniformMatrix4fv(viewMatrixUnif, 1, GL_FALSE,
                       glm::value_ptr(m_camera->viewMatrix()));

    //super::draw();
    gameObjectManager().draw(m_sp);

    m_sp->use(false);
}

void Game::mouseWheelMoved(int wheelDelta)
{
    m_camera->moveForward(-wheelDelta);
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

    for (Actor *a : gameObjectManager().actors()) {
        if (a->name() != "floor") {
            a->rotate(0.0f, delta * 0.5f, 0.0f);
            a->moveForward(delta * 20.0f);
        }
    }

    super::update(delta);
}
