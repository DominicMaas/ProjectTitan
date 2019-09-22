#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <vector>

class Chunk
{
private:
	glm::vec3 _position;

	// Rendering
	unsigned int _vbo;
	unsigned int _vao;
	Shader _shader;
	glm::mat4 _modelMatrix;
	std::vector<float> chunkFaces;

	void genFace(int no, float vertexMap[6][36], float x, float y, float z);

public:
	Chunk(Shader shader, glm::vec3 position);

	void render(Camera& c, glm::mat4 proj);
	void rebuild();
};
