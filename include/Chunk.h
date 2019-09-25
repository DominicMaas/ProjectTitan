#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include <limits>
#include "BlockManager.h"
#include "World.h"
#include "Block.h"
#include "core/ExecutionTimer.h"
#include "core/Logger.h"
#include "core/BlockMap.h"

// Define World class to prevent compile Issues (Probably a better way to do it)
class World;

struct ChunkVertex
{
	ChunkVertex(int px, int py, int pz, int nx, int ny, int nz, glm::vec3 color)
	{
		Position = glm::vec3(px, py, pz);
		Normal = glm::vec3(nx, ny, nz);
		Color = color;
	}

	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
};

class Chunk
{
private:
	// Data
	glm::vec3 _position;
	Block*** _blocks;

	// Rendering
	unsigned int _vbo = 0;
	unsigned int _vao = 0;
	glm::mat4 _modelMatrix;
	int _vertices;

	bool isTransparent(int x, int y, int z);
	unsigned int getBlockType(int x, int y, int z);

	World* _world;

	bool _changed = true;
	bool _loaded = false;
public:
	// Constants
	static const int CHUNK_WIDTH = 16;
	static const int CHUNK_HEIGHT = 64;

	Chunk(glm::vec3 position, World* world);
	~Chunk();

	void load();
	void render();
	void rebuild();

	bool shouldRebuildChunk() { return _changed; }

	void setChanged() { _changed = true; }

	glm::vec3 getPosition() { return _position; }

	bool isLoaded() {
		return _loaded;
	}
};