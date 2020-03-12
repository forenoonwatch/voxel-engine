#pragma once

#include <engine/math/vector.hpp>

class Registry;

struct CameraController {
    Vector3f position;
    float rotationY;
    float rotationX;

    float speed;
};

void update_camera_controller(Registry& registry, float deltaTime);
