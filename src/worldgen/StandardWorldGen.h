#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "BaseWorldGen.h"
#include "constants.h"
#include "FastNoise.h"
#include "../BlockManager.h"

class StandardWorldGen : public BaseWorldGen {
public:
	StandardWorldGen(int seed, float scale, int octaves, float persistance, float lacunarity, glm::vec3 offset);

	// Get the theoretical block type for the given x, y, z position, returns
	// and unsigned int that relates to a certain block type.
	unsigned int getTheoreticalBlockType(int x, int y, int z);

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