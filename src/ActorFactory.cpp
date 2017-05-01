#include "ActorFactory.h"

using namespace boost::property_tree;

static std::shared_ptr<RenderComponent> getRenderComponent(ptree& actorTree)
{
    auto rd = std::make_shared<RenderComponent>();

    rd->role = Role::Dynamic;
    rd->mesh = actorTree.get("mesh", "");
    rd->shaderProgram = actorTree.get("shaderProgram", "default");

    if (auto textures = actorTree.get_child_optional("textures")) {
        for (ptree::value_type& v : actorTree.get_child("textures")) {
            rd->textures.push_back(v.second.data());
        }
    }

    return rd;
}

static std::shared_ptr<TransformationComponent> getTransformationComponent(ptree& actorTree)
{
    auto tr = std::make_shared<TransformationComponent>();
    tr->scale = actorTree.get("scale", 1.0f);
    tr->position.x = actorTree.get("position.x", 0.0f);
    tr->position.y = actorTree.get("position.y", 0.0f);
    tr->position.z = actorTree.get("position.z", 0.0f);

    return tr;
}

std::unique_ptr<Actor> ActorFactory::create(boost::property_tree::ptree::value_type& v)
{
    auto a = std::make_unique<Actor>(getNextId());
    
    const std::string& actorType  = v.first;
    ptree& actorTree = v.second;

    if (actorType == "skybox") {
        auto rd = getRenderComponent(actorTree);
        rd->role = Role::Skybox;
        a->addComponent(ComponentId::Render, rd);
    }
    else if (actorType == "terrain") {
        auto rd = getRenderComponent(actorTree);
        rd->role = Role::Terrain;
        a->addComponent(ComponentId::Render, rd);
            
        auto tr = getTransformationComponent(actorTree);
        a->addComponent(ComponentId::Transformation, tr);
    }
    else if (actorType == "actor") {
        auto rd = getRenderComponent(actorTree);
        rd->role = Role::Dynamic;
        a->addComponent(ComponentId::Render, rd);

        auto tr = getTransformationComponent(actorTree);    
        a->addComponent(ComponentId::Transformation, tr);
    }

    return a;
}

//--------------------------------------------------------------------------

unsigned int ActorFactory::getNextId()
{
    static unsigned int id = 0;
    ++id;
    return id;
}
