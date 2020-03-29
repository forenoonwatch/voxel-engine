#include "chunk.hpp"

#include <engine/rendering/vertex-array.hpp>

#include <engine/math/matrix.hpp>

#include <cstdint>

#include "chunk-manager.hpp"
#include "terrain-generator.hpp"

Chunk::Chunk()
        : blocks {}
        , vertexArray(nullptr)
        , position(INT32_MAX, INT32_MAX, INT32_MAX)
        , flags(0)
        , blockTree(Chunk::CHUNK_SIZE) {}

void Chunk::init(RenderContext& context, const IndexedModel& model) {
    vertexArray = new VertexArray(context, model, GL_STREAM_DRAW);
}

void Chunk::load(TerrainGenerator& generator) {
    std::unique_lock<std::mutex> lock(mutex);

    flags = FLAG_NEEDS_REBUILD;

    blockTree.clear();

    const Vector3i chunkWorldPos = position * CHUNK_SIZE;

    for (int32 x = 0; x < CHUNK_SIZE; ++x) {
        for (int32 z = 0; z < CHUNK_SIZE; ++z) {
            const int32 yMax = generator.getHeight(
                    (chunkWorldPos.x + x), (chunkWorldPos.z + z));

            for (int32 y = 0; y < CHUNK_SIZE; ++y) {
                const int32 yGlobal = chunkWorldPos.y + y;
                const Vector3i localPos(x, y, z);

                if (yGlobal < yMax) {
                    blocks[x][y][z].set_active(true);

                    if (yGlobal < yMax - 3) {
                        blocks[x][y][z].set_type(BlockType::STONE);
                    }
                    else {
                        blocks[x][y][z].set_type(BlockType::DIRT);
                    }

                    blockTree.add(localPos);
                }
                else if (yGlobal == yMax) {
                    blocks[x][y][z].set_active(true);
                    blocks[x][y][z].set_type(BlockType::GRASS);

                    blockTree.add(localPos);
                }
                else {
                    blocks[x][y][z].set_active(false);
                }
            }
        }
    }
}

