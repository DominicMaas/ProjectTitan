#pragma once

#include <glm/glm.hpp>
#include "Block.h"

class BlockManager
{
public:
	static glm::vec3 getColorFromId(unsigned int id);
};
