#include "World.h"

void World::rebuildChunks()
{
	_rebuiltChunksThisFrame = 0;

	// Loop through all the chunks
	for (Chunk* chunk : _chunks)
	{
		if (chunk->isLoaded() && chunk->shouldRebuildChunk())
		{
			// Only reload a certain number of chunks per frame
			if (_rebuiltChunksThisFrame != REBUILD_CHUNKS_PER_FRAME)
			{
				chunk->rebuild();
				_rebuiltChunksThisFrame++;
			}
		}
	}
}

void World::genChunk(glm::vec3 position)
{
	_chunks.push_back(new Chunk(glm::vec3(position.x, position.y, position.z), this));
}

void World::genChunks()
{
	int size = 0;

	for (int x = -size; x < size; x++)
	{
		for (int z = -size; z < size; z++)
		{
			genChunk(glm::vec3(x * Chunk::CHUNK_WIDTH, 0, z * Chunk::CHUNK_WIDTH));
		}
	}
}

void World::loadChunks()
{
	_loadedChunksThisFrame = 0;

	// Loop through all the chunks
	for (Chunk* chunk : _chunks)
	{
		// Only run if the chunk is not loaded
		if (!chunk->isLoaded())
		{
			// Only load a certain number of chunks per frame
			if (_loadedChunksThisFrame != LOADED_CHUNKS_PER_FRAME)
			{
				chunk->load();
				_loadedChunksThisFrame++;
			}
		}
	}
}

World::World(int seed, std::string worldName)
	: _worldShader(Shader("shaders/chunk_shader.vert", "shaders/chunk_shader.frag"))
	, _worldSkybox(Shader("shaders/skybox_shader.vert", "shaders/skybox_shader.frag"))
{
	// World properties
	_sunDirection = glm::vec3(0.1f, -0.8f, 0.4f);
	_sunColor = glm::vec3(1, 1, 1);
	_sunSpeed = 0.005f;
	_sunAmbient = 0.4f;
	_seed = seed;

	// If no seed, generate seed
	if (_seed == 0)
	{
		srand((unsigned)time(0));
		_seed = rand() % 1000000;
	}

	// Noise generation
	_noise.SetNoiseType(FastNoise::Perlin);
	_noise.SetSeed(_seed);

	// Setup skybox
	std::vector<std::string> faces
	{
		"textures/skybox/right.jpg",
		"textures/skybox/left.jpg",
		"textures/skybox/top.jpg",
		"textures/skybox/bottom.jpg",
		"textures/skybox/front.jpg",
		"textures/skybox/back.jpg"
	};

	this->_worldSkybox.setup(faces);

	// Temp until better system is implemented
	genChunks();
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
	// Load any chunks
	loadChunks();

	// Rebuild any chunks
	rebuildChunks();

	// Run sun updates
	float sunVelocity = _sunSpeed * delta;
	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, sunVelocity, glm::vec3(0.0, 0.0, 1.0));
	_sunDirection = glm::vec3(rotationMat * glm::vec4(_sunDirection, 1.0));

	// Loop through all the chunks
	for (Chunk* chunk : _chunks)
	{
		// This chunk is not loaded
		if (!chunk->isLoaded())
			continue;

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

		chunk->render();
	}

	this->_worldSkybox.render(c.getViewMatrix(), proj);

	// Super basic chunk creation detection 
	int cWorldX = (int)floor(c.getPosition().x / Chunk::CHUNK_WIDTH) * Chunk::CHUNK_WIDTH;
	int cWorldZ = (int)floor(c.getPosition().z / Chunk::CHUNK_WIDTH) * Chunk::CHUNK_WIDTH;

	// How far to check
	int d = 16;
	int cD = (Chunk::CHUNK_WIDTH * d);

	int cX = cWorldX + (Chunk::CHUNK_WIDTH * d);
	int cZ = cWorldZ + (Chunk::CHUNK_WIDTH * d);

	for (int x = cWorldX - cD; x < cWorldX + cD; x += Chunk::CHUNK_WIDTH)
	{
		for (int z = cWorldZ - cD; z < cWorldZ + cD; z += Chunk::CHUNK_WIDTH)
		{
			if (findChunk(glm::vec3(x, 0, z)) == NULL) {
				genChunk(glm::vec3(x, 0, z));
			}
		}
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

unsigned int World::getBlockTypeAtPosition(glm::vec3 position)
{
	float h = abs(_noise.GetNoise(position.x, position.y + 10, position.z)) * 100;

	if (position.y > h)
	{
		return BlockManager::BLOCK_AIR;
	}
	else if (position.y > h - 2)
	{
		return BlockManager::BLOCK_GRASS;
	}
	else if (position.y > h - 5)
	{
		return BlockManager::BLOCK_DIRT;
	}
	else
	{
		return BlockManager::BLOCK_STONE;
	}
}

Chunk* World::findChunk(glm::vec3 position)
{
	// Loop through all the chunks
	for (Chunk* chunk : _chunks)
	{
		glm::vec3 chunkPos = chunk->getPosition();

		if ((position.x >= chunkPos.x) && (position.z >= chunkPos.z) && (position.x < chunkPos.x + Chunk::CHUNK_WIDTH) && (position.z < chunkPos.z + Chunk::CHUNK_WIDTH))
		{
			return chunk;
		}
	}

	return NULL;
}