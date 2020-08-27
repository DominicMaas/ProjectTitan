#pragma once

#include "constants.h"
#include <string>
#include <vector>
#include "Chunk.h"
#include "Camera.h"
#include <thread>
#include <glm/glm.hpp>
#include "Skybox.h"
#include <algorithm>
#include "worldgen/BaseWorldGen.h"
#include "worldgen/StandardWorldGen.h"
#include <reactphysics3d/reactphysics3d.h>

// Define Chunk class to prevent compile Issues (Probably a better way to do it)
class Chunk;

class World {
private:
    // Physics
    reactphysics3d::PhysicsWorld *_physicsWorld;
    reactphysics3d::PhysicsCommon *_physicsCommon;

    reactphysics3d::RigidBody *_worldBody;

    std::vector<Chunk *> _chunks;
    Skybox _worldSkybox;

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
    void render(Camera &c);
    void updatePhysics(long double deltaAccum);

    void reset(bool resetSeed);

    // Constants
    static const int LOADED_CHUNKS_PER_FRAME = 3;
    static const int REBUILD_CHUNKS_PER_FRAME = 3;

    Chunk *findChunk(glm::vec3 position);

    // Get the world generator for this world
    BaseWorldGen *getWorldGen() { return _worldGen; }

    // Physics
    reactphysics3d::PhysicsWorld *getPhysicsWorld() { return _physicsWorld; };
    reactphysics3d::PhysicsCommon *getPhysicsCommon() { return _physicsCommon; };
    reactphysics3d::RigidBody *getWorldBody() { return _worldBody; };

    int ChunksRendered;
    int ChunksFrustumCulled;
};
