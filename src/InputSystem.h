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
    InputSystem(InputSystem&&)                 = delete;
    InputSystem& operator=(InputSystem&&) = delete;

    void update(float delta);

    void addActor(int id, ControlComponent* ctrl) { m_nodes[id] = ctrl; }
    void removeActor(int id) { m_nodes.erase(id); }

    void mouseMoved(const SDL_Event& event);
    void mouseButtonPressed(const SDL_Event& event);
    void mouseButtonReleased(const SDL_Event& event);
    void keyPressed(const SDL_Event& event);
    void keyReleased(const SDL_Event& event);

    void setMouseRelativeMode(bool enable);
    bool isMouseRelativeMode() const { return m_mouseRelativeMode; }

    void setDebug(bool debug) { m_debug = debug; }

  private:
    std::map<int, ControlComponent*> m_nodes;
    ControlComponent m_comp; //< Holds all input events until update is called.
    bool m_debug = false;    //< Updates only objects used in debug

    bool m_mouseRelativeMode = false;
};

#endif // INPUTSYSTEM_H
