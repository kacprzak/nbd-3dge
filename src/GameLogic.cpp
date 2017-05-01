#include "GameLogic.h"
#include "ActorFactory.h"

GameLogic::GameLogic(const Settings& settings)
    : m_settings{settings}
{
}

void GameLogic::attachView(std::shared_ptr<GameView> gameView, unsigned actorId)
{
    int viewId = m_gameViews.size();
    m_gameViews.push_back(gameView);
    gameView->onAttach(viewId, actorId);
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
    
    for (ptree::value_type& v : pt.get_child("scene")) {
        auto a = ActorFactory::create(v);
        m_actors.push_back(std::move(a));
    }

    for (auto& gv : m_gameViews) {
        for (auto& a : m_actors) {
            auto tr = a->getComponent<TransformationComponent>(ComponentId::Transformation);
            auto rd = a->getComponent<RenderComponent>(ComponentId::Render);
            auto str = tr.lock();
            auto srd = rd.lock();
            if (srd)
                gv->addActor(a->id(), str.get(), srd.get());
        }
    }
}


void GameLogic::onAfterMainLoop(Engine* /*e*/) {}
