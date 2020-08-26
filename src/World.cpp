#include "World.h"
#include "core/ResourceManager.h"

void World::rebuildChunks() {
    _rebuiltChunksThisFrame = 0;

    // Loop through all the chunks
    for (Chunk *chunk : _chunks) {
        if (chunk->isLoaded() && chunk->shouldRebuildChunk()) {
            // Only reload a certain number of chunks per frame
            if (_rebuiltChunksThisFrame != REBUILD_CHUNKS_PER_FRAME) {
                chunk->rebuild();
                _rebuiltChunksThisFrame++;
            }
        }
    }
}

void World::loadChunks() {
    _loadedChunksThisFrame = 0;

    // Loop through all the chunks
    for (Chunk *chunk : _chunks) {
        // Only run if the chunk is not loaded
        if (!chunk->isLoaded()) {
            // Only load a certain number of chunks per frame
            if (_loadedChunksThisFrame != LOADED_CHUNKS_PER_FRAME) {
                // Load the chunk
                chunk->load();

                _loadedChunksThisFrame++;
            }
        }
    }
}

World::World(int seed, std::string worldName, reactphysics3d::PhysicsCommon *physics)
        : _worldSkybox("skybox") {

    reactphysics3d::PhysicsWorld::WorldSettings settings;
    settings.gravity = reactphysics3d::Vector3(0, -9.81, 0);

    // Create the physics world
    _physicsCommon = physics;
    _physicsWorld = _physicsCommon->createPhysicsWorld(settings);

    // World properties
    _sunDirection = glm::vec3(0.0f, -1.0f, 0.6f);
    _sunColor = glm::vec3(1, 1, 1);
    _sunSpeed = 0.0f;
    _sunAmbient = 0.2f;

    _rebuiltChunksThisFrame = 0;
    _loadedChunksThisFrame = 0;

    // If no seed, generate seed
    if (seed == 0) {
        // Generate a random seed
        srand((unsigned) time(0));
        _worldGen = new StandardWorldGen(rand(), 0.75f, 5, 0.5f, 2.0f, glm::vec3(0, 0, 0));
    } else {
        _worldGen = new StandardWorldGen(seed, 0.75f, 5, 0.5f, 2.0f, glm::vec3(0, 0, 0));
    }

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
}

World::World(std::string worldName, reactphysics3d::PhysicsCommon *physics) : World(0, worldName, physics) {}

World::~World() {
    // Remove all chunks
    for (Chunk *b : _chunks) {
        delete b;
    }

    _chunks.clear();

    delete _worldGen;
}

void World::update(Camera &c, glm::mat4 proj, long double delta) {
    // Update the sun position
    float sunVelocity = _sunSpeed * delta;
    glm::mat4 rotationMat(1);
    rotationMat = glm::rotate(rotationMat, sunVelocity, glm::vec3(0.0, 0.0, 1.0));
    _sunDirection = glm::vec3(rotationMat * glm::vec4(_sunDirection, 1.0));

    // Load any chunks
    loadChunks();

    // Rebuild any chunks
    rebuildChunks();

    float renderDistance = 4 * CHUNK_WIDTH;

    // Calculation about the camera position and render distance
    int cWorldX = ((int) floor(c.getPosition().x / CHUNK_WIDTH) * CHUNK_WIDTH) - CHUNK_WIDTH;
    int cWorldZ = ((int) floor(c.getPosition().z / CHUNK_WIDTH) * CHUNK_WIDTH) - CHUNK_WIDTH;

    // Generate new chunks
    for (float x = cWorldX - renderDistance; x <= cWorldX + renderDistance; x += CHUNK_WIDTH)
    for (float z = cWorldZ - renderDistance; z <= cWorldZ + renderDistance; z += CHUNK_WIDTH) {
        if (findChunk(glm::vec3(x, 0, z)) == NULL) {
            _chunks.push_back(new Chunk(glm::vec3(x, 0, z), this));
        }
    }
}


void World::updatePhysics(long double deltaAccum) {
    _physicsWorld->update(deltaAccum);
}

void World::render(Camera &c, glm::mat4 proj) {
    float renderDistance = 4 * CHUNK_WIDTH;

    // Loop through all the chunks
    for (Chunk *chunk : _chunks) {
        // This chunk is not loaded
        if (!chunk->isLoaded())
            continue;

        // The chunk is not in the players view distance
        if (abs(chunk->getCenter().x - c.getPosition().x) >= renderDistance ||
            abs(chunk->getCenter().z - c.getPosition().z) >= renderDistance)
            continue;

        Shader* shader = ResourceManager::getShader("chunk");

        // Use world shader
        shader->use();

        // Bind the texture
        ResourceManager::getTexture("test")->bind();

        // Set light color and direction
        shader->setVec3("light.color", _sunColor);
        shader->setVec3("light.direction", _sunDirection);
        shader->setFloat("light.ambient", _sunAmbient);

        // Set the camera view and view position matrix
        shader->setMat4("view", c.getViewMatrix());
        shader->setVec3("viewPos", c.getPosition());
        shader->setMat4("projection", proj);

        chunk->render();
    }

    // Render the skybox
    this->_worldSkybox.render(c.getViewMatrix(), proj);
}

void World::reset(bool resetSeed) {
    // Rebuild all chunks
    for (Chunk *b : _chunks) {
        b->setChanged();
    }
}

Chunk *World::findChunk(glm::vec3 position) {
    // Loop through all the chunks
    for (Chunk *chunk : _chunks) {
        glm::vec3 chunkPos = chunk->getPosition();

        if ((position.x >= chunkPos.x) && (position.z >= chunkPos.z) && (position.x < chunkPos.x + CHUNK_WIDTH) &&
            (position.z < chunkPos.z + CHUNK_WIDTH)) {
            return chunk;
        }
    }

    return NULL;
}