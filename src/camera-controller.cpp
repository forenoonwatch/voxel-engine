#include "camera-controller.hpp"

#include "camera.hpp"
#include "player-input.hpp"

#include <engine/ecs/ecs.hpp>

void update_camera_controller(Registry& registry, float deltaTime) {
    registry.view<Camera, CameraController, PlayerInputComponent>().each([deltaTime](
            auto& camera, auto& controller, auto& pic) {
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;

        if (pic.forward) {
            z -= 1.f;
        }

        if (pic.back) {
            z += 1.f;
        }

        if (pic.left) {
            x -= 1.f;
        }

        if (pic.right) {
            x += 1.f;
        }

        if (pic.up) {
            y += 1.f;
        }

        if (pic.down) {
            y -= 1.f;
        }

        if (pic.rightMouse) {
            controller.rotationX += pic.mouseDeltaY * -0.01f;
            controller.rotationY += pic.mouseDeltaX * -0.01f;

            if (controller.rotationX < -1.27f) {
                controller.rotationX = -1.27f;
            }
            else if (controller.rotationX > 1.27f) {
                controller.rotationX = 1.27f;
            }
        }

        Matrix4f tf = Math::rotate(Matrix4f(1.f), controller.rotationY,
                Vector3f(0.f, 1.f, 0.f));

        controller.position += Vector3f(tf[0]) * (x * controller.speed * deltaTime)
                + Vector3f(tf[2]) * (z * controller.speed * deltaTime);
        controller.position.y += y * controller.speed * deltaTime;

        tf *= Math::rotate(Matrix4f(1.f), controller.rotationX,
                Vector3f(1.f, 0.f, 0.f));
        tf = Math::translate(Matrix4f(1.f), controller.position) * tf;

        // TODO: be wary of this
        tf[3] = Vector4f(controller.position, 1.f);

        camera.invView = tf;
    });
}
