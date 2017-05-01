#include "ActorFactory.h"

std::unique_ptr<Actor> ActorFactory::create(boost::property_tree::ptree::value_type& v)
{
    using namespace boost::property_tree;
    
    auto a = std::make_unique<Actor>(getNextId());
    
    const std::string& actorType  = v.first;
    ptree& actorTree = v.second;

    if (actorType == "camera") {
        /*
          float x = actorTree.get("position.x", 0.0f);
          float y = actorTree.get("position.y", 0.0f);
          float z = actorTree.get("position.z", 0.0f);

          m_camera = std::make_shared<Camera>();
          m_camera->moveTo(x, y, z);
          m_scene.setCamera(m_camera);
        */
    }
    else if (actorType == "skybox") {
        auto rd = std::make_shared<RenderComponent>();

        rd->role = Role::Skybox;
        rd->textures.push_back(actorTree.get<std::string>("texture"));
        rd->shaderProgram = actorTree.get("shaderProgram", "default");

        a->addComponent(ComponentId::Render, rd);
    }
    else if (actorType == "terrain") {
        auto rd = std::make_shared<RenderComponent>();

        rd->role = Role::Terrain;
        //const std::string& name = actorTree.get<std::string>("name");
        rd->mesh = actorTree.get<std::string>("heightMap");
        rd->shaderProgram = actorTree.get("shaderProgram", "default");

        for (ptree::value_type& v : actorTree.get_child("textures")) {
            rd->textures.push_back(v.second.data());
        }

        a->addComponent(ComponentId::Render, rd);
            
        auto tr = std::make_shared<TransformationComponent>();
        tr->scale = actorTree.get("scale", 1.0f);
        tr->position.x = actorTree.get("position.x", 0.0f);
        tr->position.y = actorTree.get("position.y", 0.0f);
        tr->position.z = actorTree.get("position.z", 0.0f);

        a->addComponent(ComponentId::Transformation, tr);
    }
    else if (actorType == "actor") {
        auto rd = std::make_shared<RenderComponent>();

        rd->role = Role::Dynamic;
        //        const std::string& name = actorTree.get<std::string>("name");
        rd->mesh = actorTree.get("mesh", "");
        rd->shaderProgram = actorTree.get("shaderProgram", "default");

        if (auto textures = actorTree.get_child_optional("textures")) {
            for (ptree::value_type& v : actorTree.get_child("textures")) {
                rd->textures.push_back(v.second.data());
            }
        }
        
        a->addComponent(ComponentId::Render, rd);
            
        auto tr = std::make_shared<TransformationComponent>();
        tr->scale = actorTree.get("scale", 1.0f);
        tr->position.x = actorTree.get("position.x", 0.0f);
        tr->position.y = actorTree.get("position.y", 0.0f);
        tr->position.z = actorTree.get("position.z", 0.0f);

        a->addComponent(ComponentId::Transformation, tr);
    }

    return a;
}

//--------------------------------------------------------------------------

unsigned int ActorFactory::getNextId()
{
    static unsigned long id = 0;
    ++id;
    return id;
}
