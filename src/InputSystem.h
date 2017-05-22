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

    void update(float delta);

    void addActor(int id, ControlComponent* ctrl) { m_nodes[id] = ctrl; }
    void removeActor(int id) { m_nodes.erase(id); }

    //! If ctrl is not nullptr then all events will be passed to camera only.
    void setDebugCamera(ControlComponent* ctrl) { m_debugCamera = ctrl; }

    void mouseMoved(const SDL_Event& event);
    void mouseButtonPressed(const SDL_Event& event);
    void mouseButtonReleased(const SDL_Event& event);
    void keyPressed(const SDL_Event& event);
    void keyReleased(const SDL_Event& event);

    void setMouseRelativeMode(bool enable);
    bool isMouseRelativeMode() const { return m_mouseRelativeMode; }

  private:
    std::map<int, ControlComponent*> m_nodes;
    ControlComponent m_comp; //< Holds all input events until update is called.
    ControlComponent* m_debugCamera = nullptr;

    bool m_mouseRelativeMode = false;
};

#endif // INPUTSYSTEM_H
