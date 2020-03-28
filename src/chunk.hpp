#pragma once

#include "block.hpp"

#include "chunk-builder.hpp"

#include "block-tree.hpp"

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>

#include <mutex>

class RenderContext;
class VertexArray;
class IndexedModel;
class TerrainGenerator;

class Chunk final {
    public:
        static constexpr const int32 CHUNK_SIZE = 16;
        static constexpr const float BLOCK_RENDER_SIZE = 0.5f;

        Chunk();

        void init(RenderContext& context, const IndexedModel& model);

        void load(TerrainGenerator& terrainGenerator);
        void rebuild(Memory::SharedPointer<ChunkBuilder> chunkBuilder);

        void moveTo(const Vector3i& position) noexcept;

        void setRebuilt() noexcept;

        Block& get(uint32 x, uint32 y, uint32 z) noexcept;
        Block& get(const Vector3i& position) noexcept;

        const Block& get(const Vector3i& position) const noexcept;

        VertexArray& getVertexArray() noexcept;

        const Vector3i& getPosition() const noexcept;
        
        bool occludesNegX() const noexcept;
        bool occludesPosX() const noexcept;
        bool occludesNegY() const noexcept;
        bool occludesPosY() const noexcept;
        bool occludesNegZ() const noexcept;
        bool occludesPosZ() const noexcept;

        bool isEmpty() const noexcept;
        bool needsRebuild() const noexcept;

        bool shouldRender() const noexcept;

        std::mutex& getMutex() noexcept;

        BlockTreeNode& getBlockTree() noexcept;
        const BlockTreeNode& getBlockTree() const noexcept;

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
            FLAG_NEEDS_REBUILD  = 128
        };

        Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
        VertexArray* vertexArray;
        Vector3i position;
        uint32 flags;

        std::mutex mutex;

        BlockTreeNode blockTree;

        friend class ChunkManager;
};
