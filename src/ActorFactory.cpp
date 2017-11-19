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

    rd->mesh            = actorTree.get("mesh", prototype.mesh);
    rd->material        = actorTree.get("material", prototype.material);
    rd->shaderProgram   = actorTree.get("shaderProgram", prototype.shaderProgram);
    rd->transparent     = actorTree.get("transparent", prototype.transparent);
    rd->backfaceCulling = actorTree.get("backfaceCulling", prototype.backfaceCulling);

    return rd;
}

//------------------------------------------------------------------------------

static std::shared_ptr<LightComponent> getLightComponent(ptree& actorTree, LightComponent prototype)
{
    auto lt = std::make_shared<LightComponent>();

    lt->castsShadows = actorTree.get("castsShadows", prototype.castsShadows);
    lt->material     = actorTree.get("material", prototype.material);

    return lt;
}

//------------------------------------------------------------------------------

static std::shared_ptr<TransformationComponent>
getTransformationComponent(ptree& actorTree, TransformationComponent prototype)
{
    auto tr                 = std::make_shared<TransformationComponent>();
    std::string orientation = actorTree.get("orientation", "");
    if (!orientation.empty()) {
        // Conversion form euler to quaternion
        tr->orientation = glm::quat{glm::vec3{stringToVector(orientation)}};
    } else {
        tr->orientation = prototype.orientation;
    }

    std::string position = actorTree.get("position", "");
    if (!position.empty()) {
        tr->position = glm::vec3{stringToVector(position)};
    } else {
        tr->position = prototype.position;
    }

    tr->scale = actorTree.get("scale", prototype.scale);

    return tr;
}

//------------------------------------------------------------------------------

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
    LightComponent ltProto;
    PhysicsComponent phProto;

    if (auto trNode = actorTree.get_child_optional("transformation")) {
        auto tr = getTransformationComponent(trNode.get(), trProto);
        a->addComponent(ComponentId::Transformation, tr);
    }

    if (auto rdNode = actorTree.get_child_optional("render")) {
        auto rd = getRenderComponent(rdNode.get(), rdProto);
        a->addComponent(ComponentId::Render, rd);
    }

    if (auto ltNode = actorTree.get_child_optional("light")) {
        auto lt = getLightComponent(ltNode.get(), ltProto);
        a->addComponent(ComponentId::Light, lt);
    }

    if (auto phNode = actorTree.get_child_optional("physics")) {
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
    LightComponent ltProto;
    PhysicsComponent phProto;

    if (auto prototypeNode = actorTree.get_child_optional("prototype")) {
        auto prototypeName = actorTree.get<std::string>("prototype");
        auto it            = m_prototypes.find(prototypeName);
        if (it != std::end(m_prototypes)) {
            const auto& p = *it;
            auto tr =
                p.second->getComponent<TransformationComponent>(ComponentId::Transformation).lock();
            if (tr) trProto = *tr;

            auto rd         = p.second->getComponent<RenderComponent>(ComponentId::Render).lock();
            if (rd) rdProto = *rd;

            auto lt         = p.second->getComponent<LightComponent>(ComponentId::Light).lock();
            if (lt) ltProto = *lt;

            auto ph         = p.second->getComponent<PhysicsComponent>(ComponentId::Physics).lock();
            if (ph) phProto = *ph;

        } else {
            LOG_WARNING << "Unkonown actor prototype: " << prototypeName;
        }
    }

    if (auto trNode = actorTree.get_child_optional("transformation")) {
        auto tr = getTransformationComponent(trNode.get(), trProto);
        a->addComponent(ComponentId::Transformation, tr);
    }

    if (auto rdNode = actorTree.get_child_optional("render")) {
        auto rd = getRenderComponent(rdNode.get(), rdProto);
        a->addComponent(ComponentId::Render, rd);
    }

    if (auto ltNode = actorTree.get_child_optional("light")) {
        auto lt = getLightComponent(ltNode.get(), ltProto);
        a->addComponent(ComponentId::Light, lt);
    }

    if (auto phNode = actorTree.get_child_optional("physics")) {
        auto ph = getPhysicsComponent(phNode.get(), phProto);
        a->addComponent(ComponentId::Physics, ph);
    }

    if (auto ctrlNode = actorTree.get_child_optional("control")) {
        a->addComponent(ComponentId::Control, std::make_shared<ControlComponent>());
    }

    if (actorType == "skybox") {
        auto rd          = a->getComponent<RenderComponent>(ComponentId::Render).lock();
        if (rd) rd->role = Role::Skybox;
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
