#include "Chunk.h"

Chunk::Chunk(Shader shader, glm::vec3 position) : _shader(shader)
{
	_shader = shader;
	_position = position;
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
	_shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	_shader.setVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));

	// Render
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, chunkFaces.size());
	glBindVertexArray(0);
}

void Chunk::genFace(int no, float vertexMap[6][36], float x, float y, float z)
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
			chunkFaces.push_back(vertexMap[no][l]);
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

	for (float x = 0; x < 8; x++) {
		for (float y = 0; y < 8; y++) {
			for (float z = 0; z < 8; z++) {
				genFace(faceFront, vertexMap, x, y, z);
				genFace(faceBack, vertexMap, x, y, z);
				genFace(faceRight, vertexMap, x, y, z);
				genFace(faceLeft, vertexMap, x, y, z);
				genFace(faceDown, vertexMap, x, y, z);
				genFace(faceUp, vertexMap, x, y, z);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Setup world position
	_modelMatrix = glm::translate(glm::mat4(1.0f), _position);
}