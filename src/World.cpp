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

void World::genChunk(glm::vec3 position)
{
	_chunks.push_back(new Chunk(glm::vec3(position.x * Chunk::CHUNK_WIDTH, 0, position.z * Chunk::CHUNK_WIDTH), this));
}

void World::genChunks()
{
	int size = 0;

	for (int x = -size; x < size; x++)
	{
		for (int z = -size; z < size; z++)
		{
			genChunk(glm::vec3(x, 0, z));
		}
	}

	genChunk(glm::vec3(0, 0, 0));
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

	this->_worldSkybox.render(c.getViewMatrix(), proj);
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
	for (Chunk* c : _chunks)
	{
		glm::vec3 chunkPos = c->getPosition();

		if ((position.x < chunkPos.x) || (position.z < chunkPos.z) || (position.x >= chunkPos.x + Chunk::CHUNK_WIDTH) || (position.z >= chunkPos.z + Chunk::CHUNK_WIDTH))
		{
			return c;
		}
	}

	return NULL;
}