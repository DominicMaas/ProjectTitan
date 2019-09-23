#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include "BlockManager.h"
#include "World.h"
#include "Block.h"
#include "core/ExecutionTimer.h"
#include "core/Logger.h"
#include "core/BlockMap.h"

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
	int _vertices;
	void genFace(int face, float x, float y, float z, glm::vec3 color, std::vector<float>* chunkFaces);

	bool isTransparent(glm::vec3 position);
	unsigned int getBlockType(glm::vec3 position);

	World* _world;

	bool _changed = true;
public:
	// Constants
	static const int CHUNK_WIDTH = 32;
	static const int CHUNK_HEIGHT = 32;

	Chunk(glm::vec3 position, World* world);
	~Chunk();

	void render();
	void rebuild();

	bool shouldRebuildChunk() { return _changed; }

	void setChanged() { _changed = true; }

	glm::vec3 getPosition() { return _position; }
};