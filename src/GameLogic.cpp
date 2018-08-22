#include "GameLogic.h"
#include "ActorFactory.h"
#include "PhysicsSystem.h"

#include <boost/algorithm/string/predicate.hpp>

class RotationScript : public Script
{
  public:
    void execute(float elapsedTime, Actor* a) override
    {
        auto trWeak = a->getComponent<TransformationComponent>(ComponentId::Transformation);
        if (auto tr = trWeak.lock()) {
            tr->rotation = glm::rotate(tr->rotation, elapsedTime * 0.55f, {0.f, 1.f, 0.f});
        }
    }
};

class ControlScript : public Script
{
  public:
    void execute(float /*elapsedTime*/, Actor* a) override
    {
        auto ctrl = a->getComponent<ControlComponent>(ComponentId::Control).lock();
        auto ph   = a->getComponent<PhysicsComponent>(ComponentId::Physics).lock();

        if (ctrl && ph) {
            ph->force  = glm::vec3{0.f, 9.81f * ph->mass, 0.f};
            ph->torque = glm::vec3{};

            if (ctrl->actions & ControlComponent::Forward) {
                ph->force.z += ph->maxForce.z;
            }
            if (ctrl->actions & ControlComponent::Back) {
                ph->force.z -= ph->maxForce.z;
            }
            if (ctrl->actions & ControlComponent::Up) {
                ph->force.y += ph->maxForce.y;
            }
            if (ctrl->actions & ControlComponent::Down) {
                ph->force.y -= ph->maxForce.y;
            }
            if (ctrl->actions & ControlComponent::StrafeRight) {
                // ph->force.x -= ph->maxForce.x;
                ph->torque.y -= 200;
            }
            if (ctrl->actions & ControlComponent::StrafeLeft) {
                // ph->force.x += ph->maxForce.x;
                ph->torque.y += 200;
            }
            ph->torque.x = -ctrl->axes.y * 200;
            ph->torque.z = ctrl->axes.x * 200;
        }
    }
};

//==============================================================================

GameLogic::GameLogic(const Settings& settings, const std::shared_ptr<ResourcesMgr>& resourcesMgr)
    : m_settings{settings}
    , m_resourcesMgr(resourcesMgr)
    , m_physicsSystem{new PhysicsSystem}
{
    if (!m_resourcesMgr) {
        m_resourcesMgr =
            std::make_shared<ResourcesMgr>(m_settings.dataFolder, m_settings.shadersFolder);
    }

    m_resourcesMgr->addScript("sun_script", std::make_shared<RotationScript>());
}

GameLogic::~GameLogic() {}

//------------------------------------------------------------------------------

void GameLogic::attachView(std::shared_ptr<GameView> gameView, unsigned actorId)
{
    int viewId = m_gameViews.size();
    m_gameViews.push_back(gameView);
    gameView->onAttach(viewId, actorId);

    m_physicsSystem->setDebugDrawer(gameView.get()->debugDrawer());
}

void GameLogic::onBeforeMainLoop(Engine* /*e*/)
{
    // Load scene
       using boost::property_tree::ptree;
    ptree pt;

    read_xml(m_settings.dataFolder + "scene.xml", pt);
    const auto& assetsXml = pt.get<std::string>("scene.assets");

    for (auto& gv : m_gameViews) {
        gv->loadResources(assetsXml);
    }

    /*
    ActorFactory factory;
    for (ptree::value_type& v : pt.get_child("scene")) {
        if (boost::algorithm::ends_with(v.first, "Prototype")) {
            factory.registerPrototype(v);
        } else {
            auto a = factory.create(v);
            m_actors.push_back(std::move(a));
        }
    }

    for (auto& gv : m_gameViews) {
        for (auto& a : m_actors) {
            auto tr = a->getComponent<TransformationComponent>(ComponentId::Transformation).lock();
            auto rd = a->getComponent<RenderComponent>(ComponentId::Render).lock();
            auto lt = a->getComponent<LightComponent>(ComponentId::Light).lock();
            auto ctrl = a->getComponent<ControlComponent>(ComponentId::Control).lock();

            if (rd || lt) gv->addActor(a->id(), tr.get(), rd.get(), lt.get(), ctrl.get());
        }
    }

    for (auto& a : m_actors) {
        auto tr = a->getComponent<TransformationComponent>(ComponentId::Transformation).lock();
        auto ph = a->getComponent<PhysicsComponent>(ComponentId::Physics).lock();

        if (tr && ph) m_physicsSystem->addActor(a->id(), tr.get(), ph.get(), *m_resourcesMgr);
    }
    */
}

void GameLogic::onAfterMainLoop(Engine* /*e*/)
{
    for (auto& a : m_actors) {
        for (auto& gv : m_gameViews) {
            gv->removeActor(a->id());
        }

        m_physicsSystem->removeActor(a->id());
    }
}

//------------------------------------------------------------------------------

void GameLogic::update(float elapsedTime)
{
    for (auto& a : m_actors) {
        ControlScript ctrlScript;
        ctrlScript.execute(elapsedTime, a.get());

        auto sc = a->getComponent<ScriptComponent>(ComponentId::Script).lock();
        if (sc) {
            auto script = m_resourcesMgr->getScript(sc->name);
            script->execute(elapsedTime, a.get());
        }
    }

    m_physicsSystem->update(elapsedTime);
}

//------------------------------------------------------------------------------

void GameLogic::debugDraw()
{
    if (m_drawDebug) m_physicsSystem->drawDebugData();
}
