#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "Components.h"

#include <SDL.h>
#include <boost/utility.hpp>

class GameView : private boost::noncopyable
{
  public:
    virtual ~GameView() = default;

    virtual bool processInput(const SDL_Event& event) = 0;
    virtual void update(float elapsedTime)            = 0;
    virtual void draw()                               = 0;

    /*! Callbacks */
    virtual void onAttach(int /*gameViewId*/, unsigned long /*actorId*/) {}

    virtual void loadResources(const std::string& xmlFile) = 0;
    virtual void unloadResources()                         = 0;

    virtual void addActor(int id, TransformationComponent* tr, RenderComponent* rd) = 0;
    virtual void removeActor(int id) = 0;
};

#endif // GAMEVIEW_H
