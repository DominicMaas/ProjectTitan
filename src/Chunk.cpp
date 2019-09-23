#include "Chunk.h"

Chunk::Chunk(glm::vec3 position, World* world)
{
	_position = position;
	_world = world;

	// Create the blocks
	ExecutionTimer allocateMemTimer("Allocating memory for chunk");
	_blocks = new Block * *[CHUNK_WIDTH];
	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		_blocks[i] = new Block * [CHUNK_HEIGHT];

		for (int j = 0; j < CHUNK_HEIGHT; j++)
		{
			_blocks[i][j] = new Block[CHUNK_WIDTH];
		}
	}
	allocateMemTimer.stop();

	// Build terrain
	ExecutionTimer genTerrianTimer("Generating terrain");
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
	genTerrianTimer.stop();

	glGenBuffers(1, &_vbo);
	glGenVertexArrays(1, &_vao);
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

void Chunk::render()
{
	// Set the position of this chunk in the shader
	_world->getWorldShader()->setMat4("model", _modelMatrix);

	// Render
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, _vertices);
	glBindVertexArray(0);
}

void Chunk::genFace(int face, float x, float y, float z, glm::vec3 color, std::vector<float>* chunkFaces)
{
	// Position
	chunkFaces->push_back(VertexMap[face][0] + x);
	chunkFaces->push_back(VertexMap[face][1] + y);
	chunkFaces->push_back(VertexMap[face][2] + z);

	// Normals
	chunkFaces->push_back(VertexMap[face][3]);
	chunkFaces->push_back(VertexMap[face][4]);
	chunkFaces->push_back(VertexMap[face][5]);

	// Colors
	chunkFaces->push_back(color.x);
	chunkFaces->push_back(color.y);
	chunkFaces->push_back(color.z);

	// Position
	chunkFaces->push_back(VertexMap[face][6] + x);
	chunkFaces->push_back(VertexMap[face][7] + y);
	chunkFaces->push_back(VertexMap[face][8] + z);

	// Normals
	chunkFaces->push_back(VertexMap[face][9]);
	chunkFaces->push_back(VertexMap[face][10]);
	chunkFaces->push_back(VertexMap[face][11]);

	// Colors
	chunkFaces->push_back(color.x);
	chunkFaces->push_back(color.y);
	chunkFaces->push_back(color.z);

	// Position
	chunkFaces->push_back(VertexMap[face][12] + x);
	chunkFaces->push_back(VertexMap[face][13] + y);
	chunkFaces->push_back(VertexMap[face][14] + z);

	// Normals
	chunkFaces->push_back(VertexMap[face][15]);
	chunkFaces->push_back(VertexMap[face][16]);
	chunkFaces->push_back(VertexMap[face][17]);

	// Colors
	chunkFaces->push_back(color.x);
	chunkFaces->push_back(color.y);
	chunkFaces->push_back(color.z);

	// Position
	chunkFaces->push_back(VertexMap[face][18] + x);
	chunkFaces->push_back(VertexMap[face][19] + y);
	chunkFaces->push_back(VertexMap[face][20] + z);

	// Normals
	chunkFaces->push_back(VertexMap[face][21]);
	chunkFaces->push_back(VertexMap[face][22]);
	chunkFaces->push_back(VertexMap[face][23]);

	// Colors
	chunkFaces->push_back(color.x);
	chunkFaces->push_back(color.y);
	chunkFaces->push_back(color.z);

	// Position
	chunkFaces->push_back(VertexMap[face][24] + x);
	chunkFaces->push_back(VertexMap[face][25] + y);
	chunkFaces->push_back(VertexMap[face][26] + z);

	// Normals
	chunkFaces->push_back(VertexMap[face][27]);
	chunkFaces->push_back(VertexMap[face][28]);
	chunkFaces->push_back(VertexMap[face][29]);

	// Colors
	chunkFaces->push_back(color.x);
	chunkFaces->push_back(color.y);
	chunkFaces->push_back(color.z);

	// Position
	chunkFaces->push_back(VertexMap[face][30] + x);
	chunkFaces->push_back(VertexMap[face][31] + y);
	chunkFaces->push_back(VertexMap[face][32] + z);

	// Normals
	chunkFaces->push_back(VertexMap[face][33]);
	chunkFaces->push_back(VertexMap[face][34]);
	chunkFaces->push_back(VertexMap[face][35]);

	// Colors
	chunkFaces->push_back(color.x);
	chunkFaces->push_back(color.y);
	chunkFaces->push_back(color.z);
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
		if (c == NULL)
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

	ExecutionTimer rebuildTimer("Rebuilding chunk");
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

	rebuildTimer.stop();

	ExecutionTimer openGlTimer("Binding OpenGL");
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
	openGlTimer.stop();
}