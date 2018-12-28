#include "ActorFactory.h"

#include "Logger.h"

#include <nlohmann/json.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

//using namespace boost::property_tree;

static glm::vec4 stringToVector(std::string str)
{
    glm::vec4 retval{1.0f};

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

static std::shared_ptr<RenderComponent> getRenderComponent(const nlohmann::json& node,
                                                           RenderComponent prototype)
{
    auto rd = std::make_shared<RenderComponent>();

    rd->mesh            = node.value("mesh", prototype.mesh);
    rd->material        = node.value("material", prototype.material);
    rd->shaderProgram   = node.value("shaderProgram", prototype.shaderProgram);
    rd->transparent     = node.value("transparent", prototype.transparent);
    rd->backfaceCulling = node.value("backfaceCulling", prototype.backfaceCulling);

    return rd;
}

//------------------------------------------------------------------------------

static std::shared_ptr<LightComponent> getLightComponent(const nlohmann::json& node,
                                                         LightComponent prototype)
{
    auto lt = std::make_shared<LightComponent>();

    lt->castsShadows = node.value("castsShadows", prototype.castsShadows);
    lt->material     = node.value("material", prototype.material);

    return lt;
}

//------------------------------------------------------------------------------

static std::shared_ptr<TransformationComponent>
getTransformationComponent(const nlohmann::json& node, TransformationComponent prototype)
{
    auto tr              = std::make_shared<TransformationComponent>();
    std::string rotation = node.value("orientation", "");
    if (!rotation.empty()) {
        // Conversion form euler to quaternion
        auto degs    = glm::vec3{stringToVector(rotation)};
        tr->rotation = glm::quat{glm::radians(degs)};
    } else {
        tr->rotation = prototype.rotation;
    }

    std::string translation = node.value("position", "");
    if (!translation.empty()) {
        tr->translation = glm::vec3{stringToVector(translation)};
    } else {
        tr->translation = prototype.translation;
    }

    std::string scale = node.value("scale", "");
    if (!scale.empty()) {
        tr->scale = glm::vec3{stringToVector(scale)};
    } else {
        tr->scale = prototype.scale;
    }

    return tr;
}

//------------------------------------------------------------------------------

static std::shared_ptr<PhysicsComponent> getPhysicsComponent(const nlohmann::json& node,
                                                             PhysicsComponent prototype)
{
    auto ph = std::make_shared<PhysicsComponent>();

    ph->shape = node.value("shape", prototype.shape);
    ph->mass  = node.value("mass", prototype.mass);

    return ph;
}

//------------------------------------------------------------------------------

static std::shared_ptr<ScriptComponent> getScriptComponent(const nlohmann::json& node,
                                                           ScriptComponent prototype)
{
    auto sc = std::make_shared<ScriptComponent>();

    sc->name = node.at(prototype.name).get<std::string>();

    return sc;
}

//------------------------------------------------------------------------------

void ActorFactory::registerPrototype(const nlohmann::json& node)
{
    //const std::string& actorType = v.first;
    //ptree& actorTree             = v.second;

    //assert(actorType == "actorPrototype");
    auto a = std::make_unique<Actor>(0);

    const auto& prototypeName = node.at("name").get<std::string>();

    TransformationComponent trProto;
    RenderComponent rdProto;
    LightComponent ltProto;
    PhysicsComponent phProto;
    ScriptComponent scProto;

	auto trNode = node.find("transformation");
    if (trNode != node.cend()) {
        auto tr = getTransformationComponent(*trNode, trProto);
        a->addComponent(ComponentId::Transformation, tr);
    }

	auto rdNode = node.find("render");
	if (rdNode != node.cend())
    {
        auto rd = getRenderComponent(*rdNode, rdProto);
        a->addComponent(ComponentId::Render, rd);
    }

	auto ltNode = node.find("light");
    if (ltNode != node.cend()) {
        auto lt = getLightComponent(*ltNode, ltProto);
        a->addComponent(ComponentId::Light, lt);
    }

	auto phNode = node.find("physics");
    if (phNode != node.cend()) {
        auto ph = getPhysicsComponent(*phNode, phProto);
        a->addComponent(ComponentId::Physics, ph);
    }

	auto scNode = node.find("script");
    if (scNode != node.cend()) {
        auto sc = getScriptComponent(*scNode, scProto);
        a->addComponent(ComponentId::Script, sc);
    }

    m_prototypes[prototypeName] = std::move(a);
}

//------------------------------------------------------------------------------

std::unique_ptr<Actor> ActorFactory::create(const nlohmann::json& node)
{
    auto a = std::make_unique<Actor>(getNextId());

    //const std::string& actorType = v.first;
    //ptree& actorTree             = v.second;

    TransformationComponent trProto;
    RenderComponent rdProto;
    LightComponent ltProto;
    PhysicsComponent phProto;
    ScriptComponent scProto;

	auto prototypeNode = node.find("prototype");
	if (prototypeNode != node.cend()) {
        std::string prototypeName = *prototypeNode;
        auto it            = m_prototypes.find(prototypeName);
        if (it != std::end(m_prototypes)) {
            const auto& p = *it;
            auto tr =
                p.second->getComponent<TransformationComponent>(ComponentId::Transformation).lock();
            if (tr) trProto = *tr;

            auto rd = p.second->getComponent<RenderComponent>(ComponentId::Render).lock();
            if (rd) rdProto = *rd;

            auto lt = p.second->getComponent<LightComponent>(ComponentId::Light).lock();
            if (lt) ltProto = *lt;

            auto ph = p.second->getComponent<PhysicsComponent>(ComponentId::Physics).lock();
            if (ph) phProto = *ph;

            auto sc = p.second->getComponent<ScriptComponent>(ComponentId::Script).lock();
            if (sc) scProto = *sc;

        } else {
            LOG_WARNING("Unkonown actor prototype: {}", prototypeName);
        }
    }

	auto trNode = node.find("transformation");
    if (trNode != node.cend()) {
        auto tr = getTransformationComponent(*trNode, trProto);
        a->addComponent(ComponentId::Transformation, tr);
    }

    auto rdNode = node.find("render");
    if (rdNode != node.cend()) {
        auto rd = getRenderComponent(*rdNode, rdProto);
        a->addComponent(ComponentId::Render, rd);
    }

    auto ltNode = node.find("light");
    if (ltNode != node.cend()) {
        auto lt = getLightComponent(*ltNode, ltProto);
        a->addComponent(ComponentId::Light, lt);
    }

    auto phNode = node.find("physics");
    if (phNode != node.cend()) {
        auto ph = getPhysicsComponent(*phNode, phProto);
        a->addComponent(ComponentId::Physics, ph);
    }

    auto scNode = node.find("script");
    if (scNode != node.cend()) {
        auto sc = getScriptComponent(*scNode, scProto);
        a->addComponent(ComponentId::Script, sc);
    }

	auto ctrlNode = node.find("control");
    if (ctrlNode != node.cend()) {
        a->addComponent(ComponentId::Control, std::make_shared<ControlComponent>());
    }

    //if (actorType == "skybox") {
    //    auto rd = a->getComponent<RenderComponent>(ComponentId::Render).lock();
    //    if (rd) rd->role = Role::Skybox;
    //}

    return a;
}

//--------------------------------------------------------------------------

unsigned int ActorFactory::getNextId()
{
    static unsigned int id = 0;
    ++id;
    return id;
}
