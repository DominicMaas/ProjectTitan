#pragma once

#include "constants.h"
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include <limits>
#include "BlockManager.h"
#include "World.h"
#include "Block.h"
#include "core/ExecutionTimer.h"
#include "core/BlockMap.h"
#include <reactphysics3d/reactphysics3d.h>

// Define World class to prevent compile Issues (Probably a better way to do it)
class World;

struct ChunkVertex {
    ChunkVertex(int px, int py, int pz, int nx, int ny, int nz, glm::vec2 texture) {
        Position = glm::vec3(px, py, pz);
        Normal = glm::vec3(nx, ny, nz);
        Texture = texture;
    }

    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 Texture;
};

class Chunk {
private:
    // Data
    glm::vec3 _position;
    Block ***_blocks;

    // Rendering
    unsigned int _vbo = 0;
    unsigned int _ebo = 0;
    unsigned int _vao = 0;

    glm::mat4 _modelMatrix;
    int _vertices;
    int _indexCount;

    bool isTransparent(int x, int y, int z);

    unsigned int getBlockType(int x, int y, int z);

    World *_world;

    bool _changed = true;
    bool _loaded = false;

    reactphysics3d::Collider *_collider;
    reactphysics3d::CollisionBody *_collisionBody;
public:
    Chunk(glm::vec3 position, World *world);

    ~Chunk();

    void load();

    void render();

    void rebuild();

    bool shouldRebuildChunk() { return _changed; }

    void setChanged() { _changed = true; }

    glm::vec3 getPosition() { return _position; }

    glm::vec3 getCenter() { return glm::vec3(_position.x + (CHUNK_WIDTH / 2), 0, _position.z + (CHUNK_WIDTH / 2)); }

    bool isLoaded() {
        return _loaded;
    }

    // Get the collider for the current chunk
    reactphysics3d::Collider *getChunkCollider() { return _collider; }
};