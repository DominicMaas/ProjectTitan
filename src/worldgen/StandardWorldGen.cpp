#include "StandardWorldGen.h"

StandardWorldGen::StandardWorldGen(int seed, float scale, int octaves, float persistance, float lacunarity,
                                   glm::vec3 offset) : BaseWorldGen(seed) {
    // Setup noise generation
    this->_noise.SetNoiseType(FastNoise::Simplex);
    this->_noise.SetSeed(_seed);

    // Set variables
    this->_scale = scale;
    this->_octaves = octaves;
    this->_persistance = persistance;
    this->_lacunarity = lacunarity;
    this->_offset = offset;
    this->_maxPossibleHeight = 0;

    // Seed for octaves
    srand(_seed);

    // Build octaves
    float amplitude = 1;
    float frequency = 1;
    for (int i = 0; i < _octaves; i++) {
        float offSetX = (rand() % (100000 - -100000) + -100000) + _offset.x;
        float offSetY = (rand() % (100000 - -100000) + -100000) + _offset.y;
        float offSetZ = (rand() % (100000 - -100000) + -100000) + _offset.z;
        _octaveOffsets.push_back(glm::vec3(offSetX, offSetY, offSetZ));

        _maxPossibleHeight += amplitude;
        amplitude *= persistance;
    }
}

unsigned char StandardWorldGen::getTheoreticalBlockType(int x, int y, int z) {
    // Variables needed
    float halfWidth = CHUNK_WIDTH / 2;
    float halfHeight = CHUNK_HEIGHT / 2;

    // Build noise
    float amplitude = 1;
    float frequency = 1;
    float noiseHeight = 0;

    // Loop through octaves
    for (int i = 0; i < _octaves; i++) {
        float sampleX = (x - halfWidth + _octaveOffsets[i].x) / _scale * frequency;
        float sampleY = (y - halfHeight + _octaveOffsets[i].y) / _scale * frequency;
        float sampleZ = (z - halfWidth + _octaveOffsets[i].z) / _scale * frequency;

        float perlinValue = _noise.GetNoise(sampleX, sampleY, sampleZ) * 2 - 1;
        noiseHeight += perlinValue * amplitude;

        amplitude *= _persistance;
        frequency += _lacunarity;
    }

    float normalizedHeight = abs((noiseHeight + 1) / (2.0f * _maxPossibleHeight * 1.4f));
    int height = (int)glm::clamp(glm::clamp(normalizedHeight, 0.0f, std::numeric_limits<float>::max()) * CHUNK_HEIGHT,
                              0.0f, (float) CHUNK_HEIGHT);

    // Water base Level
    if (y <= 2) {
        return BlockManager::BLOCK_WATER;
    }

    // This must be done first!!!
    if (y > height) {
        return BlockManager::BLOCK_AIR;
    }

    if (y == height || y == height - 1) {
        return BlockManager::BLOCK_GRASS;
    } else {
        return BlockManager::BLOCK_DIRT;
    }

    //if (y == height) {
    //    return BlockManager::BLOCK_GRASS;
    //}

    //if (y < height) {
    //    return BlockManager::BLOCK_DIRT;
    //}



    //return BlockManager::BLOCK_AIR;


    // Water Level
    //if (y <= 4) {
    //    return BlockManager::BLOCK_WATER;
    //}

    // Grass on height level
    //if (y == height) {
    //    return BlockManager::BLOCK_GRASS;
    //}

    //return BlockManager::BLOCK_DIRT;

    // Build terrian
    /*if (y > height) {


        return BlockManager::BLOCK_AIR;
    } else if (y == height) {
        if (y >= 4) // Water Level
        {
            return BlockManager::BLOCK_GRASS;
        } else {
            return BlockManager::BLOCK_WATER;
        }
    } else if (y > height - 2) {
        return BlockManager::BLOCK_DIRT;
    } else if (y > height - 4) {
        return BlockManager::BLOCK_GRASS;
    } else {
        return BlockManager::BLOCK_STONE;
    }*/
}