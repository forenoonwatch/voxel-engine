#pragma once

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>

#include <engine/core/array-list.hpp>
#include <engine/core/queue.hpp>
#include <engine/core/tree-map.hpp>

#include <engine/math/vector.hpp>

#include <thread>
#include <mutex>
#include <atomic>

#include "terrain-generator.hpp"

#include "block.hpp"
#include "chunk-tree.hpp"

class Chunk;
class RenderContext;
class RenderTarget;
class Shader;
class Camera;
class VertexArray;
class ChunkBuilder;

class ChunkManager {
    public:
        ChunkManager(RenderContext& context, int32 loadDistance);

        void update(const Camera& camera);
        void render_chunks(RenderTarget& target, Shader& shader,
                const Camera& camera);

        bool find_block_on_ray(const Vector3f& origin,
                const Vector3f& direction, Vector3i& blockPosition,
                Vector3i& sideDirection);

        void add_block(const Vector3i& position,
                const BlockType blockType);
        void remove_block(const Vector3i& position);

        const Block& get_block(const Vector3i& position) const;

        ~ChunkManager();
    private:
        NULL_COPY_AND_ASSIGN(ChunkManager);

        struct BlockUpdate {
            Vector3i position;
            bool active;
            BlockType type;
        };

        Chunk* chunkPool;
        Chunk** loadedChunks;
        int32 loadDistance;

        ChunkTreeNode chunkTree;

        Queue<Chunk*> chunksToLoad;
        std::mutex loadMutex;

        Queue<Chunk*> chunksToRebuild;
        std::mutex rebuildMutex;

        Queue<Memory::SharedPointer<ChunkBuilder>> chunksToBuffer;
        std::mutex bufferMutex;

        TreeMap<Chunk*, ArrayList<BlockUpdate>> blockUpdates;
        std::mutex blockUpdateMutex;

        Chunk** renderList;
        int32 numToRender;

        Vector3i chunkOffset;

        RenderContext* context;

        TerrainGenerator terrainGenerator;

        std::atomic<bool> running;

        ArrayList<std::thread> loadThreads;
        ArrayList<std::thread> rebuildThreads;
        ArrayList<std::thread> blockUpdateThreads;

        void load_chunks();
        void rebuild_chunks();
        void handle_block_updates();

        void update_load_list(const Camera& camera);
        void update_render_list(const Camera& camera);

        void update_chunk_tree();

        int32 get_local_index(const Vector3i& localPos) const;

        Chunk* get_chunk_by_position(const Vector3i& worldPos);
        Chunk* get_chunk_by_position(const Vector3i& worldPos) const;

        bool chunk_is_occluded(const Vector3i& chunkPos) const;
        bool is_valid_local_index(const Vector3i& index) const;
};
