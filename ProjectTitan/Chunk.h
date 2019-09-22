#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include "Block.h"

class Chunk
{
private:
	// Data
	glm::vec3 _position;
	Block*** _blocks;

	// Constants
	static const int CHUNK_SIZE = 16;

	// Rendering
	unsigned int _vbo;
	unsigned int _vao;
	Shader _shader;
	glm::mat4 _modelMatrix;
	std::vector<float> chunkFaces;

	void genFace(int no, float vertexMap[6][36], float x, float y, float z, float r, float g, float b);

public:
	Chunk(Shader shader, glm::vec3 position);
	~Chunk();

	void render(Camera& c, glm::mat4 proj);
	void rebuild();
};
