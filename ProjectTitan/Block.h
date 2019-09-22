#pragma once

#include <glm/glm.hpp>

class Block
{
private:
	unsigned int _id;

public:
	Block();

	unsigned int getId();
	void setId(unsigned int id);
};
