#pragma once

#include <pch.h>

#include <future>
#include "Chunk.h"
#include "Camera.h"
#include <thread>
#include "Skybox.h"
#include <algorithm>
#include "worldgen/BaseWorldGen.h"
#include "worldgen/StandardWorldGen.h"
#include <reactphysics3d/reactphysics3d.h>
#include "effects/ShadowMapping.h"
#include "Entity.h"

// Define Chunk class to prevent compile Issues (Probably a better way to do it)
class Chunk;
class Entity;

class World {
private:
    // Physics
    reactphysics3d::PhysicsWorld *_physicsWorld;
    reactphysics3d::PhysicsCommon *_physicsCommon;

    reactphysics3d::RigidBody *_worldBody;

    std::vector<Chunk *> _chunks;
    std::vector<Entity *> _entities;

    Skybox _worldSkybox;

    // Keep track of any futures
    std::vector<std::future<void>> _futures;

    // Lighting
    glm::vec3 _sunDirection;
    glm::vec3 _sunColor;
    float _sunSpeed;
    float _sunAmbient;

    // Chunk rebuilding
    int _rebuiltChunksThisFrame;

    void rebuildChunks();

    // Chunk loading
    int _loadedChunksThisFrame;

    void loadChunks();

    // World gen
    BaseWorldGen *_worldGen;

public:
    World(int seed, std::string worldName, reactphysics3d::PhysicsCommon *physics);
    World(std::string worldName, reactphysics3d::PhysicsCommon *physics);

    ~World();

    void update(Camera &c, long double delta);
    void updatePhysics(long double timeStep, long double accumulator);

    void render(Camera &c, Shader &shader);
    void postRender(Camera &c, Shader &shader);

    void reset(bool resetSeed);

    // Constants
    static const int LOADED_CHUNKS_PER_FRAME = 3;
    static const int REBUILD_CHUNKS_PER_FRAME = 2;

    Chunk *findChunk(glm::vec3 position);

    // Get the world generator for this world
    BaseWorldGen *getWorldGen() { return _worldGen; }

    // Physics
    reactphysics3d::PhysicsWorld *getPhysicsWorld() { return _physicsWorld; };
    reactphysics3d::PhysicsCommon *getPhysicsCommon() { return _physicsCommon; };
    reactphysics3d::RigidBody *getWorldBody() { return _worldBody; };

    int ChunksRendered;
    int ChunksFrustumCulled;

    glm::vec3 SunPosition = glm::vec3(0.0f, -1.0f, 0.8f);

    glm::mat4 getLightSpaceMatrix(Camera& camera) {

        glm::vec3 sunPos(0,1000.0f,0);

        auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, ShadowMapping::NEAR_PLANE, ShadowMapping::FAR_PLANE);
        auto lightView = glm::lookAt(sunPos, sunPos + (SunPosition * glm::vec3(10)), glm::vec3(0.0, 1.0, 0.0));

        //auto pos = glm::translate(lightProjection, -camera.getPosition());
        return lightProjection * lightView;
    }

    void addEntity(Entity* entity) {
        _entities.push_back(entity);
    }
};
