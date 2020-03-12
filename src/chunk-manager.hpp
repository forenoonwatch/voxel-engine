#pragma once

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>
#include <engine/core/array-list.hpp>

#include <engine/math/vector.hpp>

class Chunk;
class RenderContext;
class RenderTarget;
class Shader;
class Camera;

class ChunkManager {
    public:
        ChunkManager(RenderContext& context, int32 loadDistance);

        void load_chunks(const Camera& camera);
        void render_chunks(RenderTarget& target, Shader& shader,
                const Camera& camera);

        ~ChunkManager();
    private:
        NULL_COPY_AND_ASSIGN(ChunkManager);

        ArrayList<Memory::SharedPointer<Chunk>> loadedChunks;
        int32 loadDistance;

        ArrayList<Memory::WeakPointer<Chunk>> renderList;

        Vector3i chunkOffset;

        RenderContext* context;

        void update_render_list(const Camera& camera);

        Memory::SharedPointer<Chunk> load_chunk(const Vector3i& chunkPos);

        uint32 get_local_index(const Vector3i& localPos) const;

        Memory::WeakPointer<Chunk> get_chunk_by_position(const Vector3i& worldPos);
        Memory::WeakPointer<Chunk> get_chunk_by_position(const Vector3i& worldPos) const;

        bool chunk_is_occluded(const Vector3i& chunkPos) const;
        bool is_valid_local_index(const Vector3i& index) const;
};
