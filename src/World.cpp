#include "World.h"

void World::rebuildChunks()
{
	_rebuiltChunksThisFrame = 0;

	for (int i = 0; i < _chunks.size(); ++i)
	{
		if (_chunks[i]->shouldRebuildChunk())
		{
			// Only reload a certain number of chunks per frame
			if (_rebuiltChunksThisFrame != REBUILD_CHUNKS_PER_FRAME)
			{
				_chunks[i]->rebuild();
				_rebuiltChunksThisFrame++;
			}
		}
	}
}

void World::genChunk(int x, int y, int z)
{
	_chunks.push_back(new Chunk(glm::vec3(x * Chunk::CHUNK_WIDTH, 0, z * Chunk::CHUNK_WIDTH), this));
}

void World::genChunks()
{
	for (int x = -2; x < 2; x++) {
		for (int z = -2; z < 2; z++) {
			genChunk(x, 0, z);
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
	_seed = seed;

	// If no seed, generate seed
	if (_seed == 0)
		_seed = rand() % 1000000;

	// Noise generation
	_noise.SetNoiseType(FastNoise::PerlinFractal);
	_noise.SetSeed(_seed);

	// Run on another thread
	std::thread t(&World::genChunks, this);
	t.detach();
}

World::World(std::string worldName) : World(0, worldName) { }

World::~World()
{
	std::vector<Chunk*>::iterator it;
	for (it = _chunks.begin(); it != _chunks.end(); ) {
		delete* it;
		it = _chunks.erase(it);
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

	for (int i = 0; i < _chunks.size(); ++i)
	{
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

		_chunks[i]->render();
	}
}

void World::reset(bool resetSeed)
{
	if (resetSeed)
	{
		_seed = rand() % 1000000;
		_noise.SetSeed(_seed);
	}

	// Remove all chunks
	std::vector<Chunk*>::iterator it;
	for (it = _chunks.begin(); it != _chunks.end(); ) {
		delete* it;
		it = _chunks.erase(it);
	}

	// Run on another thread
	std::thread t(&World::genChunks, this);
	t.detach();
}

Shader* World::getWorldShader()
{
	return &_worldShader;
}

unsigned int World::getBlockTypeAtPosition(int x, int y, int z)
{
	float scale = 10.0f;

	float noise = abs(_noise.GetNoise(x * scale, y * scale, z * scale));
	float mappedNoise = (noise - 0) / (1 - 0) * (0 - 100) + 100;

	if (mappedNoise <= 80)
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