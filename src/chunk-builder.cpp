#include "chunk-builder.hpp"

#include <engine/math/matrix.hpp>

#include <engine/rendering/vertex-array.hpp>

#include "chunk.hpp"

namespace {
    constexpr Vector3f get_normal(const Side side) {
        switch (side) {
            case Side::SIDE_BACK:
                return Vector3f(0, 0, 1);
            case Side::SIDE_FRONT:
                return Vector3f(0, 0, -1);
            case Side::SIDE_LEFT:
                return Vector3f(-1, 0, 0);
            case Side::SIDE_RIGHT:
                return Vector3f(1, 0, 0);
            case Side::SIDE_BOTTOM:
                return Vector3f(0, -1, 0);
            case Side::SIDE_TOP:
                return Vector3f(0, 1, 0);
        }

        return Vector3f();
    }
};

void ChunkBuilder::add_quad(const Vector3f& v0, const Vector3f& v1,
        const Vector3f& v2, const Vector3f& v3,
        const Block& block, Side side, bool backFace) {
    constexpr const Vector3f STATIC_OFFSET = Vector3f(Chunk::BLOCK_RENDER_SIZE);

    const uint32 baseIndex = positions.size();

    positions.push_back(v0 - STATIC_OFFSET);
    positions.push_back(v3 - STATIC_OFFSET);
    positions.push_back(v1 - STATIC_OFFSET);
    positions.push_back(v2  - STATIC_OFFSET);

    for (uint32 i = 0; i < 4; ++i) {
        normals.push_back(get_normal(side)); // TODO: get normal from block face
        colors.push_back(Block::get_color(block.get_type()));
    }

    if (backFace) {
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);

        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 3);
        indices.push_back(baseIndex + 2);
    }
    else {
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
        indices.push_back(baseIndex + 1);

        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
    }   
}

void ChunkBuilder::fill_buffers() {
    std::unique_lock<std::mutex> lock(chunk->getMutex());

    const Vector3f pos = static_cast<Vector3f>(chunk->getPosition())
            * static_cast<float>(Chunk::CHUNK_SIZE);

    auto& vao = chunk->getVertexArray();

    vao.updateBuffer(0, positions.data(), positions.size() * sizeof(Vector3f));
    vao.updateBuffer(1, normals.data(), normals.size() * sizeof(Vector3f));
    vao.updateBuffer(2, colors.data(), colors.size() * sizeof(Vector3f));
    vao.updateBuffer(3, &pos, sizeof(Vector3f));
    vao.updateIndices(indices.data(), indices.size());

    chunk->setRebuilt();
}

void ChunkBuilder::set_chunk(Chunk* chunk) {
    this->chunk = chunk;
}

bool ChunkBuilder::is_empty() const {
    return positions.empty();
}

size_t ChunkBuilder::num_vertices() const {
    return positions.size();
}
