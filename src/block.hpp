#pragma once

#include <engine/math/vector.hpp>

enum class BlockType {
    AIR = 0,
    GRASS,
    DIRT,
    STONE,

    NUM_TYPES
};

class Block {
    public:
        inline Block() noexcept
                : active(false)
                , type(BlockType::AIR) {}

        Vector3f get_color() const noexcept;
        
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

