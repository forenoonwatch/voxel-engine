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
        , flags(0) {}

void Chunk::init(RenderContext& context, const IndexedModel& model) {
    vertexArray = new VertexArray(context, model, GL_STREAM_DRAW);
}

void Chunk::load(TerrainGenerator& generator) {
    std::unique_lock<std::mutex> lock(mutex);

    flags = FLAG_NEEDS_REBUILD;

    for (int32 x = 0; x < CHUNK_SIZE; ++x) {
        for (int32 z = 0; z < CHUNK_SIZE; ++z) {
            const int32 yMax = generator.getHeight(
                    (position.x * CHUNK_SIZE + x),
                    (position.z * CHUNK_SIZE + z));

            for (int32 y = 0; y < CHUNK_SIZE; ++y) {
                const int32 yGlobal = position.y * CHUNK_SIZE + y;

                if (yGlobal < yMax) {
                    blocks[x][y][z].set_active(true);
                    blocks[x][y][z].set_type(BlockType::DIRT);
                }
                else if (yGlobal == yMax) {
                    blocks[x][y][z].set_active(true);
                    blocks[x][y][z].set_type(BlockType::GRASS);
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

    uint32 occFlags = FLAG_ALL_OCCLUSIONS;

    //flags |= FLAG_EMPTY;
    //flags &= ~FLAG_NEEDS_REBUILD;

    Side side = Side::SIDE_BACK;
    int n;

    BlockFace mask[CHUNK_SIZE * CHUNK_SIZE];

    for (bool backFace = true, b = false; b != backFace;
            backFace = backFace && b, b = !b) {
        for (int d  = 0; d < 3; ++d) {
            int u = (d + 1) % 3;
            int v = (d + 2) % 3;

            Vector3i x(0, 0, 0);
            Vector3i q(0, 0, 0);

            q[d] = 1;

            switch (d) {
                case 0:
                    side = backFace ? Side::SIDE_LEFT : Side::SIDE_RIGHT; // TODO: ==?
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
                        if (backFace) {
                            if (x[d] < CHUNK_SIZE - 1) {
                                auto& face = mask[n++];
                                get_block_face(face, x + q, side);
                            }
                            else {
                                ++n;
                            }
                        }
                        else {
                            if (x[d] >= 0) {
                                auto& face = mask[n++];
                                get_block_face(face, x, side);
                            }
                            else {
                                ++n;
                            }
                        }
                    }
                }

                ++x[d];

                n = 0;

                for (int j = 0; j < CHUNK_SIZE; ++j) {
                    for (int i = 0; i < CHUNK_SIZE;) {
                        if (mask[n]) {
                            int w, h; // TODO: shid loop computes width iteratively?
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

                            if (!mask[n].transparent) {
                                x[u] = i;
                                x[v] = j;

                                Vector3i du(0, 0, 0);
                                Vector3i dv(0, 0, 0);

                                du[u] = w;
                                dv[v] = h;

                                cb->add_quad(x, x + du, x + du + dv, x + dv,
                                        w, h, mask[n], backFace);
                            }

                            for (int l = 0; l < h; ++l) {
                                for (int k = 0; k < w; ++k) {
                                    mask[n + k + l * CHUNK_SIZE].active = false;
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
            }
        }
    }

    if (cb->is_empty()) {
        flags |= FLAG_EMPTY;
    }

    //flags |= occFlags;
    cb->set_chunk(this);
}

void Chunk::setPosition(const Vector3i& position) noexcept {
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

Chunk::~Chunk() {
    if (vertexArray) {
        delete vertexArray;
    }
}

void Chunk::get_block_face(BlockFace& bf, const Vector3i& pos, Side side) {
    bf.type = blocks[pos.x][pos.y][pos.z].get_type();
    bf.side = side;
    bf.active = blocks[pos.x][pos.y][pos.z].is_active();
}
