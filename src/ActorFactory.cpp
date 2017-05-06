#include "ActorFactory.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::property_tree;

static glm::vec4 stringToVector(std::string str)
{
    glm::vec4 retval;

    if (!str.empty()) {
        boost::trim(str);
        std::vector<std::string> splitted;
        boost::split(splitted, str, boost::is_any_of(" \t"));
        for (size_t i = 0; i < splitted.size(); ++i)
            retval[i] = boost::lexical_cast<float>(splitted[i]);
    }

    return retval;
}

static std::shared_ptr<RenderComponent> getRenderComponent(ptree& actorTree)
{
    auto rd = std::make_shared<RenderComponent>();

    rd->role          = Role::Dynamic;
    rd->mesh          = actorTree.get("mesh", "");
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
    auto tr                 = std::make_shared<TransformationComponent>();
    std::string orientation = actorTree.get("orientation", "");
    // Conversion form euler to quaternion
    tr->orientation = glm::vec3{stringToVector(orientation)};

    std::string position = actorTree.get("position", "");
    tr->position         = stringToVector(position);

    tr->scale = actorTree.get("scale", 1.0f);

    return tr;
}

std::unique_ptr<Actor> ActorFactory::create(boost::property_tree::ptree::value_type& v)
{
    auto a = std::make_unique<Actor>(getNextId());

    const std::string& actorType = v.first;
    ptree& actorTree             = v.second;

    auto trNode = actorTree.get_child_optional("transformation");
    if (trNode) {
        auto tr = getTransformationComponent(trNode.get());
        a->addComponent(ComponentId::Transformation, tr);
    }

    auto rdNode = actorTree.get_child_optional("render");
    if (rdNode) {
        auto rd  = getRenderComponent(rdNode.get());
        rd->role = Role::Skybox;
        a->addComponent(ComponentId::Render, rd);
    }

    if (actorType == "skybox") {
        auto rd = a->getComponent<RenderComponent>(ComponentId::Render).lock();
        if (rd)
            rd->role = Role::Skybox;
    } else if (actorType == "terrain") {
        auto rd = a->getComponent<RenderComponent>(ComponentId::Render).lock();
        if (rd)
            rd->role = Role::Terrain;
    } else if (actorType == "actor") {
        auto rd = a->getComponent<RenderComponent>(ComponentId::Render).lock();
        if (rd)
            rd->role = Role::Dynamic;
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
