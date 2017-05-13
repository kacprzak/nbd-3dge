#include "InputSystem.h"

void InputSystem::mouseMoved(const SDL_Event& event) {}
void InputSystem::mouseButtonPressed(const SDL_Event& event) {}
void InputSystem::mouseButtonReleased(const SDL_Event& event) {}

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
