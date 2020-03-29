#include "chunk-manager.hpp"

#include <engine/core/memory.hpp>

#include <engine/math/matrix.hpp>
#include <engine/math/math.hpp>

#include <engine/rendering/vertex-array.hpp>

#include "chunk.hpp"
#include "camera.hpp"

#define CUBE(n) ((n) * (n) * (n))

#define NUM_THREADS             1
#define MAX_CHUNKS_TO_REBUILD   8

ChunkManager::ChunkManager(RenderContext& context, int32 loadDistance)
        : chunkPool((Chunk*)Memory::malloc(CUBE(loadDistance) * sizeof(Chunk)))
        , loadedChunks((Chunk**)Memory::malloc(CUBE(loadDistance) * sizeof(Chunk*)))
        , loadDistance(loadDistance)
        , chunkTree(loadDistance)
        , renderList((Chunk**)Memory::malloc(CUBE(loadDistance) * sizeof(Chunk*)))
        , chunkOffset(3, 0, 0)
        , context(&context)
        , running {true} {
    IndexedModel model;
    model.allocateElement(3);
    model.allocateElement(3);
    model.allocateElement(3);
    model.allocateElement(3);
    model.setInstancedElementStartIndex(3);

    for (int32 i = 0; i < CUBE(loadDistance); ++i) {
        new (chunkPool + i) Chunk();

        chunkPool[i].init(context, model);
        loadedChunks[i] = chunkPool + i;
    }

    for (int32 z = 0; z < loadDistance; ++z) {
        for (int32 y = 0; y < loadDistance; ++y) {
            for (int32 x = 0; x < loadDistance; ++x) {
                const Vector3i localPos(x, y, z);
                chunkTree.add(localPos,
                        loadedChunks[get_local_index(localPos)]);
            }
        }
    }

    for (int32 i = 0; i < NUM_THREADS; ++i) {
        loadThreads.emplace_back([&]() { load_chunks(); });
        rebuildThreads.emplace_back([&]() { rebuild_chunks(); });
        blockUpdateThreads.emplace_back([&]() { handle_block_updates(); });
    }
}

void ChunkManager::update(const Camera& camera) {
    update_load_list(camera);

    std::unique_lock<std::mutex> lock(bufferMutex);

    while (!chunksToBuffer.empty()) {
        auto cb = chunksToBuffer.front();
        chunksToBuffer.pop();

        cb->fill_buffers();
        cb.reset();
    }
}

void ChunkManager::update_load_list(const Camera& camera) {
    Vector3i newOffset(camera.invView[3]);
    newOffset /= Chunk::CHUNK_SIZE;
    newOffset -= loadDistance / 2;

    const Vector3i deltaOffset = newOffset - chunkOffset;

    if (deltaOffset.x == 0 && deltaOffset.y == 0 && deltaOffset.z == 0) {
        return;
    }

    Chunk* newChunks[CUBE(loadDistance)];

    std::unique_lock<std::mutex> lock(loadMutex);

    for (int32 x = 0; x < loadDistance; ++x) {
        for (int32 y = 0; y < loadDistance; ++y) {
            for (int32 z = 0; z < loadDistance; ++z) {
                const Vector3i pLocal(x, y, z);
                const Vector3i pOld = pLocal + deltaOffset;

                if (is_valid_local_index(pOld)) {
                    Chunk* chnk = loadedChunks[get_local_index(pOld)];
                    newChunks[get_local_index(pLocal)] = chnk;

                    if (chnk->getPosition() != pLocal + newOffset) {
                        chnk->moveTo(pLocal + newOffset);
                        chunksToLoad.push(chnk);
                    }
                }
                else {
                    Vector3i oldIndex = pOld % Vector3i(loadDistance);

                    if (oldIndex.x < 0) {
                        oldIndex.x += loadDistance;
                    }

                    if (oldIndex.y < 0) {
                        oldIndex.y += loadDistance;
                    }

                    if (oldIndex.z < 0) {
                        oldIndex.z += loadDistance;
                    }

                    if (Chunk* chnk = loadedChunks[get_local_index(oldIndex)];
                            chnk) {
                        newChunks[get_local_index(pLocal)] = chnk;

                        chnk->moveTo(pLocal + newOffset);
                        chunksToLoad.push(chnk);
                    }
                    else {
                        DEBUG_LOG_TEMP2("MISSING CHUNK REEEEE");
                    }
                }
            }
        }
    }

    chunkOffset = newOffset;

    Memory::memcpy(loadedChunks, newChunks, CUBE(loadDistance) * sizeof(Chunk*));

    update_chunk_tree();
}

void ChunkManager::rebuild_chunks() {
    while (running) {
        std::unique_lock<std::mutex> lock(rebuildMutex);

        Chunk* chunk = nullptr;

        if (!chunksToRebuild.empty()) {
            chunk = chunksToRebuild.front();
            chunksToRebuild.pop();
        }

        lock.unlock();

        if (chunk) {
            auto cb = Memory::make_shared<ChunkBuilder>();
            chunk->rebuild(cb);

            std::unique_lock<std::mutex> bufferLock(bufferMutex);
            chunksToBuffer.push(cb);
        }
    }
}

