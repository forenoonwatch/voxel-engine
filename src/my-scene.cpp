#include "my-scene.hpp"

#include <engine/core/hashed-string.hpp>

#include <engine/ecs/ecs.hpp>

#include <engine/resource/resource-cache.hpp>
#include <engine/resource/shader-loader.hpp>
#include <engine/resource/vertex-array-loader.hpp>

#include <engine/application/application.hpp>

#include <engine/rendering/render-target.hpp>

#include <engine/math/math.hpp>
#include <engine/math/matrix.hpp>

#include "camera.hpp"
#include "camera-controller.hpp"
#include "player-input.hpp"

#include "chunk-manager.hpp"

void MyScene::load() {
    ResourceCache<Shader>::getInstance().load<ShaderLoader>("basic-shader"_hs,
        getEngine()->getRenderContext(), "./res/shaders/basic-shader.glsl");

    IndexedModel::AllocationHints hints;
    hints.elementSizes.push_back(3);
	hints.elementSizes.push_back(2);
	hints.elementSizes.push_back(3);
	hints.elementSizes.push_back(3);
	hints.elementSizes.push_back(16);
	hints.instancedElementStartIndex = 4;

    ResourceCache<VertexArray>::getInstance().load<VertexArrayLoader>("cube"_hs,
            getEngine()->getRenderContext(), "./res/models/cube.obj",
            hints);

    screen = new RenderTarget(getEngine()->getRenderContext(),
            getEngine()->getApplication().getWidth(),
            getEngine()->getApplication().getHeight());

    auto& registry = getEngine()->getRegistry();

    auto eCam = registry.create();
    registry.assign<Camera>(eCam, Math::perspective(Math::toRadians(70.f),
            4.f / 3.f, 0.1f, 1000.f),
            Math::translate(Matrix4f(1.f), Vector3f(0.f, 0.f, 2.f)));
    registry.assign<CameraController>(eCam, Vector3f(0.f, 0.f, 2.f),
            0.f, 0.f, 15.f);
    registry.assign<PlayerInputComponent>(eCam);

    chunkManager = new ChunkManager(getEngine()->getRenderContext(), 4);
}

void MyScene::update(float deltaTime) {
    update_player_input(getEngine()->getRegistry(), getEngine()->getInput());
    update_camera_controller(getEngine()->getRegistry(), deltaTime);
    update_camera_system(getEngine()->getRegistry());
}

void MyScene::render() {
    screen->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto shader = ResourceCache<Shader>::getInstance().handle("basic-shader"_hs);
    auto cube = ResourceCache<VertexArray>::getInstance().handle("cube"_hs);
    auto& context = getEngine()->getRenderContext();

    auto* cam = getEngine()->getRegistry().raw<Camera>();
    //cube->updateBuffer(4, &cam->viewProjection, sizeof(Matrix4f));

    chunkManager->load_chunks(*cam);
    chunkManager->render_chunks(*screen, shader, *cam);
}

void MyScene::unload() {
    delete chunkManager;
    delete screen;
}