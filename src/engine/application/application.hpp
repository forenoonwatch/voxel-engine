#pragma once

#include <engine/core/common.hpp>
#include <engine/core/string.hpp>

class Input;

class GLFWwindow;
typedef GLFWwindow* WindowHandle;

class Application final {
    public:
        typedef void (*KeyCallback)(WindowHandle, int, int, int, int);
        typedef void (*MouseButtonCallback)(WindowHandle, int, int, int);
        typedef void (*WindowCloseCallback)(WindowHandle);
        typedef void (*CursorPosCallback)(WindowHandle, double, double);
        typedef void (*SignalCallback)(int);

        Application(const StringView& title, int width, int height) noexcept;

        void pollEvents() noexcept;
        void swapBuffers() noexcept;

        void setKeyCallback(KeyCallback callback) noexcept;
        void setMouseButtonCallback(MouseButtonCallback callback) noexcept;
        void setWindowCloseCallback(WindowCloseCallback callback) noexcept;
        void setCursorPosCallback(CursorPosCallback callback) noexcept;
        void setInterruptCallback(SignalCallback callback) noexcept;

        WindowHandle getHandle() noexcept;

        int getWidth() const noexcept;
        int getHeight() const noexcept;

        ~Application();
    private:
        NULL_COPY_AND_ASSIGN(Application);

        WindowHandle handle;

        int width;
        int height;
};