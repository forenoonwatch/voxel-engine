#pragma once

#include <engine/math/vector.hpp>

enum class BlockType {
    AIR = 0,
    GRASS,
    DIRT,
    STONE,

    NUM_TYPES
};

enum class Side {
    SIDE_BACK = 0,
    SIDE_FRONT,
    SIDE_LEFT,
    SIDE_RIGHT,
    SIDE_TOP,
    SIDE_BOTTOM,

    NUM_SIDES
};

class Block {
    public:
        static Vector3f get_color(const BlockType type) noexcept;

        inline Block() noexcept
                : active(false)
                , type(BlockType::AIR) {}
        
        inline void set_active(bool active) noexcept {
            this->active = active;
        }

        inline void set_type(BlockType type) noexcept {
            this->type = type;
        }

        inline bool is_active() const noexcept {
            return active;
        }

        inline BlockType get_type() const noexcept {
            return type;
        }

        inline bool operator==(const Block& block) const noexcept {
            return type == block.type;
        }

        inline bool operator!=(const Block& block) const noexcept {
            return type != block.type;
        }

        inline operator bool() const noexcept {
            return active;
        }
    private:
        bool active;
        BlockType type;
};
