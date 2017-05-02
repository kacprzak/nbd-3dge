#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "Settings.h"
#include "GameView.h"
#include "PhysicsSystem.h"
#include "Actor.h"

#include <boost/utility.hpp>
#include <list>
#include <memory>

class Engine;

class GameLogic : private boost::noncopyable
{
    using GameViewList = std::list<std::shared_ptr<GameView>>;
    using ActorsList = std::list<std::shared_ptr<Actor>>;
 public:
    GameLogic(const Settings& settings);
    virtual ~GameLogic() {}

    virtual void update(float elapsedTime);

    GameViewList& gameViews() { return m_gameViews; }
    /*!  Use these if you need Engine to create or clean stuff */
    void onBeforeMainLoop(Engine* e);
    void onAfterMainLoop(Engine* e);

    void attachView(std::shared_ptr<GameView> gameView, unsigned actorId = 0);
    
 private:
    PhysicsSystem m_physicsSystem;
    
    Settings m_settings;
    GameViewList m_gameViews;
    ActorsList m_actors;

};

#endif // GAMELOGIC_H
