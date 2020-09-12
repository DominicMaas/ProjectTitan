#include "World.h"
#include "core/managers/ResourceManager.h"
#include "core/Frustum.h"
#include "core/managers/PipelineManager.h"

void World::rebuildChunks() {
    _rebuiltChunksThisFrame = 0;

    // Loop through all the chunks
    for (Chunk *chunk : _chunks) {
        if (chunk->isLoaded() && chunk->shouldRebuildChunk()) {
            // Only reload a certain number of chunks per frame
            if (_rebuiltChunksThisFrame < REBUILD_CHUNKS_PER_FRAME) {
                chunk->rebuild();
                _rebuiltChunksThisFrame++;
            }
        }
    }
}

static void loadChunk(Chunk* chunk) {
    chunk->load();
}

void World::loadChunks() {
    int chunksLoaded = 0;

    // Loop through all the chunks
    for (Chunk *chunk : _chunks) {
        // If the chunk needs to be loaded, and it's not currently loading
        if (!chunk->isLoaded() && !chunk->isLoaded()) {

            //_futures.push_back(std::async(std::launch::async, loadChunk, chunk));
            loadChunk(chunk);
            chunksLoaded++;
        }

        if (chunksLoaded > LOADED_CHUNKS_PER_FRAME) {
            return;
        }
    }
}

World::World(int seed, std::string worldName, reactphysics3d::PhysicsCommon *physics) {
    reactphysics3d::PhysicsWorld::WorldSettings settings;
    settings.gravity = reactphysics3d::Vector3(0, -9.81, 0);

    // Create the physics world
    _physicsCommon = physics;
    _physicsWorld = _physicsCommon->createPhysicsWorld(settings);

    // Create a collision body for this world
    _worldBody = _physicsWorld->createRigidBody(reactphysics3d::Transform());
    _worldBody->setType(reactphysics3d::BodyType::STATIC);
    _worldBody->enableGravity(false);

    // World properties
    _sunDirection = glm::vec3(0.0f, -1.0f, 0.8f);
    _sunColor = glm::vec3(1, 1, 1);
    _sunSpeed = 0.0f;
    _sunAmbient = 0.4f;

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
    //std::vector<std::string> faces
   // {
   //     "textures/skybox/right.jpg",
   //     "textures/skybox/left.jpg",
   //     "textures/skybox/top.jpg",
   //     "textures/skybox/bottom.jpg",
    //    "textures/skybox/front.jpg",
   //     "textures/skybox/back.jpg"
    //};

    //this->_worldSkybox.setup(faces);
}

World::World(std::string worldName, reactphysics3d::PhysicsCommon *physics) : World(0, worldName, physics) {}

World::~World() {
    // Remove all chunks
    for (Chunk *b : _chunks) {
        delete b;
    }

    for (Entity *e : _entities) {
        delete e;
    }

    _chunks.clear();
    _entities.clear();

    delete _worldGen;
}

void World::update(float deltaTime, Camera &c) {
    // Update the sun position
    float sunVelocity = _sunSpeed * deltaTime;
    glm::mat4 rotationMat(1);
    rotationMat = glm::rotate(rotationMat, sunVelocity, glm::vec3(0.0, 0.0, 1.0));
    _sunDirection = glm::vec3(rotationMat * glm::vec4(_sunDirection, 1.0));

    // Load any chunks
    loadChunks();

    // Rebuild any chunks
    rebuildChunks();

    float renderDistance = 8 * CHUNK_WIDTH;

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

    // Update entities
    for (Entity* entity : _entities) {
        entity->update(deltaTime);
    }
}

void World::updatePhysics(long double timeStep, long double accumulator) {
    for (Entity* entity : _entities) {
        entity->updatePhysics(timeStep, accumulator);
    }
}

void World::render(vk::CommandBuffer &commandBuffer, Camera &c) {
    // Calculate the frustum
    Frustum frustum = Frustum::GetFrustum(c.getProjectionMatrix() * c.getViewMatrix());

    // The render distance
    float renderDistance = 8 * CHUNK_WIDTH;

    // Bind the blocks texture
    auto* pipeline = PipelineManager::getPipeline("basic");
    auto* basicTexture = ResourceManager::getTexture("block_map");
    basicTexture->bind(commandBuffer, pipeline->getPipelineLayout());

    // Keep track of the number of chunks being rendered
    ChunksRendered = 0;

    // Loop through all the chunks
    for (Chunk *chunk : _chunks) {
        // This chunk is not loaded
        if (!chunk->isLoaded())
            continue;

        // The chunk is not in the players view distance
        if (abs(chunk->getCenter().x - c.getPosition().x) >= renderDistance ||
            abs(chunk->getCenter().z - c.getPosition().z) >= renderDistance)
            continue;

        // TODO: Fix this when lighting is working
        // Ensure the chunk is in the frustum
        bool isVisible = frustum.isBoxVisible(chunk->getPosition(), chunk->getPosition() + glm::vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH));
        if (!isVisible)
            continue;

        ChunksRendered++;

        // Render the chunk
        chunk->render(commandBuffer);
    }

    for (Entity* entity : _entities) {
        //entity->render(shader);
    }
}

void World::postRender(Camera &c, Shader &shader) {
    // Render the skybox
    //this->_worldSkybox.render(c.getViewMatrix(), c.getProjectionMatrix());
}

void World::reset(bool resetSeed) {
    // Rebuild all chunks
    for (Chunk *chunk : _chunks) {
        chunk->setChanged();
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

    return nullptr;
}

