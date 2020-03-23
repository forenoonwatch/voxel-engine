#pragma once

#include <engine/core/common.hpp>
#include <engine/core/array-list.hpp>

#include <engine/math/vector.hpp>

class Chunk;
class Block;
enum class Side;

class ChunkBuilder {
    public:
        ChunkBuilder() = default;

        void add_quad(const Vector3f& v0, const Vector3f& v1,
                const Vector3f& v2, const Vector3f& v3,
                const Block& block, Side side, bool backFace);

        void fill_buffers();

        void set_chunk(Chunk* chunk);

        bool is_empty() const;

        size_t num_vertices() const;
    private:
        NULL_COPY_AND_ASSIGN(ChunkBuilder);

        ArrayList<Vector3f> positions;
        ArrayList<Vector3f> normals;
        ArrayList<Vector3f> colors;
        ArrayList<uint32> indices;

        Chunk* chunk;
};
