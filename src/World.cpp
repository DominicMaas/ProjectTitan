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
	genChunk(0, 0, 0);
}

World::World(int seed, std::string worldName) : _worldShader(Shader("shaders/chunk_shader.vert", "shaders/chunk_shader.frag"))
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
	genChunks();
	// std::thread t(&World::genChunks, this);
	// t.detach();
}

World::World(std::string worldName) : World(0, worldName) { }

World::~World()
{
	// Remove all chunks
	for (Chunk* b : _chunks)
	{
		delete b;
	}

	_chunks.clear();
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
	// Rebuild all chunks
	for (Chunk* b : _chunks)
	{
		b->setChanged();
	}
}

Shader* World::getWorldShader()
{
	return &_worldShader;
}

unsigned int World::getBlockTypeAtPosition(int x, int y, int z)
{
	float noise = _noise.GetNoise(x, y, z);

	if (noise > 0)
	{
		return BlockManager::BLOCK_GRASS;
	}
	else
	{
		return BlockManager::BLOCK_AIR;
	}
}