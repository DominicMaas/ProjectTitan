#pragma once

#include <string>
#include <vector>
#include "Chunk.h"
#include "Camera.h"
#include <thread>
#include <glm/glm.hpp>
#include "FastNoise.h"
#include "Skybox.h"
#include <algorithm>

// Define Chunk class to prevent compile Issues (Probably a better way to do it)
class Chunk;

class World
{
private:
	std::vector<Chunk*> _chunks;
	Shader _worldShader;
	Skybox _worldSkybox;

	// Lighting
	glm::vec3 _sunDirection;
	glm::vec3 _sunColor;
	float _sunSpeed;
	float _sunAmbient;

	FastNoise _noise;
	int _seed;

	// Chunk rebuilding
	int _rebuiltChunksThisFrame;
	void rebuildChunks();

	// Chunk generation

	void genChunk(glm::vec3 position);
	void genChunks();

	int _loadedChunksThisFrame;
	void loadChunks();

public:
	World(int seed, std::string worldName);
	World(std::string worldName);
	~World();

	void update(Camera& c, glm::mat4 proj, float delta);

	void reset(bool resetSeed);

	Shader* getWorldShader();

	// Constants
	static const int LOADED_CHUNKS_PER_FRAME = 2;
	static const int REBUILD_CHUNKS_PER_FRAME = 2;

	Chunk* findChunk(glm::vec3 position);

	FastNoise getNoise() { return _noise; }

	int getSeed() { return _seed; }
};
