#include "GameLogic.h"
#include "ActorFactory.h"
#include "PhysicsSystem.h"

#include <boost/algorithm/string/predicate.hpp>

/*
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
    };*/

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
            auto tr   = a->getComponent<TransformationComponent>(ComponentId::Transformation);
            auto rd   = a->getComponent<RenderComponent>(ComponentId::Render);
            auto ctrl = a->getComponent<ControlComponent>(ComponentId::Control);

            auto str   = tr.lock();
            auto srd   = rd.lock();
            auto sctrl = ctrl.lock();
            if (srd) gv->addActor(a->id(), str.get(), srd.get(), sctrl.get());
        }
    }

    for (auto& a : m_actors) {
        auto tr  = a->getComponent<TransformationComponent>(ComponentId::Transformation);
        auto ph  = a->getComponent<PhysicsComponent>(ComponentId::Physics);
        auto str = tr.lock();
        auto sph = ph.lock();
        if (str && sph) m_physicsSystem->addActor(a->id(), str.get(), sph.get(), *m_resourcesMgr);
    }
}

void GameLogic::onAfterMainLoop(Engine* /*e*/)
{
    for (auto& gv : m_gameViews) {
        for (auto& a : m_actors) {
            auto tr  = a->getComponent<TransformationComponent>(ComponentId::Transformation);
            auto rd  = a->getComponent<RenderComponent>(ComponentId::Render);
            auto str = tr.lock();
            auto srd = rd.lock();
            if (srd) gv->removeActor(a->id());
        }
    }

    for (auto& a : m_actors) {
        auto tr  = a->getComponent<TransformationComponent>(ComponentId::Transformation);
        auto ph  = a->getComponent<PhysicsComponent>(ComponentId::Physics);
        auto str = tr.lock();
        auto sph = ph.lock();
        if (str && sph) m_physicsSystem->removeActor(a->id());
    }
}

//------------------------------------------------------------------------------

void GameLogic::update(float elapsedTime)
{
    for (auto& a : m_actors) {
        auto ctrl  = a->getComponent<ControlComponent>(ComponentId::Control);
        auto ph    = a->getComponent<PhysicsComponent>(ComponentId::Physics);
        auto sctrl = ctrl.lock();
        auto sph   = ph.lock();
        if (sctrl && sph) {
            sph->force  = glm::vec3{0.f, 9.81f * sph->mass, 0.f};
            sph->torque = glm::vec3{};

            if (sctrl->actions & ControlComponent::Forward) {
                sph->force.z += sph->maxForce.z;
            }
            if (sctrl->actions & ControlComponent::Back) {
                sph->force.z -= sph->maxForce.z;
            }
            if (sctrl->actions & ControlComponent::Up) {
                sph->force.y += sph->maxForce.y;
            }
            if (sctrl->actions & ControlComponent::Down) {
                sph->force.y -= sph->maxForce.y;
            }
            if (sctrl->actions & ControlComponent::StrafeRight) {
                // sph->force.x -= sph->maxForce.x;
                sph->torque.y -= 200;
            }
            if (sctrl->actions & ControlComponent::StrafeLeft) {
                // sph->force.x += sph->maxForce.x;
                sph->torque.y += 200;
            }
            sph->torque.x = -sctrl->axes.y * 200;
            sph->torque.z = sctrl->axes.x * 200;
        }
    }

    m_physicsSystem->update(elapsedTime);
}

//------------------------------------------------------------------------------

void GameLogic::debugDraw()
{
    if (m_drawDebug) m_physicsSystem->drawDebugData();
}
