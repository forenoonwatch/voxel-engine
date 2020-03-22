#pragma once

#include <engine/math/plane.hpp>
#include <engine/math/matrix.hpp>

class Frustum {
    public:
        void update(const Matrix4f& viewProjection);

        bool intersectsCube(const Vector3f& center, float sideLength) const;
    private:
        Plane planes[6];
};
