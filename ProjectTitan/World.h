#pragma once

#include <string>
#include <vector>
#include "Chunk.h"
#include "Camera.h"
#include <glm/glm.hpp>

// Define Chunk class to prevent compile Issues (Probably a better way to do it)
class Chunk;

class World
{
private:
	std::vector<Chunk*> _chunks;
	Shader _worldShader;

	// Lighting
	glm::vec3 _sunDirection;
	glm::vec3 _sunColor;
	float _sunSpeed;
	float _sunAmbient;

public:
	World(std::string seed, std::string worldName);

	void update(Camera& c, glm::mat4 proj);

	Shader* getWorldShader();
};
