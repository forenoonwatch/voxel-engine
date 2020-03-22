
inline float Plane::getPointDistance(const Vector3f& point) const {
    // ax + by + cz + d = 0
    // dist = (q - p) dot n
    // n = (a, b, c)
    // px * a + py * b + pz * c + d = 0
    // px * nx + py * ny + pz * nz + d = 0
    // dot(p, n) + d = 0
    // d = -dot(p, n)

    return Math::dot(Vector4f(point, 1.f), plane);
}

inline Vector3f Plane::getNormal() const {
    return Vector3f(plane);
}

inline Vector4f& Plane::getVector() {
    return plane;
}

inline const Vector4f& Plane::getVector() const {
    return plane;
}
