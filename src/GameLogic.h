#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "Actor.h"
#include "GameView.h"
#include "Settings.h"

#include <boost/utility.hpp>
#include <list>
#include <memory>

class Engine;
class PhysicsSystem;

class GameLogic final : private boost::noncopyable
{
    using GameViewList = std::list<std::shared_ptr<GameView>>;
    using ActorsList   = std::list<std::shared_ptr<Actor>>;

  public:
    GameLogic(const Settings& settings);
    ~GameLogic();

    void update(float elapsedTime);
    void draw();

    GameViewList& gameViews() { return m_gameViews; }
    /*!  Use these if you need Engine to create or clean stuff */
    void onBeforeMainLoop(Engine* e);
    void onAfterMainLoop(Engine* e);

    void attachView(std::shared_ptr<GameView> gameView, unsigned actorId = 0);

    void toggleDrawDebug() { m_drawDebug = !m_drawDebug; }

  private:
    const Settings m_settings;
    std::unique_ptr<PhysicsSystem> m_physicsSystem;
    GameViewList m_gameViews;
    ActorsList m_actors;
    bool m_drawDebug = false;
};

#endif // GAMELOGIC_H
