#include "chunk.hpp"

#include <engine/rendering/vertex-array.hpp>

static void create_face_posX(const Vector3f&, const Vector3f&, ArrayList<Vector3f>&,
        ArrayList<Vector3f>&, ArrayList<Vector3f>&, ArrayList<uint32>&);
static void create_face_negX(const Vector3f&, const Vector3f&, ArrayList<Vector3f>&,
        ArrayList<Vector3f>&, ArrayList<Vector3f>&, ArrayList<uint32>&);
static void create_face_posY(const Vector3f&, const Vector3f&, ArrayList<Vector3f>&,
        ArrayList<Vector3f>&, ArrayList<Vector3f>&, ArrayList<uint32>&);
static void create_face_negY(const Vector3f&, const Vector3f&, ArrayList<Vector3f>&,
        ArrayList<Vector3f>&, ArrayList<Vector3f>&, ArrayList<uint32>&);
static void create_face_posZ(const Vector3f&, const Vector3f&, ArrayList<Vector3f>&,
        ArrayList<Vector3f>&, ArrayList<Vector3f>&, ArrayList<uint32>&);
static void create_face_negZ(const Vector3f&, const Vector3f&, ArrayList<Vector3f>&,
        ArrayList<Vector3f>&, ArrayList<Vector3f>&, ArrayList<uint32>&);

Chunk::Chunk(RenderContext& context, const Vector3i& position)
        : blocks {}
        , position(position)
        , flags(0) {
    IndexedModel model;
    model.allocateElement(3);
    model.allocateElement(3);
    model.allocateElement(3);
    model.allocateElement(16);
    model.setInstancedElementStartIndex(3);

    vertexArray = new VertexArray(context, model, GL_STREAM_DRAW);
}

void Chunk::build_chunk() {

    ArrayList<Vector3f> positions;
    ArrayList<Vector3f> normals;
    ArrayList<Vector3f> colors;
    ArrayList<uint32> indices;

    uint32 occFlags = FLAG_ALL_OCCLUSIONS;

    for (uint32 z = 0; z < CHUNK_SIZE; ++z) {
        for (uint32 y = 0; y < CHUNK_SIZE; ++y) {
            for (uint32 x = 0; x < CHUNK_SIZE; ++x) {
                if (x == 0) {
                    if ((occFlags & FLAG_OCCLUDES_NEG_X) && !blocks[x][y][z].is_active()) {
                        occFlags &= ~FLAG_OCCLUDES_NEG_X;
                    }
                }
                else if (x == CHUNK_SIZE - 1) {
                    if ((occFlags & FLAG_OCCLUDES_POS_X) && !blocks[x][y][z].is_active()) {
                        occFlags &= ~FLAG_OCCLUDES_POS_X;
                    }
                }

                if (y == 0) {
                    if ((occFlags & FLAG_OCCLUDES_NEG_Y) && !blocks[x][y][z].is_active()) {
                        occFlags &= ~FLAG_OCCLUDES_NEG_Y;
                    }
                }
                else if (y == CHUNK_SIZE - 1) {
                    if ((occFlags & FLAG_OCCLUDES_POS_Y) && !blocks[x][y][z].is_active()) {
                        occFlags &= ~FLAG_OCCLUDES_POS_Y;
                    }
                }

                if (z == 0) {
                    if ((occFlags & FLAG_OCCLUDES_NEG_Z) && !blocks[x][y][z].is_active()) {
                        occFlags &= ~FLAG_OCCLUDES_NEG_Z;
                    }
                }
                else if (z == CHUNK_SIZE - 1) {
                    if ((occFlags & FLAG_OCCLUDES_POS_Z) && !blocks[x][y][z].is_active()) {
                        occFlags &= ~FLAG_OCCLUDES_POS_Z;
                    }
                }

                if (!blocks[x][y][z].is_active()) {
                    continue;
                }

                const Vector3f pos(x, y, z);
                const Vector3f color = blocks[x][y][z].get_color();

                if (x == 0 || !blocks[x - 1][y][z].is_active()) {
                    create_face_negX(pos, color, positions, normals, colors, indices);
                }

                if (x == CHUNK_SIZE - 1 || !blocks[x + 1][y][z].is_active()) {
                    create_face_posX(pos, color, positions, normals, colors, indices);
                }

                if (y == 0 || !blocks[x][y - 1][z].is_active()) {
                    create_face_negY(pos, color, positions, normals, colors, indices);
                }

                if (y == CHUNK_SIZE - 1 || !blocks[x][y + 1][z].is_active()) {
                    create_face_posY(pos, color, positions, normals, colors, indices);
                }

                if (z == 0 || !blocks[x][y][z - 1].is_active()) {
                    create_face_negZ(pos, color, positions, normals, colors, indices);
                }

                if (z == CHUNK_SIZE - 1 || !blocks[x][y][z + 1].is_active()) {
                    create_face_posZ(pos, color, positions, normals, colors, indices);
                }
            }
        }
    }

    flags |= occFlags;

    vertexArray->updateBuffer(0, &positions[0], positions.size() * sizeof(Vector3f));
    vertexArray->updateBuffer(1, &normals[0], normals.size() * sizeof(Vector3f));
    vertexArray->updateBuffer(2, &colors[0], colors.size() * sizeof(Vector3f));
    vertexArray->updateIndices(&indices[0], indices.size());
}

