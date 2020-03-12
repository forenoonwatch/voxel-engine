#include "engine/scene/base-engine.hpp"

#include "engine/application/application.hpp"
#include "engine/application/input.hpp"

#include "engine/ecs/registry.hpp"

#include "engine/rendering/render-context.hpp"

BaseEngine::BaseEngine() {}

void BaseEngine::init(Application& application, Input& input,
        Registry& registry, RenderContext& renderContext) {
    this->application = &application;
    this->input = &input;
    this->registry = &registry;
    this->renderContext = &renderContext;
}

void BaseEngine::update(float deltaTime) noexcept {
    application->pollEvents();
}

void BaseEngine::postUpdate(float deltaTime) noexcept {
    input->update();
}

void BaseEngine::render() noexcept {
    application->swapBuffers();
}

Application& BaseEngine::getApplication() noexcept {
    return *application;
}

Input& BaseEngine::getInput() noexcept {
    return *input;
}

Registry& BaseEngine::getRegistry() noexcept {
    return *registry;
}

RenderContext& BaseEngine::getRenderContext() noexcept {
    return *renderContext;
}

BaseEngine::~BaseEngine() {

}
