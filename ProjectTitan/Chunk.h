#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include "BlockManager.h"
#include "World.h"
#include "Block.h"

// Define World class to prevent compile Issues (Probably a better way to do it)
class World;

class Chunk
{
private:
	// Data
	glm::vec3 _position;
	Block*** _blocks;

	// Rendering
	unsigned int _vbo;
	unsigned int _vao;
	glm::mat4 _modelMatrix;
	std::vector<float> chunkFaces;

	void genFace(int no, float vertexMap[6][36], float x, float y, float z, glm::vec3 color);

	bool isTransparent(int x, int y, int z);
	Block getBlock(int x, int y, int z);

	World* _world;

	bool _changed = true;

public:
	// Constants
	static const int CHUNK_WIDTH = 32;
	static const int CHUNK_HEIGHT = 64;

	Chunk(glm::vec3 position, World* world);
	~Chunk();

	void render();
	void rebuild();

	bool shouldRebuildChunk() { return _changed; }
};