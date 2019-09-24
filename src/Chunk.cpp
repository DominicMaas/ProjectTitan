#include "Chunk.h"

Chunk::Chunk(glm::vec3 position, World* world)
{
	_position = position;
	_world = world;
}

Chunk::~Chunk()
{
	// Delete the blocks
	for (int i = 0; i < CHUNK_WIDTH; ++i)
	{
		for (int j = 0; j < CHUNK_HEIGHT; ++j)
		{
			delete[] _blocks[i][j];
		}

		delete[] _blocks[i];
	}
	delete[] _blocks;

	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
}

void Chunk::load()
{
	// Create the blocks
	_blocks = new Block * *[CHUNK_WIDTH];
	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		_blocks[i] = new Block * [CHUNK_HEIGHT];

		for (int j = 0; j < CHUNK_HEIGHT; j++)
		{
			_blocks[i][j] = new Block[CHUNK_WIDTH];
		}
	}

	// Build terrain
	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		for (int y = 0; y < CHUNK_HEIGHT; y++)
		{
			for (int z = 0; z < CHUNK_WIDTH; z++)
			{
				glm::vec3 worldPos = _position + glm::vec3(x, y, z);

				unsigned int type = _world->getBlockTypeAtPosition(worldPos);
				_blocks[x][y][z].setId(type);
			}
		}
	}

	_loaded = true;
}

void Chunk::render()
{
	// Set the position of this chunk in the shader
	_world->getWorldShader()->setMat4("model", _modelMatrix);

	// Render
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, _vertices);
	glBindVertexArray(0);
}

bool Chunk::isTransparent(int x, int y, int z)
{
	if (y < 0) return false;

	if (getBlockType(x, y, z) == BlockManager::BLOCK_AIR)
		return true;

	return false;
}

unsigned int Chunk::getBlockType(int x, int y, int z)
{
	if ((y < 0) || (y >= CHUNK_HEIGHT))
		return BlockManager::BLOCK_AIR;

	if ((x < 0) || (z < 0) || (x >= CHUNK_WIDTH) || (z >= CHUNK_WIDTH))
	{
		// Calculate world coordinates
		glm::vec3 worldPos(x, y, z);
		worldPos += _position;

		// Find the chunk that contains these coordinates
		Chunk* c = _world->findChunk(worldPos);

		// Get the block that would be here if the chunk were loaded
		if (c == NULL || !c->isLoaded())
			return _world->getBlockTypeAtPosition(worldPos);

		// Calculate local space coordinates
		glm::vec3 cLocal = worldPos -= c->getPosition();
		return c->getBlockType(cLocal.x, cLocal.y, cLocal.z);
	}

	return _blocks[x][y][z].getId();
}

void Chunk::rebuild()
{
	std::vector<ChunkVertex> vertices;

	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int y = 0; y < CHUNK_HEIGHT; y++) {
			for (int z = 0; z < CHUNK_WIDTH; z++) {
				// Get the id at this position
				Block b = _blocks[x][y][z];

				// Don't render Air
				if (b.getId() == BlockManager::BLOCK_AIR)
					continue;

				// Get block data
				glm::vec3 color = BlockManager::getColorFromId(b.getId());

				// Front
				if (isTransparent(x, y, z - 1))
				{
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, 0, 0, -1, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 0 + z, 0, 0, -1, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 0 + z, 0, 0, -1, color));

					vertices.push_back(ChunkVertex(1 + x, 1 + y, 0 + z, 0, 0, -1, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 0 + z, 0, 0, -1, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, 0, 0, -1, color));
				}

				// Back
				if (isTransparent(x, y, z + 1))
				{
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 1 + z, 0, 0, 1, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 1 + z, 0, 0, 1, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 0, 0, 1, color));

					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 0, 0, 1, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 1 + z, 0, 0, 1, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 1 + z, 0, 0, 1, color));
				}

				// Right
				if (isTransparent(x - 1, y, z))
				{
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 1 + z, -1, 0, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 0 + z, -1, 0, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, -1, 0, 0, color));

					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, -1, 0, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 1 + z, -1, 0, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 1 + z, -1, 0, 0, color));
				}

				// Left
				if (isTransparent(x + 1, y, z))
				{
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 1, 0, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 0 + z, 1, 0, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 0 + z, 1, 0, 0, color));

					vertices.push_back(ChunkVertex(1 + x, 0 + y, 0 + z, 1, 0, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 1 + z, 1, 0, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 1, 0, 0, color));
				}

				// Down
				if (isTransparent(x, y - 1, z))
				{
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, 0, -1, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 0 + z, 0, -1, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 0 + y, 1 + z, 0, -1, 0, color));

					vertices.push_back(ChunkVertex(1 + x, 0 + y, 1 + z, 0, -1, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 1 + z, 0, -1, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 0 + y, 0 + z, 0, -1, 0, color));
				}

				if (isTransparent(x, y + 1, z))
				{
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 0 + z, 0, 1, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 0 + z, 0, 1, 0, color));
					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 0, 1, 0, color));

					vertices.push_back(ChunkVertex(1 + x, 1 + y, 1 + z, 0, 1, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 1 + z, 0, 1, 0, color));
					vertices.push_back(ChunkVertex(0 + x, 1 + y, 0 + z, 0, 1, 0, color));
				}
			}
		}
	}

	// Set the number of vertices
	_vertices = vertices.size();
	
	// Gen first time
	if (_vbo == 0) glGenBuffers(1, &_vbo);
	if (_vao == 0) glGenVertexArrays(1, &_vao);

	// Bind Vertex Array Object
	glBindVertexArray(_vao);

	// Copy our vertices's array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ChunkVertex) * _vertices, vertices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, Position));
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, Normal));
	glEnableVertexAttribArray(1);

	// color attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, Color));
	glEnableVertexAttribArray(2);

	// Setup world position
	_modelMatrix = glm::translate(glm::mat4(1.0f), _position);

	// The chunk has been rebuilt
	_changed = false;
}