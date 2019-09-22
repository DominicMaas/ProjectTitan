#include "BlockManager.h"

glm::vec3 BlockManager::getColorFromId(unsigned int id)
{
	switch (id)
	{
	case Block::BLOCK_GRASS:
		return glm::vec3(0.0f, 0.5f, 0.31f);
	case Block::BLOCK_DIRT:
		return glm::vec3(0.4f, 0.4f, 0.4f);
	}

	return glm::vec3(0, 0, 0);
}