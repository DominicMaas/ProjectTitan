#include "Chunk.h"

Chunk::Chunk(Shader* shader, glm::vec3 position) : _shader(shader)
{
	_shader = shader;
	_position = position;

	// Create the blocks
	_blocks = new Block * *[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		_blocks[i] = new Block * [CHUNK_SIZE];

		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			_blocks[i][j] = new Block[CHUNK_SIZE];
		}
	}

	// Build terrain
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				//if (sqrt((float)(x - CHUNK_SIZE / 2) * (x - CHUNK_SIZE / 2) + (y - CHUNK_SIZE / 2) * (y - CHUNK_SIZE / 2) + (z - CHUNK_SIZE / 2) * (z - CHUNK_SIZE / 2)) <= CHUNK_SIZE / 2)
				//{
				if (y > 14)
				{
					_blocks[x][y][z].setId(Block::BLOCK_GRASS);
				}
				else
				{
					_blocks[x][y][z].setId(Block::BLOCK_DIRT);
				}
				//}
			}
		}
	}
}

Chunk::~Chunk()
{
	// Delete the blocks
	for (int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (int j = 0; j < CHUNK_SIZE; ++j)
		{
			delete[] _blocks[i][j];
		}

		delete[] _blocks[i];
	}
	delete[] _blocks;
}

void Chunk::render(Camera& c, glm::mat4 proj)
{
	// Use shader
	_shader->use();

	// TODO, cleanup
	_shader->setMat4("model", _modelMatrix);
	_shader->setMat4("view", c.getViewMatrix());
	_shader->setMat4("projection", proj);
	_shader->setVec3("viewPos", c.getPosition());

	// Set in block? todo
	_shader->setVec3("lightColor", 1, 1, 1);
	_shader->setVec3("lightPos", glm::vec3(0.1f, -0.8f, 0.4f));

	// Render
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, chunkFaces.size() / 9);
	glBindVertexArray(0);
}

void Chunk::genFace(int no, float vertexMap[6][36], float x, float y, float z, glm::vec3 color)
{
	for (int l = 0; l < 36; l++)
	{
		if ((l) % 6 == 0)
			chunkFaces.push_back(vertexMap[no][l] + x);
		else if ((l - 1) % 6 == 0)
			chunkFaces.push_back(vertexMap[no][l] + y);
		else if ((l - 2) % 6 == 0)
			chunkFaces.push_back(vertexMap[no][l] + z);
		else if ((l - 3) % 6 == 0)
			chunkFaces.push_back(vertexMap[no][l]);
		else if ((l - 4) % 6 == 0)
			chunkFaces.push_back(vertexMap[no][l]);
		else
		{
			chunkFaces.push_back(vertexMap[no][l]);

			// Colors at the end
			chunkFaces.push_back(color.x);
			chunkFaces.push_back(color.y);
			chunkFaces.push_back(color.z);
		}
	}
}

bool Chunk::isTransparent(int x, int y, int z)
{
	Block b = getBlock(x, y, z);
	if (b.getId() == Block::BLOCK_AIR)
		return true;

	return false;
}

Block Chunk::getBlock(int x, int y, int z)
{
	// TODO, check on other chunks
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE)
		return Block();

	if (x < 0 || y < 0 || x < 0)
		return Block();

	return _blocks[x][y][z];
}

void Chunk::rebuild()
{
	int faceFront = 0;
	int faceBack = 1;
	int faceRight = 2;
	int faceLeft = 3;
	int faceDown = 4;
	int faceUp = 5;

	// Pos: X, Y, Z, Normal: X, Y, Z
	float vertexMap[6][36] = {
		{
			// Front
			0, 0, 0, 0, 0, -1,
			1, 0, 0, 0, 0, -1,
			1, 1, 0, 0, 0, -1,
			1, 1, 0, 0, 0, -1,
			0, 1, 0, 0, 0, -1,
			0, 0, 0, 0, 0, -1
		},{
			// Back
			0, 0, 1,  0.0f,  0.0f, 1.0f,
			1, 0, 1,  0.0f,  0.0f, 1.0f,
			1, 1, 1,  0.0f,  0.0f, 1.0f,
			1, 1, 1,  0.0f,  0.0f, 1.0f,
			0, 1, 1,  0.0f,  0.0f, 1.0f,
			0, 0, 1,  0.0f,  0.0f, 1.0f
		},{
			// Right
			0, 1, 1, -1.0f,  0.0f,  0.0f,
			0, 1, 0, -1.0f,  0.0f,  0.0f,
			0, 0, 0, -1.0f,  0.0f,  0.0f,
			0, 0, 0, -1.0f,  0.0f,  0.0f,
			0, 0, 1, -1.0f,  0.0f,  0.0f,
			0, 1, 1, -1.0f,  0.0f,  0.0f
		},{
			// Left
			1, 1, 1, 1.0f,  0.0f,  0.0f,
			1, 1, 0, 1.0f,  0.0f,  0.0f,
			1, 0, 0, 1.0f,  0.0f,  0.0f,
			1, 0, 0, 1.0f,  0.0f,  0.0f,
			1, 0, 1, 1.0f,  0.0f,  0.0f,
			1, 1, 1, 1.0f,  0.0f,  0.0f
		},{
			// Down
			0, 0, 0, 0.0f, -1.0f,  0.0f,
			1, 0, 0, 0.0f, -1.0f,  0.0f,
			1, 0, 1, 0.0f, -1.0f,  0.0f,
			1, 0, 1, 0.0f, -1.0f,  0.0f,
			0, 0, 1, 0.0f, -1.0f,  0.0f,
			0, 0, 0, 0.0f, -1.0f,  0.0f
		},{
			// UP
			0, 1, 0, 0.0f,  1.0f,  0.0f,
			1, 1, 0, 0.0f,  1.0f,  0.0f,
			1, 1, 1, 0.0f,  1.0f,  0.0f,
			1, 1, 1, 0.0f,  1.0f,  0.0f,
			0, 1, 1, 0.0f,  1.0f,  0.0f,
			0, 1, 0, 0.0f,  1.0f,  0.0f
		} };

	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				Block b = _blocks[x][y][z];

				// Don't render Air
				if (b.getId() == Block::BLOCK_AIR)
					continue;

				// Get block data
				glm::vec3 color = BlockManager::getColorFromId(b.getId());

				// Render block
				if (isTransparent(x, y, z - 1))
					genFace(faceFront, vertexMap, x, y, z, color);

				if (isTransparent(x, y, z + 1))
					genFace(faceBack, vertexMap, x, y, z, color);

				if (isTransparent(x - 1, y, z))
					genFace(faceRight, vertexMap, x, y, z, color);

				if (isTransparent(x + 1, y, z))
					genFace(faceLeft, vertexMap, x, y, z, color);

				if (isTransparent(x, y - 1, z))
					genFace(faceDown, vertexMap, x, y, z, color);

				if (isTransparent(x, y + 1, z))
					genFace(faceUp, vertexMap, x, y, z, color);
			}
		}
	}

	glGenBuffers(1, &_vbo);
	glGenVertexArrays(1, &_vao);

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
}