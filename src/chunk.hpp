#pragma once

#include "block.hpp"

#include <engine/core/common.hpp>

#include <libnoise/noise.h>

class RenderContext;
class VertexArray;
class IndexedModel;

class Chunk final {
    public:
        static constexpr const uint32 CHUNK_SIZE = 16;
        static constexpr const float BLOCK_RENDER_SIZE = 0.5f;

        Chunk();

        void init(RenderContext& context, const IndexedModel& model);

        void load(const Vector3i& position,
                noise::module::Perlin& perlin);
        void rebuild();

        Block& get(uint32 x, uint32 y, uint32 z) noexcept;

        VertexArray& getVertexArray() noexcept;

        const Vector3i& getPosition() const noexcept;
        
        bool occludesNegX() const noexcept;
        bool occludesPosX() const noexcept;
        bool occludesNegY() const noexcept;
        bool occludesPosY() const noexcept;
        bool occludesNegZ() const noexcept;
        bool occludesPosZ() const noexcept;

        bool isEmpty() const noexcept;

        ~Chunk();
    private:
        NULL_COPY_AND_ASSIGN(Chunk);

        enum ChunkFlags {
            FLAG_OCCLUDES_POS_X = 1,
            FLAG_OCCLUDES_NEG_X = 2,
            FLAG_OCCLUDES_POS_Y = 4,
            FLAG_OCCLUDES_NEG_Y = 8,
            FLAG_OCCLUDES_POS_Z = 16,
            FLAG_OCCLUDES_NEG_Z = 32,

            FLAG_ALL_OCCLUSIONS = 63,

            FLAG_EMPTY          = 64,
        };

        Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
        VertexArray* vertexArray;
        Vector3i position;
        uint32 flags;
};
