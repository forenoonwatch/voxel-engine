#include "engine/application/application.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <csignal>

Application::Application(const StringView& title, int width, int height) noexcept
        : handle(nullptr)
        , width(width)
        , height(height) {
    glfwInit();

    handle = glfwCreateWindow(width, height, title.begin(), nullptr, nullptr);
    glfwMakeContextCurrent(handle);

    glewInit();
}

void Application::pollEvents() noexcept {
    glfwPollEvents();
}

void Application::swapBuffers() noexcept {
    glfwSwapBuffers(handle);
}

void Application::setKeyCallback(KeyCallback callback) noexcept {
    glfwSetKeyCallback(handle, callback);
}

void Application::setMouseButtonCallback(MouseButtonCallback callback) noexcept {
    glfwSetMouseButtonCallback(handle, callback);
}

void Application::setWindowCloseCallback(WindowCloseCallback callback) noexcept {
    glfwSetWindowCloseCallback(handle, callback);
}

void Application::setCursorPosCallback(CursorPosCallback callback) noexcept {
    glfwSetCursorPosCallback(handle, callback);
}

void Application::setInterruptCallback(SignalCallback callback) noexcept {
    std::signal(SIGINT, callback);
}

WindowHandle Application::getHandle() noexcept {
    return handle;
}

int Application::getWidth() const noexcept {
    return width;
}

int Application::getHeight() const noexcept {
    return height;
}

Application::~Application() {
    glfwTerminate();
}
