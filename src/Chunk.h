#pragma once

#include <pch.h>

#include <limits>
#include <reactphysics3d/reactphysics3d.h>

#include "Camera.h"
#include "core/managers/BlockManager.h"
#include "World.h"
#include "Block.h"
#include "core/Mesh.h"
#include "core/BlockMap.h"

// Define World class to prevent compile Issues (Probably a better way to do it)
class World;
class Mesh;

class Chunk {
private:
    vk::Buffer _uniformBuffer;
    VmaAllocation _uniformAllocation;
    vk::DescriptorSet _descriptorSet;

    // Data
    glm::vec3 _position;
    Block* _blocks;

    Mesh* _mesh;

    glm::mat4 _modelMatrix;

    bool isTransparent(int x, int y, int z);

    unsigned char getBlockType(int x, int y, int z);
    World *_world;

    bool _changed = true;
    bool _loaded = false;
    bool _loading = false;

    reactphysics3d::Collider* _collider = nullptr;

    void setBlockArrayType(int x, int y, int z, unsigned char type)
    {
        _blocks[z * CHUNK_WIDTH * CHUNK_HEIGHT + y * CHUNK_WIDTH + x] = Block { .material = type };
    }

    unsigned char getBlockArrayType(int x, int y, int z)
    {
        return _blocks[z * CHUNK_WIDTH * CHUNK_HEIGHT + y * CHUNK_WIDTH + x].material;
    }


public:
    Chunk(glm::vec3 position, World *world);

    ~Chunk();

    void load();

    void render(vk::CommandBuffer &commandBuffer);

    void rebuild();

    bool shouldRebuildChunk() { return _changed; }

    void setChanged() { _changed = true; }

    glm::vec3 getPosition() { return _position; }

    glm::vec3 getCenter() { return glm::vec3(_position.x + (CHUNK_WIDTH / 2), 0, _position.z + (CHUNK_WIDTH / 2)); }

    bool isLoaded() {
        return _loaded;
    }

    bool isLoading() { return _loading; }

    // Get the collider for the current chunk
    //reactphysics3d::Collider *getChunkCollider() { return _collider; }
};