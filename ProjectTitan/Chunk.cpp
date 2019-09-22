#include "Chunk.h"

Chunk::Chunk(Shader shader, glm::vec3 position) : _shader(shader)
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
	_shader.use();

	// TODO, cleanup
	_shader.setMat4("model", _modelMatrix);
	_shader.setMat4("view", c.getViewMatrix());
	_shader.setMat4("projection", proj);
	_shader.setVec3("viewPos", c.getPosition());

	// Set in block? todo
	_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	_shader.setVec3("lightPos", glm::vec3(0.2f, -1.0f, 0.3f));

	// Render
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, chunkFaces.size());
	glBindVertexArray(0);
}

void Chunk::genFace(int no, float vertexMap[6][36], float x, float y, float z, float r, float g, float b)
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
			chunkFaces.push_back(r);
			chunkFaces.push_back(g);
			chunkFaces.push_back(b);
		}
	}
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

				genFace(faceFront, vertexMap, x, y, z, 0.0f, 0.5f, 0.31f);
				genFace(faceBack, vertexMap, x, y, z, 0.0f, 0.5f, 0.31f);
				genFace(faceRight, vertexMap, x, y, z, 0.0f, 0.5f, 0.31f);
				genFace(faceLeft, vertexMap, x, y, z, 0.0f, 0.5f, 0.31f);
				genFace(faceDown, vertexMap, x, y, z, 0.0f, 0.5f, 0.31f);
				genFace(faceUp, vertexMap, x, y, z, 0.0f, 0.5f, 0.31f);
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