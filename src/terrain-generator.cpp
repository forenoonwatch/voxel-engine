#include "terrain-generator.hpp"

int32 TerrainGenerator::getHeight(int32 x, int32 z) {
    std::unique_lock<std::mutex> lock(mutex);

    double value = perlin.GetValue(0.01 * x, 0.01 * z, 0.5);
    value *= 10;

    return static_cast<int32>(value);
}
