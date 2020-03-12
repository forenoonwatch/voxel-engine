#include "block.hpp"

Vector3f Block::get_color() const noexcept {
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
