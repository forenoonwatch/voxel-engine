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
    private:
        bool active;
        BlockType type;
};

struct BlockFace {
    BlockType type;
    Side side;
    bool active = false;
    bool transparent = false;

    constexpr bool operator==(const BlockFace& bf) const {
        return type == bf.type && side == bf.side
                && transparent == bf.transparent;
    }

    constexpr bool operator!=(const BlockFace& bf) const {
        return !(*this == bf);
    }

    constexpr operator bool() const {
        return active;
    }

    Vector3f get_normal() const;
};

