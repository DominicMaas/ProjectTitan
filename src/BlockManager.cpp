#include "BlockManager.h"

glm::vec3 BlockManager::getColorFromId(unsigned int id)
{
	switch (id)
	{
	case BlockManager::BLOCK_GRASS: //7,43,8
		return glm::vec3(0.027f, 0.168f, 0.031f);
	case BlockManager::BLOCK_STONE:
		return glm::vec3(0.4f, 0.4f, 0.4f);
	case BlockManager::BLOCK_DIRT:
		return glm::vec3(0.8f, 0.3f, 0.3f);
	case BlockManager::BLOCK_WATER:
		return glm::vec3(0.1451f, 0.4275f, 0.4824f);
	}

	return glm::vec3(0, 0, 0);
}