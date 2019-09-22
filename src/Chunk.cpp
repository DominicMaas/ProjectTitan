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
				unsigned int type = _world->getBlockTypeAtPosition(_position.x + x, _position.y + y, _position.z + z);
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
	glDrawArrays(GL_TRIANGLES, 0, chunkFaces.size() / 9);
	glBindVertexArray(0);
}

void Chunk::genFace(int face, float x, float y, float z, glm::vec3 color)
{
	for (int l = 0; l < 36; l++)
	{
		if ((l) % 6 == 0)
			chunkFaces.push_back(VertexMap[face][l] + x);
		else if ((l - 1) % 6 == 0)
			chunkFaces.push_back(VertexMap[face][l] + y);
		else if ((l - 2) % 6 == 0)
			chunkFaces.push_back(VertexMap[face][l] + z);
		else if ((l - 3) % 6 == 0)
			chunkFaces.push_back(VertexMap[face][l]);
		else if ((l - 4) % 6 == 0)
			chunkFaces.push_back(VertexMap[face][l]);
		else
		{
			chunkFaces.push_back(VertexMap[face][l]);

			// Colors at the end
			chunkFaces.push_back(color.x);
			chunkFaces.push_back(color.y);
			chunkFaces.push_back(color.z);
		}
	}
}

bool Chunk::isTransparent(int x, int y, int z)
{
	if (getBlock(x, y, z).getId() == BlockManager::BLOCK_AIR)
		return true;

	return false;
}

Block Chunk::getBlock(int x, int y, int z)
{
	// TODO, check on other chunks
	if (x >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || z >= CHUNK_WIDTH)
		return Block();

	if (x <= 0 || y <= 0 || x <= 0)
		return Block();

	return _blocks[x][y][z];
}

void Chunk::rebuild()
{
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
				// Get the id at this position
				Block b = _blocks[x][y][z];

				// Don't render Air
				if (b.getId() == BlockManager::BLOCK_AIR)
					continue;

				// Get block data
				glm::vec3 color = BlockManager::getColorFromId(b.getId());

				// Render block
				if (isTransparent(x, y, z - 1))
					genFace(faceFront, x, y, z, color);

				if (isTransparent(x, y, z + 1))
					genFace(faceBack, x, y, z, color);

				if (isTransparent(x - 1, y, z))
					genFace(faceRight, x, y, z, color);

				if (isTransparent(x + 1, y, z))
					genFace(faceLeft, x, y, z, color);

				if (isTransparent(x, y - 1, z))
					genFace(faceDown, x, y, z, color);

				if (isTransparent(x, y + 1, z))
					genFace(faceUp, x, y, z, color);
			}
		}
	}
	rebuildTimer.stop();

	ExecutionTimer openGlTimer("Binding OpenGL");
	// Bind Vertex Array Object
	glBindVertexArray(_vao);

	// Copy our vertices's array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * chunkFaces.size(), chunkFaces.data(), GL_STATIC_DRAW);

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