void ChunkManager::render_chunks(RenderTarget& target,
        Shader& shader, const Camera& camera) {
    update_render_list(camera);

    for (int32 i = 0; i < numToRender; ++i) {
        Chunk* c = renderList[i];
        std::unique_lock<std::mutex> lock(c->getMutex());

        context->draw(target, shader, c->getVertexArray(), GL_TRIANGLES);
    }

    //DEBUG_LOG_TEMP("Rendered %d/%d chunks", numToRender, CUBE(loadDistance));
}

bool ChunkManager::find_block_on_ray(const Vector3f& position,
        const Vector3f& direction, Vector3i& outBlockPosition,
        Vector3i& outSideDirection) {
    Vector3i chunkCoord;
    Vector3f chunkPos;
    Vector3i blockCoord;
    Vector3f blockPos;

    const Vector3f origin = position / static_cast<float>(Chunk::CHUNK_SIZE)
            - Vector3f(chunkOffset);

    constexpr const Vector3f DIRECTIONS[] = {Vector3f(-1, 0, 0), Vector3f(1, 0, 0),
            Vector3f(0, -1, 0), Vector3f(0, 1, 0), Vector3f(0, 0, -1), Vector3f(0, 0, 1)};

    if (chunkTree.intersectsRay(origin, direction, chunkCoord, chunkPos,
            blockCoord, blockPos)) {
        float maxDot = -FLT_MAX;
        int32 maxIndex;

        for (int32 i = 0; i < 6; ++i) {
            const float d = Math::dot(DIRECTIONS[i], blockPos - Vector3f(blockCoord));

            if (d > maxDot) {
                maxDot = d;
                maxIndex = i;
            }
        }

        outBlockPosition = (chunkCoord + chunkOffset) * Chunk::CHUNK_SIZE
                + blockCoord;
        outSideDirection = DIRECTIONS[maxIndex];

        return true;
    }

    return false;
}

void ChunkManager::add_block(const Vector3i& position,
        const BlockType blockType) {
    Vector3i blockPos = position % Chunk::CHUNK_SIZE;

    if (blockPos.x < 0) {
        blockPos.x += Chunk::CHUNK_SIZE;
    }

    if (blockPos.y < 0) {
        blockPos.y += Chunk::CHUNK_SIZE;
    }

    if (blockPos.z < 0) {
        blockPos.z += Chunk::CHUNK_SIZE;
    }

    const Vector3i chunkPos = (position - blockPos) / Chunk::CHUNK_SIZE
            - chunkOffset;

    auto* chunk = loadedChunks[get_local_index(chunkPos)];

    std::unique_lock<std::mutex> lock(blockUpdateMutex);
    blockUpdates[chunk].push_back({blockPos, true, blockType});
}

void ChunkManager::remove_block(const Vector3i& position) {
    Vector3i blockPos = position % Chunk::CHUNK_SIZE;

    if (blockPos.x < 0) {
        blockPos.x += Chunk::CHUNK_SIZE;
    }

    if (blockPos.y < 0) {
        blockPos.y += Chunk::CHUNK_SIZE;
    }

    if (blockPos.z < 0) {
        blockPos.z += Chunk::CHUNK_SIZE;
    }

    const Vector3i chunkPos = (position - blockPos) / Chunk::CHUNK_SIZE
            - chunkOffset;

    auto* chunk = loadedChunks[get_local_index(chunkPos)];

    std::unique_lock<std::mutex> lock(blockUpdateMutex);
    blockUpdates[chunk].push_back({blockPos, false, BlockType::AIR});
}

const Block& ChunkManager::get_block(const Vector3i& position) const {
    Vector3i blockPos = position % Chunk::CHUNK_SIZE;

    if (blockPos.x < 0) {
        blockPos.x += Chunk::CHUNK_SIZE;
    }

    if (blockPos.y < 0) {
        blockPos.y += Chunk::CHUNK_SIZE;
    }

    if (blockPos.z < 0) {
        blockPos.z += Chunk::CHUNK_SIZE;
    }

    const Vector3i chunkPos = (position - blockPos) / Chunk::CHUNK_SIZE
            - chunkOffset;

    const auto* chunk = loadedChunks[get_local_index(chunkPos)];

    return chunk->get(blockPos);
}

ChunkManager::~ChunkManager() {
    running = false;

    for (auto& thread : loadThreads) {
        thread.join();
    }

    for (auto& thread : rebuildThreads) {
        thread.join();
    }

    for (auto& thread : blockUpdateThreads) {
        thread.join();
    }

    for (int32 i = 0; i < CUBE(loadDistance); ++i) {
        std::unique_lock<std::mutex> lock(chunkPool[i].getMutex());
        chunkPool[i].~Chunk();
    }

    Memory::free(renderList);

    Memory::free(loadedChunks);
    Memory::free(chunkPool);
}

