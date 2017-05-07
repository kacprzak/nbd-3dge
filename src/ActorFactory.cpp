#include "ActorFactory.h"

#include "Logger.h"

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

//------------------------------------------------------------------------------

static std::shared_ptr<RenderComponent> getRenderComponent(ptree& actorTree,
                                                           RenderComponent prototype)
{
    auto rd = std::make_shared<RenderComponent>();

    rd->role          = Role::Dynamic;
    rd->mesh          = actorTree.get("mesh", prototype.mesh);
    rd->shaderProgram = actorTree.get("shaderProgram", prototype.shaderProgram);

    if (auto textures = actorTree.get_child_optional("textures")) {
        for (ptree::value_type& v : actorTree.get_child("textures")) {
            rd->textures.push_back(v.second.data());
        }
    } else {
        rd->textures = prototype.textures;
    }

    return rd;
}

static std::shared_ptr<TransformationComponent>
getTransformationComponent(ptree& actorTree, TransformationComponent prototype)
{
    auto tr                 = std::make_shared<TransformationComponent>();
    std::string orientation = actorTree.get("orientation", "");
    if (!orientation.empty()) {
        // Conversion form euler to quaternion
        tr->orientation = glm::vec3{stringToVector(orientation)};
    } else {
        tr->orientation = prototype.orientation;
    }

    std::string position = actorTree.get("position", "");
    if (!position.empty()) {
        tr->position = stringToVector(position);
    } else {
        tr->position = prototype.position;
    }

    tr->scale = actorTree.get("scale", prototype.scale);

    return tr;
}

static std::shared_ptr<PhysicsComponent> getPhysicsComponent(ptree& actorTree,
                                                             PhysicsComponent prototype)
{
    auto ph = std::make_shared<PhysicsComponent>();

    ph->shape = actorTree.get("shape", prototype.shape);
    ph->mass  = actorTree.get("mass", prototype.mass);

    return ph;
}

//------------------------------------------------------------------------------

void ActorFactory::registerPrototype(boost::property_tree::ptree::value_type& v)
{
    const std::string& actorType = v.first;
    ptree& actorTree             = v.second;

    assert(actorType == "actorPrototype");
    auto a = std::make_unique<Actor>(0);

    const auto& prototypeName = actorTree.get<std::string>("name");

    TransformationComponent trProto;
    RenderComponent rdProto;
    PhysicsComponent phProto;

    auto trNode = actorTree.get_child_optional("transformation");
    if (trNode) {
        auto tr = getTransformationComponent(trNode.get(), trProto);
        a->addComponent(ComponentId::Transformation, tr);
    }

    auto rdNode = actorTree.get_child_optional("render");
    if (rdNode) {
        auto rd  = getRenderComponent(rdNode.get(), rdProto);
        rd->role = Role::Skybox;
        a->addComponent(ComponentId::Render, rd);
    }

    auto phNode = actorTree.get_child_optional("physics");
    if (phNode) {
        auto ph = getPhysicsComponent(phNode.get(), phProto);
        a->addComponent(ComponentId::Physics, ph);
    }

    m_prototypes[prototypeName] = std::move(a);
}

//------------------------------------------------------------------------------

std::unique_ptr<Actor> ActorFactory::create(boost::property_tree::ptree::value_type& v)
{
    auto a = std::make_unique<Actor>(getNextId());

    const std::string& actorType = v.first;
    ptree& actorTree             = v.second;

    TransformationComponent trProto;
    RenderComponent rdProto;
    PhysicsComponent phProto;

    auto prototypeNode = actorTree.get_child_optional("prototype");
    if (prototypeNode) {
        auto prototypeName = actorTree.get<std::string>("prototype");
        auto it            = m_prototypes.find(prototypeName);
        if (it != std::end(m_prototypes)) {
            const auto& p = *it;
            auto tr =
                p.second->getComponent<TransformationComponent>(ComponentId::Transformation).lock();
            if (tr) trProto = *tr;

            auto rd         = p.second->getComponent<RenderComponent>(ComponentId::Render).lock();
            if (rd) rdProto = *rd;

            auto ph         = p.second->getComponent<PhysicsComponent>(ComponentId::Physics).lock();
            if (ph) phProto = *ph;

        } else {
            LOG_WARNING << "Unkonown actor prototype: " << prototypeName;
        }
    }

    auto trNode = actorTree.get_child_optional("transformation");
    if (trNode) {
        auto tr = getTransformationComponent(trNode.get(), trProto);
        a->addComponent(ComponentId::Transformation, tr);
    }

    auto rdNode = actorTree.get_child_optional("render");
    if (rdNode) {
        auto rd  = getRenderComponent(rdNode.get(), rdProto);
        rd->role = Role::Skybox;
        a->addComponent(ComponentId::Render, rd);
    }

    auto phNode = actorTree.get_child_optional("physics");
    if (phNode) {
        auto ph = getPhysicsComponent(phNode.get(), phProto);
        a->addComponent(ComponentId::Physics, ph);
    }

    if (actorType == "skybox") {
        auto rd          = a->getComponent<RenderComponent>(ComponentId::Render).lock();
        if (rd) rd->role = Role::Skybox;
    } else if (actorType == "terrain") {
        auto rd          = a->getComponent<RenderComponent>(ComponentId::Render).lock();
        if (rd) rd->role = Role::Terrain;
    } else if (actorType == "actor") {
        auto rd          = a->getComponent<RenderComponent>(ComponentId::Render).lock();
        if (rd) rd->role = Role::Dynamic;
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
