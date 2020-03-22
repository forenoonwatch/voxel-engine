#pragma once

#include <engine/math/vector.hpp>

class Plane {
    public:
        constexpr Plane()
                : plane() {}
        constexpr Plane(float x, float y, float z, float w)
                : plane(x, y, z, w) {}
        constexpr Plane(const Vector4f& plane)
                : plane(plane) {}

        constexpr Plane(const Vector3f& position, const Vector3f& normal)
                : plane(normal.x, normal.y, normal.z, -Math::dot(position, normal)) {}

        inline float getPointDistance(const Vector3f& point) const;

        inline Vector3f getNormal() const;

        inline Vector4f& getVector();
        inline const Vector4f& getVector() const;
    private:
        Vector4f plane;
};

#include "plane.inl"
