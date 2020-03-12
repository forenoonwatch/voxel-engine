#include <engine/scene/core-engine.hpp>

#include <engine/application/application.hpp>
#include <engine/application/input.hpp>

#include <engine/ecs/ecs.hpp>

#include <engine/rendering/render-context.hpp>

#include "my-scene.hpp"

CoreEngine coreEngine;
Input input;

void on_signal(int);
void on_key_callback(WindowHandle, int, int, int, int);
void on_mouse_callback(WindowHandle, int, int, int);
void on_mouse_move(WindowHandle, double, double);
void on_window_closed(WindowHandle);

int main() {
    Application application("Voxel Engine", 800, 600);
    Registry registry;
    RenderContext renderContext;

    application.setKeyCallback(on_key_callback);
    application.setMouseButtonCallback(on_mouse_callback);
    application.setWindowCloseCallback(on_window_closed);
    application.setCursorPosCallback(on_mouse_move);
    application.setInterruptCallback(on_signal);

    coreEngine.init(application, input, registry, renderContext);
    coreEngine.load<MyScene>();

    return 0;
}

void on_signal(int) {
    coreEngine.stop();
}

void on_key_callback(WindowHandle, int keyCode, int scanCode, int action, int mods) {
    input.on_key_callback(keyCode, action);
}

void on_mouse_callback(WindowHandle win, int button, int action, int mods) {
    input.on_mouse_callback(button, action);
}

void on_mouse_move(WindowHandle win, double mouseX, double mouseY) {
    input.on_mouse_move(mouseX, mouseY);
}

void on_window_closed(WindowHandle win) {
    coreEngine.stop();
}
