#include "my-scene.hpp"

#include <engine/core/hashed-string.hpp>

#include <engine/ecs/ecs.hpp>

#include <engine/resource/resource-cache.hpp>
#include <engine/resource/shader-loader.hpp>
#include <engine/resource/vertex-array-loader.hpp>

#include <engine/application/application.hpp>
#include <engine/application/input.hpp>

#include <engine/rendering/render-target.hpp>

#include <engine/math/math.hpp>
#include <engine/math/matrix.hpp>

#include "camera.hpp"
#include "camera-controller.hpp"
#include "player-input.hpp"

#include "chunk-manager.hpp"
#include "chunk.hpp"

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

    chunkManager = new ChunkManager(getEngine()->getRenderContext(), 16);

    cameraBuffer = new UniformBuffer(getEngine()->getRenderContext(),
            sizeof(Matrix4f), GL_STREAM_DRAW, 0);
}

void MyScene::update(float deltaTime) {
    static bool wireframe = false; // TODO: poopoo

    update_player_input(getEngine()->getRegistry(), getEngine()->getInput());
    update_camera_controller(getEngine()->getRegistry(),
            getEngine()->getApplication(), deltaTime);
    update_camera_system(getEngine()->getRegistry());

    auto* cam = getEngine()->getRegistry().raw<Camera>();

    if (getEngine()->getInput().was_key_pressed(Input::KEY_R)) {
        cam->syncFrustum = !cam->syncFrustum;
    }

    if (getEngine()->getInput().was_key_pressed(Input::KEY_V)) {
        wireframe = !wireframe;

        if (wireframe) {
            getEngine()->getRenderContext().setPolygonMode(
                    RenderContext::POLYGON_MODE_WIREFRAME);
        }
        else {
            getEngine()->getRenderContext().setPolygonMode(
                    RenderContext::POLYGON_MODE_FILL);
        }
    }

    chunkManager->update(*cam);

    update_block_placement();
}

void MyScene::render() {
    screen->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto shader = ResourceCache<Shader>::getInstance().handle("basic-shader"_hs);
    auto cube = ResourceCache<VertexArray>::getInstance().handle("cube"_hs);

    auto* cam = getEngine()->getRegistry().raw<Camera>();

    cameraBuffer->update(&cam->viewProjection);

    chunkManager->render_chunks(*screen, shader, *cam);
}

void MyScene::unload() {
    delete cameraBuffer;
    delete chunkManager;
    delete screen;
}

void MyScene::update_block_placement() {
    static BlockType buildType = BlockType::STONE; // TODO: poopoo
    constexpr const int32 radius = 27;

    auto* cam = getEngine()->getRegistry().raw<Camera>();

    if (getEngine()->getInput().was_mouse_pressed(Input::MOUSE_BUTTON_LEFT)) {
        Vector3i blockPos, sideDir;

        const Vector3f origin(cam->invView[3]);

        if (chunkManager->find_block_on_ray(origin,
                cam->rayDirection, blockPos, sideDir)) {
            if (getEngine()->getInput().is_key_down(Input::KEY_LEFT_SHIFT)) {
                for (int32 z = -radius; z <= radius; ++z) {
                    for (int32 y = -radius; y <= radius; ++y) {
                        for (int32 x = -radius; x <= radius; ++x) {
                            const Vector3i pos = blockPos + Vector3i(x, y, z);

                            if (Math::length(Vector3f(x, y, z)) <= radius) {
                                chunkManager->remove_block(pos);
                            }
                        }
                    }
                }
            }
            else {
                chunkManager->add_block(blockPos + sideDir, buildType);
            }
        }
    }
    else if (getEngine()->getInput().was_mouse_pressed(Input::MOUSE_BUTTON_MIDDLE)) {
        Vector3i blockPos, sideDir;

        if (chunkManager->find_block_on_ray(Vector3f(cam->invView[3]),
                cam->rayDirection, blockPos, sideDir)) {
            buildType = chunkManager->get_block(blockPos).get_type();
        }
    }
}
