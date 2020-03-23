#include "block.hpp"

Vector3f Block::get_color(const BlockType type) noexcept {
    switch (type) {
        case BlockType::DIRT:
            return Vector3f(127.f / 255.f, 91.f / 255.f, 40.f / 255.f);
        case BlockType::GRASS:
            return Vector3f(36.f / 255.f, 130.f / 255.f, 45.f / 255.f);
        case BlockType::STONE:
            return Vector3f(0.5f, 0.5f, 0.5f);
        default:
            return Vector3f(0.f, 0.f, 0.f);
    }
}

Vector3f BlockFace::get_normal() const {
    switch (side) {
        case Side::SIDE_BACK:
            return Vector3f(0, 0, 1);
        case Side::SIDE_FRONT:
            return Vector3f(0, 0, -1);
        case Side::SIDE_LEFT:
            return Vector3f(-1, 0, 0);
        case Side::SIDE_RIGHT:
            return Vector3f(1, 0, 0);
        case Side::SIDE_BOTTOM:
            return Vector3f(0, -1, 0);
        case Side::SIDE_TOP:
            return Vector3f(0, 1, 0);
    }

    return Vector3f();
}
