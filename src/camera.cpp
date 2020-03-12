#include "camera.hpp"

#include <engine/ecs/ecs.hpp>

void update_camera_system(Registry& registry) {
    registry.view<Camera>().each([](auto& camera) {
        camera.view = Math::inverse(camera.invView);
        camera.viewProjection = camera.projection * camera.view;
    });
}
