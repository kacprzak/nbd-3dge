#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "Components.h"

#include <SDL.h>
#include <map>

class InputSystem
{
  public:
    InputSystem()                   = default;
    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;

    void addActor(int id, ControlComponent* ctrl) { m_nodes[id] = ctrl; }
    void removeActor(int id) { m_nodes.erase(id); }

    void mouseMoved(const SDL_Event& event);
    void mouseButtonPressed(const SDL_Event& event);
    void mouseButtonReleased(const SDL_Event& event);
    void keyPressed(const SDL_Event& event);
    void keyReleased(const SDL_Event& event);

  private:
    std::map<int, ControlComponent*> m_nodes;
};

#endif // INPUTSYSTEM_H
