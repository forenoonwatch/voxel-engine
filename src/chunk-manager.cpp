#include "chunk-manager.hpp"

#include <engine/core/memory.hpp>
#include <engine/math/matrix.hpp>

#include <engine/rendering/vertex-array.hpp>

#include "chunk.hpp"
#include "camera.hpp"

ChunkManager::ChunkManager(RenderContext& context, int32 loadDistance)
        : loadDistance(loadDistance)
        , context(&context)
        , chunkOffset(3, 0, 0) {
    loadedChunks.resize(loadDistance * loadDistance * loadDistance);
}

void ChunkManager::load_chunks(const Camera& camera) {
    Vector3i newOffset(camera.invView[3]);
    newOffset /= Chunk::CHUNK_SIZE;

    Vector3i deltaOffset = newOffset - chunkOffset;
    //chunkOffset = newOffset;

    Memory::SharedPointer<Chunk> newChunks[loadDistance
            * loadDistance * loadDistance];
    
    int32 numReloaded = 0;

    for (int32 x = 0; x < loadDistance; ++x) {
        for (int32 y = 0; y < loadDistance; ++y) {
            for (int32 z = 0; z < loadDistance; ++z) {
                const Vector3i pLocal(x, y, z);
                const Vector3i pNew = pLocal + deltaOffset;

                if (is_valid_local_index(pNew)
                        && loadedChunks[get_local_index(pNew)]) {
                    newChunks[get_local_index(pLocal)]
                            = loadedChunks[get_local_index(pNew)];
                }
                else {
                    newChunks[get_local_index(pLocal)]
                            = load_chunk(Vector3i(x, y, z) + chunkOffset);
                    ++numReloaded;
                }
            }
        }
    }

    chunkOffset = newOffset;

    loadedChunks.assign(newChunks,
            newChunks + loadDistance * loadDistance * loadDistance);
    
    if (numReloaded > 0) {
        DEBUG_LOG_TEMP("Reloaded %d chunks; delta = (%d, %d, %d)",
                numReloaded, deltaOffset.x, deltaOffset.y, deltaOffset.z);
    }
}

void ChunkManager::render_chunks(RenderTarget& target,
        Shader& shader, const Camera& camera) {
    update_render_list(camera);

    for (auto ptr : renderList) {
        if (auto c = ptr.lock(); c) {
            const Matrix4f mvp = camera.viewProjection
                    * Math::translate(Matrix4f(1.f),
                    static_cast<Vector3f>(c->getPosition()) * static_cast<float>(Chunk::CHUNK_SIZE));
            c->getVertexArray().updateBuffer(3, &mvp, sizeof(Matrix4f));
            context->draw(target, shader, c->getVertexArray(), GL_TRIANGLES);
        }
    }
}

ChunkManager::~ChunkManager() {
}

void ChunkManager::update_render_list(const Camera& camera) {
    renderList.clear();

    for (uint32 z = 0; z < loadDistance; ++z) {
        for (uint32 y = 0; y < loadDistance; ++y) {
            for (uint32 x = 0; x < loadDistance; ++x) {
                const Vector3i pos(x, y, z);

                auto c = loadedChunks[get_local_index(pos)];

                if (!c) {
                    continue;
                }

                if (x > 0 && x < loadDistance - 1 && y > 0 && y < loadDistance - 1
                        && z > 0 && z < loadDistance - 1) {
                    if (chunk_is_occluded(pos + chunkOffset)) {
                        //continue;
                    }
                }

                renderList.push_back(c);
            }
        }
    }
}

Memory::SharedPointer<Chunk> ChunkManager::load_chunk(const Vector3i& chunkPos) {
    auto c = Memory::make_shared<Chunk>(*context, chunkPos);

    for (uint32 y = 0; y < 16; ++y) {
        BlockType b = y == 15 ? BlockType::GRASS : BlockType::DIRT;

        for (uint32 x = 0; x < 16; ++x) {
            for (uint32 z = 0; z < 16; ++z) {
                c->get(x, y, z).set_active(true);
                c->get(x, y, z).set_type(b);
            }
        }
    }

    if (chunkPos.x == 3 && chunkPos.y == 1 && chunkPos.z == 1) {
        c->get(15, 5, 5).set_active(false);
    }

    c->build_chunk();

    //loadedChunks[get_local_index(chunkPos - chunkOffset)] = c;
    return c;
}

uint32 ChunkManager::get_local_index(const Vector3i& localPos) const {
    return localPos.x * loadDistance * loadDistance
            + localPos.y * loadDistance + localPos.z;
}

Memory::WeakPointer<Chunk> ChunkManager::get_chunk_by_position(const Vector3i& worldPos) {
    return loadedChunks[get_local_index(worldPos - chunkOffset)];
}

Memory::WeakPointer<Chunk> ChunkManager::get_chunk_by_position(const Vector3i& worldPos) const {
    return loadedChunks[get_local_index(worldPos - chunkOffset)];
}

bool ChunkManager::chunk_is_occluded(const Vector3i& chunkPos) const {
    auto negX = get_chunk_by_position(chunkPos - Vector3i(1, 0, 0)).lock();
    auto posX = get_chunk_by_position(chunkPos + Vector3i(1, 0, 0)).lock();
    auto negY = get_chunk_by_position(chunkPos - Vector3i(0, 1, 0)).lock();
    auto posY = get_chunk_by_position(chunkPos + Vector3i(0, 1, 0)).lock();
    auto negZ = get_chunk_by_position(chunkPos - Vector3i(0, 0, 1)).lock();
    auto posZ = get_chunk_by_position(chunkPos + Vector3i(0, 0, 1)).lock();

    return (negX && posX && negY && posY && negZ && posZ
            && negX->occludesPosX() && posX->occludesNegX()
            && negY->occludesPosY() && posY->occludesNegY()
            && negZ->occludesPosY() && posZ->occludesNegY());
}

bool ChunkManager::is_valid_local_index(const Vector3i& index) const {
    return index.x >= 0 && index.y >= 0 && index.z >= 0
            && index.x < loadDistance && index.y < loadDistance
            && index.z < loadDistance;
}
