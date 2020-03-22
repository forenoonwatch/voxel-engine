#include "frustum.hpp"

void Frustum::update(const Matrix4f& m) {
    planes[0] = Plane(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0], m[3][3] + m[3][0]);
    planes[1] = Plane(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0], m[3][3] - m[3][0]);

    planes[2] = Plane(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1], m[3][3] + m[3][1]);
    planes[3] = Plane(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1], m[3][3] - m[3][1]);

    planes[4] = Plane(m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2], m[3][3] + m[3][2]);
    planes[5] = Plane(m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2], m[3][3] - m[3][2]);
}

bool Frustum::intersectsCube(const Vector3f& center, float sideLength) const {
    for (const auto& plane : planes) {
        int32 in = 0;

        for (float z = 0; z <= sideLength; z += sideLength) {
            for (float y = 0; y <= sideLength; y += sideLength) {
                for (float x = 0; x <= sideLength; x += sideLength) {
                    const auto pos = center + Vector3f(x, y, z);

                    if (plane.getPointDistance(pos) > 0.f) {
                        ++in;
                    }
                }
            }
        }

        if (!in) {
            return false;
        }
    }

    return true;
}
