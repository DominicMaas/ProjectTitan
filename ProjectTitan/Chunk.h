#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include "Block.h"
#include "BlockManager.h"
#include "World.h"

class Chunk
{
private:
	// Data
	glm::vec3 _position;
	Block*** _blocks;

	World* _world;

	// Rendering
	unsigned int _vbo;
	unsigned int _vao;
	glm::mat4 _modelMatrix;
	std::vector<float> chunkFaces;

	void genFace(int no, float vertexMap[6][36], float x, float y, float z, glm::vec3 color);

	bool isTransparent(int x, int y, int z);
	Block getBlock(int x, int y, int z);

public:
	// Constants
	static const int CHUNK_SIZE = 16;

	Chunk(World* world, glm::vec3 position);
	~Chunk();

	void render();
	void rebuild();
};