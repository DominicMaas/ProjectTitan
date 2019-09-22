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
};
