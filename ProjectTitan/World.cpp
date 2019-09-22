#include "World.h"

World::World(std::string seed, std::string worldName) : _worldShader(Shader("Test.vert", "Test.frag"))
{
	for (int x = -8; x < 8; x++) {
		for (int z = -8; z < 8; z++) {
			_chunks.push_back(new Chunk(&_worldShader, glm::vec3(x * Chunk::CHUNK_SIZE, 0, z * Chunk::CHUNK_SIZE)));
		}
	}

	for (int i = 0; i < _chunks.size(); ++i)
	{
		_chunks[i]->rebuild();
	}
}

void World::update(Camera& c, glm::mat4 proj)
{
	for (int i = 0; i < _chunks.size(); ++i)
	{
		_chunks[i]->render(c, proj);
	}
}