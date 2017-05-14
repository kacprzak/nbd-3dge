#include "InputSystem.h"

void InputSystem::mouseMoved(const SDL_Event& event)
{
    float mouseSensitivity = 0.2f;

    for (auto keyVal : m_nodes) {
        ControlComponent* comp = keyVal.second;
        comp->axes.x           = event.motion.xrel * mouseSensitivity;
        comp->axes.y           = event.motion.yrel * mouseSensitivity;
    }
}

void InputSystem::mouseButtonPressed(const SDL_Event& event)
{
    for (auto keyVal : m_nodes) {
        ControlComponent* comp = keyVal.second;
        switch (event.button.button) {
        case SDL_BUTTON_LEFT: comp->actions |= ControlComponent::Fire; break;
        case SDL_BUTTON_RIGHT: comp->actions |= ControlComponent::AltFire; break;
        }
    }
}

void InputSystem::mouseButtonReleased(const SDL_Event& event)
{
    for (auto keyVal : m_nodes) {
        ControlComponent* comp = keyVal.second;
        switch (event.button.button) {
        case SDL_BUTTON_LEFT: comp->actions &= ~(ControlComponent::Fire); break;
        case SDL_BUTTON_RIGHT: comp->actions &= ~(ControlComponent::AltFire); break;
        }
    }
}

void InputSystem::keyPressed(const SDL_Event& event)
{
    for (auto keyVal : m_nodes) {
        ControlComponent* comp = keyVal.second;
        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_W: comp->actions |= ControlComponent::Forward; break;
        case SDL_SCANCODE_S: comp->actions |= ControlComponent::Back; break;
        case SDL_SCANCODE_A: comp->actions |= ControlComponent::StrafeLeft; break;
        case SDL_SCANCODE_D: comp->actions |= ControlComponent::StrafeRight; break;
        case SDL_SCANCODE_E: comp->actions |= ControlComponent::Up; break;
        case SDL_SCANCODE_C: comp->actions |= ControlComponent::Down; break;
        default: break;
        }
    }
}

void InputSystem::keyReleased(const SDL_Event& event)
{
    for (auto keyVal : m_nodes) {
        ControlComponent* comp = keyVal.second;
        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_W: comp->actions &= ~(ControlComponent::Forward); break;
        case SDL_SCANCODE_S: comp->actions &= ~(ControlComponent::Back); break;
        case SDL_SCANCODE_A: comp->actions &= ~(ControlComponent::StrafeLeft); break;
        case SDL_SCANCODE_D: comp->actions &= ~(ControlComponent::StrafeRight); break;
        case SDL_SCANCODE_E: comp->actions &= ~(ControlComponent::Up); break;
        case SDL_SCANCODE_C: comp->actions &= ~(ControlComponent::Down); break;
        default: break;
        }
    }
}

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
