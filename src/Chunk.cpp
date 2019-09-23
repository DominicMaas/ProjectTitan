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
	glDrawArrays(GL_TRIANGLES, 0, _vertices / 9);
	glBindVertexArray(0);
}

void Chunk::genFace(int face, float x, float y, float z, glm::vec3 color, std::vector<float>* chunkFaces)
{
	for (int l = 0; l < 36; l++)
	{
		if ((l) % 6 == 0)
			chunkFaces->push_back(VertexMap[face][l] + x);
		else if ((l - 1) % 6 == 0)
			chunkFaces->push_back(VertexMap[face][l] + y);
		else if ((l - 2) % 6 == 0)
			chunkFaces->push_back(VertexMap[face][l] + z);
		else if ((l - 3) % 6 == 0)
			chunkFaces->push_back(VertexMap[face][l]);
		else if ((l - 4) % 6 == 0)
			chunkFaces->push_back(VertexMap[face][l]);
		else
		{
			chunkFaces->push_back(VertexMap[face][l]);

			// Colors at the end
			chunkFaces->push_back(color.x);
			chunkFaces->push_back(color.y);
			chunkFaces->push_back(color.z);
		}
	}
}

bool Chunk::isTransparent(glm::vec3 position)
{
	//if (position.y < 0) return false;

	//if (getBlockType(position) == BlockManager::BLOCK_AIR)
	//	return true;

	return true;
}

unsigned int Chunk::getBlockType(glm::vec3 position)
{
	if ((position.y < 0) || (position.y >= CHUNK_HEIGHT))
		return BlockManager::BLOCK_AIR;

	if ((position.x < 0) || (position.z < 0) || (position.x >= CHUNK_WIDTH) || (position.z >= CHUNK_WIDTH))
	{
		// Calculate world coordinates
		glm::vec3 worldPos(position);
		worldPos += _position;

		// Find the chunk that contains these coordinates
		Chunk* c = _world->findChunk(worldPos);

		// Get the block that would be here if the chunk were loaded
		if (c == NULL)
			return _world->getBlockTypeAtPosition(worldPos);

		return c->getBlockType(worldPos - c->getPosition());
	}

	return _blocks[(int)position.x][(int)position.y][(int)position.z].getId();
}

void Chunk::rebuild()
{
	std::vector<float> chunkFaces;

	ExecutionTimer rebuildTimer("Rebuilding chunk");
	int faceFront = 0;
	int faceBack = 1;
	int faceRight = 2;
	int faceLeft = 3;
	int faceDown = 4;
	int faceUp = 5;

	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int y = 0; y < CHUNK_HEIGHT; y++) {
			for (int z = 0; z < CHUNK_WIDTH; z++) {
				// Calculate cube index
				int cubeIndex = 0;

				if (isTransparent(glm::vec3(x - 1, y - 1, z - 1))) cubeIndex |= 1;
				if (isTransparent(glm::vec3(x + 1, y - 1, z - 1))) cubeIndex |= 2;
				if (isTransparent(glm::vec3(x + 1, y - 1, z + 1))) cubeIndex |= 4;
				if (isTransparent(glm::vec3(x - 1, y - 1, z + 1))) cubeIndex |= 8;
				if (isTransparent(glm::vec3(x - 1, y + 1, z - 1))) cubeIndex |= 16;
				if (isTransparent(glm::vec3(x + 1, y + 1, z - 1))) cubeIndex |= 32;
				if (isTransparent(glm::vec3(x + 1, y + 1, z + 1))) cubeIndex |= 64;
				if (isTransparent(glm::vec3(x - 1, y + 1, z + 1))) cubeIndex |= 128;

				// Get the id at this position
				Block b = _blocks[x][y][z];

				// Don't render Air
				if (b.getId() == BlockManager::BLOCK_AIR)
					continue;

				// Get block data
				glm::vec3 color = BlockManager::getColorFromId(b.getId());

				// Render block
				if (isTransparent(glm::vec3(x, y, z - 1)))
					genFace(faceFront, x, y, z, color, &chunkFaces);

				if (isTransparent(glm::vec3(x, y, z + 1)))
					genFace(faceBack, x, y, z, color, &chunkFaces);

				if (isTransparent(glm::vec3(x - 1, y, z)))
					genFace(faceRight, x, y, z, color, &chunkFaces);

				if (isTransparent(glm::vec3(x + 1, y, z)))
					genFace(faceLeft, x, y, z, color, &chunkFaces);

				if (isTransparent(glm::vec3(x, y - 1, z)))
					genFace(faceDown, x, y, z, color, &chunkFaces);

				if (isTransparent(glm::vec3(x, y + 1, z)))
					genFace(faceUp, x, y, z, color, &chunkFaces);
			}
		}
	}

	// Set the number of vertices
	_vertices = chunkFaces.size();

	rebuildTimer.stop();

	ExecutionTimer openGlTimer("Binding OpenGL");
	// Bind Vertex Array Object
	glBindVertexArray(_vao);

	// Copy our vertices's array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _vertices, chunkFaces.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// color attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Setup world position
	_modelMatrix = glm::translate(glm::mat4(1.0f), _position);

	// The chunk has been rebuilt
	_changed = false;
	openGlTimer.stop();
}