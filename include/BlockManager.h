#pragma once

#include <glm/glm.hpp>

class BlockManager
{
public:
	static glm::vec3 getColorFromId(unsigned int id);

	static const int BLOCK_AIR = 0;
	static const int BLOCK_GRASS = 1;
	static const int BLOCK_DIRT = 2;
};
