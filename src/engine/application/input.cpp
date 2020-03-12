#include "engine/application/input.hpp"

#include "engine/core/memory.hpp"

Input::Input() noexcept
        : keyStates {0}
        , lastKeyStates {0}
        , mouseStates {0}
        , lastMouseStates {0}
        , mouseX(0.0)
        , mouseY(0.0)
        , lastMouseX(0.0)
        , lastMouseY(0.0) {}

void Input::on_key_callback(int keyCode, int state) noexcept {
    keyStates[keyCode] = state;
}

void Input::on_mouse_callback(int mouseButton, int state) noexcept {
    mouseStates[mouseButton] = state;
}

void Input::on_mouse_move(double mouseX, double mouseY) noexcept {
    this->mouseX = mouseX;
    this->mouseY = mouseY;
}

void Input::update() noexcept {
    Memory::memcpy(lastKeyStates, keyStates, sizeof(keyStates));
    Memory::memcpy(lastMouseStates, mouseStates, sizeof(mouseStates));

    lastMouseX = mouseX;
    lastMouseY = mouseY;
}

bool Input::is_key_down(int keyCode) const noexcept {
    return keyStates[keyCode];
}

bool Input::is_mouse_down(int mouseButton) const noexcept {
    return mouseStates[mouseButton];
}

bool Input::was_key_pressed(int keyCode) const noexcept {
    return keyStates[keyCode] && !lastKeyStates[keyCode];
}

bool Input::was_key_released(int keyCode) const noexcept {
    return !keyStates[keyCode] && lastKeyStates[keyCode];
}

bool Input::was_mouse_pressed(int mouseButton) const noexcept {
    return mouseStates[mouseButton] && !lastMouseStates[mouseButton];
}

bool Input::was_mouse_released(int mouseButton) const noexcept {
    return !mouseStates[mouseButton] && lastMouseStates[mouseButton];
}

double Input::getMouseX() const noexcept {
    return mouseX;
}

double Input::getMouseY() const noexcept {
    return mouseY;
}

double Input::getMouseDeltaX() const noexcept {
    return mouseX - lastMouseX;
}

double Input::getMouseDeltaY() const noexcept {
    return mouseY - lastMouseY;
}
