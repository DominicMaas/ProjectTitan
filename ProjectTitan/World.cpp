#include "World.h"

void World::rebuildChunks()
{
	for (int i = 0; i < _chunks.size(); ++i)
	{
		if (_chunks[i]->shouldRebuildChunk())
		{
			_chunks[i]->rebuild();
		}
	}
}

World::World(int seed, std::string worldName) : _worldShader(Shader("Test.vert", "Test.frag"))
{
	// World properties
	_sunDirection = glm::vec3(0.1f, -0.8f, 0.4f);
	_sunColor = glm::vec3(1, 1, 1);
	_sunSpeed = 0.005f;
	_sunAmbient = 0.4f;

	// Noise generation
	_noise.SetNoiseType(FastNoise::Simplex);
	_noise.SetSeed(seed);

	for (int x = -8; x < 8; x++) {
		for (int z = -8; z < 8; z++) {
			_chunks.push_back(new Chunk(glm::vec3(x * Chunk::CHUNK_SIZE, 0, z * Chunk::CHUNK_SIZE), this));
		}
	}
}

void World::update(Camera& c, glm::mat4 proj, float delta)
{
	// Rebuild any chunks
	rebuildChunks();

	// Run sun updates
	float sunVelocity = _sunSpeed * delta;
	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, sunVelocity, glm::vec3(0.0, 0.0, 1.0));
	_sunDirection = glm::vec3(rotationMat * glm::vec4(_sunDirection, 1.0));

	// Use world shader
	_worldShader.use();

	// Set light color and direction
	_worldShader.setVec3("light.color", _sunColor);
	_worldShader.setVec3("light.direction", _sunDirection);
	_worldShader.setFloat("light.ambient", _sunAmbient);

	// Set the camera view and view position matrix
	_worldShader.setMat4("view", c.getViewMatrix());
	_worldShader.setVec3("viewPos", c.getPosition());

	// TODO, move out
	_worldShader.setMat4("projection", proj);

	for (int i = 0; i < _chunks.size(); ++i)
	{
		_chunks[i]->render();
	}
}

Shader* World::getWorldShader()
{
	return &_worldShader;
}

unsigned int World::getBlockTypeAtPosition(int x, int y, int z)
{
	float noise = _noise.GetNoise(x, y, z);
	if (noise <= -0.4)
	{
		if (y > 26)
		{
			return BlockManager::BLOCK_GRASS;
		}
		else
		{
			return BlockManager::BLOCK_DIRT;
		}
	}
	else
	{
		return BlockManager::BLOCK_AIR;
	}
}