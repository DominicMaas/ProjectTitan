#pragma once

#include <pch.h>

#include "Shader.h"
#include "Camera.h"
#include <limits>
#include "core/managers/BlockManager.h"
#include "World.h"
#include "Block.h"
#include "Mesh.h"
#include "core/BlockMap.h"
#include <reactphysics3d/reactphysics3d.h>

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
    Block ***_blocks;

    Mesh* _mesh;

    glm::mat4 _modelMatrix;

    bool isTransparent(int x, int y, int z);

    unsigned int getBlockType(int x, int y, int z);

    World *_world;

    bool _changed = true;
    bool _loaded = false;
    bool _loading = false;

    reactphysics3d::Collider* _collider = nullptr;
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