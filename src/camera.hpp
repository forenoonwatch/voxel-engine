#pragma once

#include <engine/math/matrix.hpp>

class Registry;

struct Camera {
    Matrix4f projection;
    Matrix4f invView;

    Matrix4f view;
    Matrix4f viewProjection;
};

void update_camera_system(Registry& registry);