void Chunk::update(float deltaTime) {

}

void Chunk::render() {

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

Chunk::~Chunk() {
    delete vertexArray;
}

static void create_face_posX(const Vector3f& pos, const Vector3f& color,
        ArrayList<Vector3f>& positions,
        ArrayList<Vector3f>& normals, ArrayList<Vector3f>& colors,
        ArrayList<uint32>& indices) {
    const uint32 baseIndex = positions.size();

    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    
    for (uint32 i = 0; i < 4; ++i) {
        normals.emplace_back(1.f, 0.f, 0.f);
        colors.emplace_back(color);
    }

    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);

    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
}

static void create_face_negX(const Vector3f& pos, const Vector3f& color,
        ArrayList<Vector3f>& positions,
        ArrayList<Vector3f>& normals, ArrayList<Vector3f>& colors,
        ArrayList<uint32>& indices) {
    const uint32 baseIndex = positions.size();

    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    
    for (uint32 i = 0; i < 4; ++i) {
        normals.emplace_back(-1.f, 0.f, 0.f);
        colors.emplace_back(color);
    }

    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex);

    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex);
}

static void create_face_posY(const Vector3f& pos, const Vector3f& color,
        ArrayList<Vector3f>& positions,
        ArrayList<Vector3f>& normals, ArrayList<Vector3f>& colors,
        ArrayList<uint32>& indices) {
    const uint32 baseIndex = positions.size();

    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    
    for (uint32 i = 0; i < 4; ++i) {
        normals.emplace_back(0.f, 1.f, 0.f);
        colors.emplace_back(color);
    }

    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);

    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
}

static void create_face_negY(const Vector3f& pos, const Vector3f& color,
        ArrayList<Vector3f>& positions,
        ArrayList<Vector3f>& normals, ArrayList<Vector3f>& colors,
        ArrayList<uint32>& indices) {
    const uint32 baseIndex = positions.size();

    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    
    for (uint32 i = 0; i < 4; ++i) {
        normals.emplace_back(0.f, -1.f, 0.f);
        colors.emplace_back(color);
    }

    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex);

    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex);
}

static void create_face_posZ(const Vector3f& pos, const Vector3f& color,
        ArrayList<Vector3f>& positions,
        ArrayList<Vector3f>& normals, ArrayList<Vector3f>& colors,
        ArrayList<uint32>& indices) {
    const uint32 baseIndex = positions.size();

    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE));
    
    for (uint32 i = 0; i < 4; ++i) {
        normals.emplace_back(0.f, 0.f, 1.f);
        colors.emplace_back(color);
    }

    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);

    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
}

static void create_face_negZ(const Vector3f& pos, const Vector3f& color,
        ArrayList<Vector3f>& positions,
        ArrayList<Vector3f>& normals, ArrayList<Vector3f>& colors,
        ArrayList<uint32>& indices) {
    const uint32 baseIndex = positions.size();

    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(-Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    positions.push_back(pos + Vector3f(Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE, -Chunk::BLOCK_RENDER_SIZE));
    
    for (uint32 i = 0; i < 4; ++i) {
        normals.emplace_back(0.f, 0.f, -1.f);
        colors.emplace_back(color);
    }

    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex);

    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex);
}
