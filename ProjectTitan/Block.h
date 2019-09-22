#pragma once

#include <glm/glm.hpp>

class Block
{
private:
	unsigned int _id = 0;

public:
	Block();

	unsigned int getId();
	void setId(unsigned int id);

	static const int BLOCK_AIR = 0;
	static const int BLOCK_GRASS = 1;
	static const int BLOCK_DIRT = 2;
};
