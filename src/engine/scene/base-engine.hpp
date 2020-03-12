#pragma once

class Application;
class Input;
class Registry;
class RenderContext;

class BaseEngine {
    public:
        BaseEngine(); 

        void init(Application& application, Input& input,
                Registry& registry, RenderContext& renderContext);

        void update(float deltaTime) noexcept;
        void postUpdate(float deltaTime) noexcept;

        void render() noexcept;

        Application& getApplication() noexcept;
        Input& getInput() noexcept;
        Registry& getRegistry() noexcept;
        RenderContext& getRenderContext() noexcept;

        virtual ~BaseEngine();
    private:
        Application* application;
        Input* input;
        Registry* registry;
        RenderContext* renderContext;
};
