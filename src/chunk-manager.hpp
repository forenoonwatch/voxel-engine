#pragma once

#include <engine/core/common.hpp>
#include <engine/core/array-list.hpp>
#include <engine/core/queue.hpp>

#include <engine/math/vector.hpp>

#include <thread>
#include <mutex>
#include <atomic>

#include "terrain-generator.hpp"

class Chunk;
class RenderContext;
class RenderTarget;
class Shader;
class Camera;
class VertexArray;

class ChunkManager {
    public:
        ChunkManager(RenderContext& context, int32 loadDistance);

        void update(const Camera& camera);
        void render_chunks(RenderTarget& target, Shader& shader,
                const Camera& camera);

        ~ChunkManager();
    private:
        NULL_COPY_AND_ASSIGN(ChunkManager);

        Chunk* chunkPool;
        Chunk** loadedChunks;
        int32 loadDistance;

        Queue<Chunk*> chunksToLoad;
        std::mutex loadMutex;

        Queue<Chunk*> chunksToRebuild;
        std::mutex rebuildMutex;

        Chunk** renderList;
        int32 numToRender;

        Vector3i chunkOffset;

        RenderContext* context;

        TerrainGenerator terrainGenerator;

        std::atomic<bool> running;

        ArrayList<std::thread> loadThreads;

        void load_chunks();

        void update_load_list(const Camera& camera);
        void update_render_list(const Camera& camera);

        int32 get_local_index(const Vector3i& localPos) const;

        Chunk* get_chunk_by_position(const Vector3i& worldPos);
        Chunk* get_chunk_by_position(const Vector3i& worldPos) const;

        bool chunk_is_occluded(const Vector3i& chunkPos) const;
        bool is_valid_local_index(const Vector3i& index) const;
};
