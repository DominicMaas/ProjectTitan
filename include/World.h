#pragma once

#include <string>
#include <vector>
#include "Chunk.h"
#include "Camera.h"
#include <thread>
#include <glm/glm.hpp>
#include "FastNoise.h"
#include "Skybox.h"

// Define Chunk class to prevent compile Issues (Probably a better way to do it)
class Chunk;

class World
{
private:
	std::vector<Chunk*> _chunks;
	Shader _worldShader;
	Skybox _worldSkybox;

	int _seed;

	// Lighting
	glm::vec3 _sunDirection;
	glm::vec3 _sunColor;
	float _sunSpeed;
	float _sunAmbient;

	FastNoise _noise;

	// Chunk rebuilding
	int _rebuiltChunksThisFrame;
	void rebuildChunks();

	// Chunk generation
	int _generatedChunksThisFrame;
	void genChunk(glm::vec3 position);
	void genChunks();

public:
	World(int seed, std::string worldName);
	World(std::string worldName);
	~World();

	void update(Camera& c, glm::mat4 proj, float delta);

	void reset(bool resetSeed);

	Shader* getWorldShader();

	unsigned int getBlockTypeAtPosition(glm::vec3 position);

	// Constants
	static const int GEN_CHUNKS_PER_FRAME = 4;
	static const int REBUILD_CHUNKS_PER_FRAME = 4;

	Chunk* findChunk(glm::vec3 position);
};
