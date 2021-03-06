#pragma once

#include <pch.h>
#include "BaseWorldGen.h"
#include "FastNoise.h"
#include "../core/managers/BlockManager.h"

class StandardWorldGen : public BaseWorldGen {
public:
    StandardWorldGen(int seed, float scale, int octaves, float persistance, float lacunarity, glm::vec3 offset);

    // Get the theoretical block type for the given x, y, z position, returns
    // and unsigned int that relates to a certain block type.
    unsigned char getTheoreticalBlockType(int x, int y, int z) override;

    int Seed;

private:
    // List of octave offsets
    std::vector<glm::vec3> _octaveOffsets;
    float _maxPossibleHeight;
    float _scale;
    int _octaves;
    float _persistance;
    float _lacunarity;
    glm::vec3 _offset;
    FastNoise _noise;
};