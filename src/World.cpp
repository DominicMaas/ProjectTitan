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
				// Load the chunk
				chunk->load();

				// Other chunks around this chunk must be rebuilt
				Chunk* cXPos = findChunk(chunk->getPosition() + glm::vec3(1 * Chunk::CHUNK_WIDTH, 0, 0));
				Chunk* cXNeg = findChunk(chunk->getPosition() - glm::vec3(1 * Chunk::CHUNK_WIDTH, 0, 0));
				Chunk* cZPos = findChunk(chunk->getPosition() + glm::vec3(0, 0, 1 * Chunk::CHUNK_WIDTH));
				Chunk* cZNeg = findChunk(chunk->getPosition() - glm::vec3(0, 0, 1 * Chunk::CHUNK_WIDTH));

				if (cXPos != NULL && cXPos->isLoaded()) cXPos->setChanged();
				if (cXNeg != NULL && cXNeg->isLoaded()) cXNeg->setChanged();
				if (cZPos != NULL && cZPos->isLoaded()) cZPos->setChanged();
				if (cZNeg != NULL && cZNeg->isLoaded()) cZNeg->setChanged();

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
		// Generate a random seed
		srand((unsigned)time(0));
		_seed = rand();
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

	float renderDistance = 6 * Chunk::CHUNK_WIDTH;

	// Calculation about the camera position and render distance
	int cWorldX = ((int)floor(c.getPosition().x / Chunk::CHUNK_WIDTH) * Chunk::CHUNK_WIDTH) - Chunk::CHUNK_WIDTH;
	int cWorldZ = ((int)floor(c.getPosition().z / Chunk::CHUNK_WIDTH) * Chunk::CHUNK_WIDTH) - Chunk::CHUNK_WIDTH;

	int minX = cWorldX - (renderDistance + (Chunk::CHUNK_WIDTH * 2));
	int maxX = cWorldX + (renderDistance + (Chunk::CHUNK_WIDTH * 2));
	int minZ = cWorldZ - (renderDistance + (Chunk::CHUNK_WIDTH * 2));
	int maxZ = cWorldZ + (renderDistance + (Chunk::CHUNK_WIDTH * 2));

	// Loop through all the chunks
	for (Chunk* chunk : _chunks)
	{
		// This chunk is not loaded
		if (!chunk->isLoaded())
			continue;

		// Check to see if chunk should be removed
		if (minX > chunk->getPosition().x ||
			minZ > chunk->getPosition().z ||
			maxZ < chunk->getPosition().z ||
			maxX < chunk->getPosition().x)
		{
			_chunks.erase(std::remove(_chunks.begin(), _chunks.end(), chunk), _chunks.end());
			delete chunk;
			continue;
		}
		else
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

			chunk->render();
		}
	}

	// Render the skybox
	this->_worldSkybox.render(c.getViewMatrix(), proj);

	// Generate new chunks
	for (int x = cWorldX - renderDistance; x < cWorldX + renderDistance; x += Chunk::CHUNK_WIDTH)
		for (int z = cWorldZ - renderDistance; z < cWorldZ + renderDistance; z += Chunk::CHUNK_WIDTH)
		{
			if (findChunk(glm::vec3(x, 0, z)) == NULL) {
				genChunk(glm::vec3(x, 0, z));
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