#include "InputSystem.h"

void InputSystem::update(float /*delta*/)
{
    for (auto keyVal : m_nodes) {
        ControlComponent* comp = keyVal.second;
        *comp                  = m_comp;
    }

    m_comp.axes = glm::vec4{0.f};
}

//------------------------------------------------------------------------------

void InputSystem::mouseMoved(const SDL_Event& event)
{
    float mouseSensitivity = 0.2f;

    if (isMouseRelativeMode()) {
        m_comp.axes.x = event.motion.xrel * mouseSensitivity;
        m_comp.axes.y = event.motion.yrel * mouseSensitivity;
    }
}

void InputSystem::mouseButtonPressed(const SDL_Event& event)
{
    switch (event.button.button) {
    case SDL_BUTTON_LEFT: m_comp.actions |= ControlComponent::Fire; break;
    case SDL_BUTTON_RIGHT: m_comp.actions |= ControlComponent::AltFire; break;
    }
}

void InputSystem::mouseButtonReleased(const SDL_Event& event)
{
    switch (event.button.button) {
    case SDL_BUTTON_LEFT: m_comp.actions &= ~(ControlComponent::Fire); break;
    case SDL_BUTTON_RIGHT: m_comp.actions &= ~(ControlComponent::AltFire); break;
    }
}

//------------------------------------------------------------------------------

void InputSystem::keyPressed(const SDL_Event& event)
{
    switch (event.key.keysym.scancode) {
    case SDL_SCANCODE_W: m_comp.actions |= ControlComponent::Forward; break;
    case SDL_SCANCODE_S: m_comp.actions |= ControlComponent::Back; break;
    case SDL_SCANCODE_A: m_comp.actions |= ControlComponent::StrafeLeft; break;
    case SDL_SCANCODE_D: m_comp.actions |= ControlComponent::StrafeRight; break;
    case SDL_SCANCODE_E: m_comp.actions |= ControlComponent::Up; break;
    case SDL_SCANCODE_C: m_comp.actions |= ControlComponent::Down; break;
    default: break;
    }
}

void InputSystem::keyReleased(const SDL_Event& event)
{
    switch (event.key.keysym.scancode) {
    case SDL_SCANCODE_W: m_comp.actions &= ~(ControlComponent::Forward); break;
    case SDL_SCANCODE_S: m_comp.actions &= ~(ControlComponent::Back); break;
    case SDL_SCANCODE_A: m_comp.actions &= ~(ControlComponent::StrafeLeft); break;
    case SDL_SCANCODE_D: m_comp.actions &= ~(ControlComponent::StrafeRight); break;
    case SDL_SCANCODE_E: m_comp.actions &= ~(ControlComponent::Up); break;
    case SDL_SCANCODE_C: m_comp.actions &= ~(ControlComponent::Down); break;
    default: break;
    }
}

//------------------------------------------------------------------------------

void InputSystem::setMouseRelativeMode(bool enable)
{
    if (enable) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
        SDL_ShowCursor(SDL_DISABLE);
    } else {
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    m_mouseRelativeMode = enable;
}