void Chunk::rebuild(Memory::SharedPointer<ChunkBuilder> cb) {
    std::unique_lock<std::mutex> lock(mutex);

    Side side = Side::SIDE_BACK;
    int n, w, h;

    flags &= ~FLAG_ALL_OCCLUSIONS;
    flags &= ~FLAG_EMPTY;

    Block mask[CHUNK_SIZE * CHUNK_SIZE];

    // TODO: do not push unrendered interior faces
    for (bool backFace = true, b = false; b != backFace;
            backFace = (backFace && b), b = !b) {
        for (int d  = 0; d < 3; ++d) {
            int u = (d + 1) % 3;
            int v = (d + 2) % 3;

            Vector3i x(0, 0, 0);
            Vector3i q(0, 0, 0);

            q[d] = 1;

            switch (d) {
                case 0:
                    side = backFace ? Side::SIDE_LEFT : Side::SIDE_RIGHT;
                    break;
                case 1:
                    side = backFace ? Side::SIDE_BOTTOM : Side::SIDE_TOP;
                    break;
                case 2:
                    side = backFace ? Side::SIDE_BACK : Side::SIDE_FRONT;
                    break;
            }

            for (x[d] = -1; x[d] < CHUNK_SIZE;) {
                n = 0;

                for (x[v] = 0; x[v] < CHUNK_SIZE; ++x[v]) {
                    for (x[u] = 0; x[u] < CHUNK_SIZE; ++x[u]) {
                        auto& block = mask[n++];
                        Block b0, b1;

                        if (x[d] >= 0) {
                            b0 = blocks[x.x][x.y][x.z];
                        }

                        if (x[d] < CHUNK_SIZE - 1) {
                            b1 = blocks[x.x + q.x][x.y + q.y][x.z + q.z];
                        }

                        block.set_active(!(b0.is_active() && b1.is_active()
                                /*&& b0.get_type() == b1.get_type()*/));

                        if (block.is_active()) {
                            if (backFace) {
                                block.set_type(b1.get_type());
                                block.set_active(b1.is_active());
                            }
                            else {
                                block.set_type(b0.get_type());
                                block.set_active(b0.is_active());
                            }
                        }
                    }
                }

                ++x[d];

                n = 0;

                for (int j = 0; j < CHUNK_SIZE; ++j) {
                    for (int i = 0; i < CHUNK_SIZE;) {
                        if (mask[n]) {
                            bool done = false;

                            for (w = 1; i + w < CHUNK_SIZE && mask[n + w]
                                    && (mask[n + w] == mask[n]); ++w) {}

                            for (h = 1; j + h < CHUNK_SIZE; ++h) {
                                for (int k = 0; k < w; ++k) {
                                    if (!mask[n + k + h * CHUNK_SIZE]
                                            || mask[n + k + h * CHUNK_SIZE] != mask[n]) {
                                        done = true;
                                        break;
                                    }
                                }

                                if (done) {
                                    break;
                                }
                            }

                            //if (!mask[n].transparent) {
                                x[u] = i;
                                x[v] = j;

                                Vector3i du(0, 0, 0);
                                Vector3i dv(0, 0, 0);

                                du[u] = w;
                                dv[v] = h;

                                cb->add_quad(x, x + du, x + du + dv, x + dv,
                                        mask[n], side, backFace);
                            //}

                            for (int l = 0; l < h; ++l) {
                                for (int k = 0; k < w; ++k) {
                                    mask[n + k + l * CHUNK_SIZE].set_active(false);
                                }
                            }

                            i += w;
                            n += w;
                        }
                        else {
                            ++i;
                            ++n;
                        }
                    }
                }

                if (x[u] == 0 && x[v] == 0 && (x[d] == 0 || x[d] == CHUNK_SIZE)
                        && w == CHUNK_SIZE && h == CHUNK_SIZE) {
                    switch (side) {
                        case Side::SIDE_FRONT:
                            flags |= FLAG_OCCLUDES_POS_Z;
                            break;
                        case Side::SIDE_BACK:
                            flags |= FLAG_OCCLUDES_NEG_Z;
                            break;
                        case Side::SIDE_LEFT:
                            flags |= FLAG_OCCLUDES_NEG_X;
                            break;
                        case Side::SIDE_RIGHT:
                            flags |= FLAG_OCCLUDES_POS_X;
                            break;
                        case Side::SIDE_TOP:
                            flags |= FLAG_OCCLUDES_POS_Y;
                            break;
                        case Side::SIDE_BOTTOM:
                            flags |= FLAG_OCCLUDES_NEG_Y;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }

    if (cb->is_empty()) {
        flags |= FLAG_EMPTY;
    }

    cb->set_chunk(this);
}

void Chunk::moveTo(const Vector3i& position) noexcept {
    std::unique_lock<std::mutex> lock(mutex);

    flags |= FLAG_NEEDS_REBUILD;
    this->position = position;
}

void Chunk::setRebuilt() noexcept {
    flags &= ~FLAG_NEEDS_REBUILD;
}

Block& Chunk::get(uint32 x, uint32 y, uint32 z) noexcept {
    return blocks[x][y][z];
}

Block& Chunk::get(const Vector3i& position) noexcept {
    return blocks[position.x][position.y][position.z];
}

const Block& Chunk::get(const Vector3i& position) const noexcept {
    return blocks[position.x][position.y][position.z];
}

VertexArray& Chunk::getVertexArray() noexcept {
    return *vertexArray;
}

const Vector3i& Chunk::getPosition() const noexcept {
    return position;
}

bool Chunk::occludesNegX() const noexcept {
    return flags & FLAG_OCCLUDES_NEG_X;
}

bool Chunk::occludesPosX() const noexcept {
    return flags & FLAG_OCCLUDES_POS_X;
}

bool Chunk::occludesNegY() const noexcept {
    return flags & FLAG_OCCLUDES_NEG_Y;
}

bool Chunk::occludesPosY() const noexcept {
    return flags & FLAG_OCCLUDES_POS_Y;
}

bool Chunk::occludesNegZ() const noexcept {
    return flags & FLAG_OCCLUDES_NEG_Z;
}

bool Chunk::occludesPosZ() const noexcept {
    return flags & FLAG_OCCLUDES_POS_Z;
}

bool Chunk::isEmpty() const noexcept {
    return flags & FLAG_EMPTY;
}

bool Chunk::needsRebuild() const noexcept {
    return flags & FLAG_NEEDS_REBUILD;
}

bool Chunk::shouldRender() const noexcept {
    return !isEmpty() && !needsRebuild();
}

std::mutex& Chunk::getMutex() noexcept {
    return mutex;
}

BlockTreeNode& Chunk::getBlockTree() noexcept {
    return blockTree;
}

const BlockTreeNode& Chunk::getBlockTree() const noexcept {
    return blockTree;
}

Chunk::~Chunk() {
    if (vertexArray) {
        delete vertexArray;
    }
}
