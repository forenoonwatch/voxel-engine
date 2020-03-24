#pragma once

#include <engine/math/matrix.hpp>

#include "frustum.hpp"

class Registry;

struct Camera {
    Matrix4f projection;
    Matrix4f invView;

    Matrix4f view;

    Matrix4f viewProjection;

    Frustum frustum;
    bool syncFrustum = true;

    Vector3f rayDirection;
};

void update_camera_system(Registry& registry);