void ChunkManager::load_chunks() {
    while (running) {
        std::unique_lock<std::mutex> lock(loadMutex);

        Chunk* chunk = nullptr;

        if (!chunksToLoad.empty()) {
            chunk = chunksToLoad.front();
            chunksToLoad.pop();
        }

        lock.unlock();

        if (chunk) {
            std::unique_lock<std::mutex> rebuildLock(rebuildMutex);

            if (chunksToRebuild.size() < MAX_CHUNKS_TO_REBUILD) {
                rebuildLock.unlock();

                chunk->load(terrainGenerator);

                rebuildLock.lock();
                chunksToRebuild.push(chunk);
            }
            else {
                lock.lock();
                chunksToLoad.push(chunk);
            }
        }
    }
}

void ChunkManager::handle_block_updates() {
    while (running) {
        std::unique_lock<std::mutex> lock(blockUpdateMutex);
        Chunk* chunk = nullptr;
        ArrayList<BlockUpdate> chunkUpdateList;

        for (auto it = std::begin(blockUpdates), end = std::end(blockUpdates);
                it != end; ++it) {
            if (!it->second.empty()) {
                chunk = it->first;
                chunkUpdateList.assign(std::begin(it->second),
                        std::end(it->second));
                it->second.clear();

                break;
            }
        }

        lock.unlock();

        if (chunk) {
            std::unique_lock<std::mutex> chunkLock(chunk->getMutex());

            for (auto it = std::begin(chunkUpdateList), end = std::end(chunkUpdateList);
                    it != end; ++it) {
                auto& block = chunk->get(it->position);
                block.set_active(it->active);
                block.set_type(it->type);

                if (it->active) {
                    chunk->getBlockTree().add(it->position);
                }
                else {
                    chunk->getBlockTree().remove(it->position);
                }
            }

            chunkLock.unlock();

            std::unique_lock<std::mutex> rebuildLock(rebuildMutex);
            chunksToRebuild.push(chunk);
        }
    }
}

void ChunkManager::update_render_list(const Camera& camera) {
    numToRender = 0;

    for (int32 z = 0; z < loadDistance; ++z) {
        for (int32 y = 0; y < loadDistance; ++y) {
            for (int32 x = 0; x < loadDistance; ++x) {
                const Vector3i pos(x, y, z);

                Chunk* c = loadedChunks[get_local_index(pos)];

                if (!c->shouldRender()) {
                    continue;
                }

                if (x > 0 && x < loadDistance - 1 && y > 0 && y < loadDistance - 1
                        && z > 0 && z < loadDistance - 1) {
                    // TODO: prebake occlusions during rebuild phase
                    if (chunk_is_occluded(pos + chunkOffset)) {
                        continue;
                    }
                }

                const Vector3f worldPos = static_cast<Vector3f>(c->getPosition())
                        * static_cast<float>(Chunk::CHUNK_SIZE);
                
                if (!camera.frustum.intersectsCube(worldPos, Chunk::CHUNK_SIZE)) {
                    continue;
                }


                renderList[numToRender++] = c;
            }
        }
    }
}

void ChunkManager::update_chunk_tree() {
    // TODO: in a perfect world I'd be walking the tree and picking chunks based on region
    // instead of vice versa
    for (int32 z = 0; z < loadDistance; ++z) {
        for (int32 y = 0; y < loadDistance; ++y) {
            for (int32 x = 0; x < loadDistance; ++x) {
                const Vector3i localPos(x, y, z);
                auto* chunk = loadedChunks[get_local_index(localPos)];

                chunkTree.add(localPos, chunk);
            }
        }
    }
}

int32 ChunkManager::get_local_index(const Vector3i& localPos) const {
    return (localPos.x * loadDistance + localPos.y) * loadDistance
            + localPos.z;
}

Chunk* ChunkManager::get_chunk_by_position(const Vector3i& worldPos) {
    return loadedChunks[get_local_index(worldPos - chunkOffset)];
}

Chunk* ChunkManager::get_chunk_by_position(const Vector3i& worldPos) const {
    return loadedChunks[get_local_index(worldPos - chunkOffset)];
}

bool ChunkManager::chunk_is_occluded(const Vector3i& chunkPos) const {
    Chunk* negX = get_chunk_by_position(chunkPos - Vector3i(1, 0, 0));
    Chunk* posX = get_chunk_by_position(chunkPos + Vector3i(1, 0, 0));
    Chunk* negY = get_chunk_by_position(chunkPos - Vector3i(0, 1, 0));
    Chunk* posY = get_chunk_by_position(chunkPos + Vector3i(0, 1, 0));
    Chunk* negZ = get_chunk_by_position(chunkPos - Vector3i(0, 0, 1));
    Chunk* posZ = get_chunk_by_position(chunkPos + Vector3i(0, 0, 1));

    return (negX && posX && negY && posY && negZ && posZ
            && negX->occludesPosX() && posX->occludesNegX()
            && negY->occludesPosY() && posY->occludesNegY()
            && negZ->occludesPosZ() && posZ->occludesNegZ());
}

bool ChunkManager::is_valid_local_index(const Vector3i& index) const {
    return index.x >= 0 && index.y >= 0 && index.z >= 0
            && index.x < loadDistance && index.y < loadDistance
            && index.z < loadDistance;
}
