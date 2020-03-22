#pragma once

#include <engine/core/common.hpp>

#include <libnoise/noise.h>

#include <mutex>

class TerrainGenerator {
    public:
        TerrainGenerator() = default;

        int32 getHeight(int32 x, int32 z);
    private:
        NULL_COPY_AND_ASSIGN(TerrainGenerator);

        noise::module::Perlin perlin;
        std::mutex mutex;
};
