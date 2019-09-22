#include "World.h"

World::World(std::string seed, std::string worldName) : _worldShader(Shader("Test.vert", "Test.frag"))
{
	for (int x = -8; x < 8; x++) {
		for (int z = -8; z < 8; z++) {
			_chunks.push_back(new Chunk(glm::vec3(x * Chunk::CHUNK_SIZE, 0, z * Chunk::CHUNK_SIZE), this));
		}
	}

	for (int i = 0; i < _chunks.size(); ++i)
	{
		_chunks[i]->rebuild();
	}
}

void World::update(Camera& c, glm::mat4 proj)
{
	// Use world shader
	_worldShader.use();

	// TODO, move out
	_worldShader.setVec3("lightColor", 1, 1, 1);
	_worldShader.setVec3("lightPos", glm::vec3(0.1f, -0.8f, 0.4f));
	_worldShader.setMat4("view", c.getViewMatrix());
	_worldShader.setMat4("projection", proj);
	_worldShader.setVec3("viewPos", c.getPosition());

	for (int i = 0; i < _chunks.size(); ++i)
	{
		_chunks[i]->render();
	}
}

Shader* World::getWorldShader()
{
	return &_worldShader;
}