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

enum ChunkLayer {
    LAYER_BACKGROUND = 0x0001,
    LAYER_FOREGROUND = 0x0002
};

// Define World class to prevent compile Issues (Probably a better way to do it)
class World;
class Mesh;

class Chunk {
private:
    vk::Buffer _uniformBuffer;
    VmaAllocation _uniformAllocation;
    vk::DescriptorSet _descriptorSet;

    // Data
    glm::vec2 _position;
    std::vector<unsigned char> _backgroundBlocks;
    std::vector<unsigned char> _foregroundBlocks;

    Mesh* _mesh;

    glm::mat4 _modelMatrix;

    bool isTransparent(int x, int y, ChunkLayer layer);

    unsigned char getBlockType(int x, int y, ChunkLayer layer);
    World *_world;

    bool _changed = true;
    bool _loaded = false;
    bool _loading = false;

    reactphysics3d::Collider* _collider = nullptr;

    void setBlockArrayType(int x, int y, ChunkLayer layer, unsigned char type)
    {
        switch(layer)
        {
            case LAYER_BACKGROUND:
                _backgroundBlocks[CHUNK_HEIGHT + y * CHUNK_WIDTH + x] = type; //Block { .material = type };
                break;
            case LAYER_FOREGROUND:
                _foregroundBlocks[CHUNK_HEIGHT + y * CHUNK_WIDTH + x] = type; //Block { .material = type };
                break;
        }
    }

    unsigned char getBlockArrayType(int x, int y, ChunkLayer layer)
    {
        switch(layer)
        {
            case LAYER_BACKGROUND:
                return _backgroundBlocks[CHUNK_HEIGHT + y * CHUNK_WIDTH + x];//.material;
                break;
            case LAYER_FOREGROUND:
                return _foregroundBlocks[CHUNK_HEIGHT + y * CHUNK_WIDTH + x];//.material;
                break;
        }
    }


public:
    Chunk(glm::vec2 position, World *world);

    ~Chunk();

    void load();

    void render(vk::CommandBuffer &commandBuffer);

    void rebuild();

    bool shouldRebuildChunk() { return _changed; }

    void setChanged() { _changed = true; }

    glm::vec2 getPosition() { return _position; }

    glm::vec2 getCenter() { return glm::vec2(_position.x + (CHUNK_WIDTH / 2), 0); }

    bool isLoaded() {
        return _loaded;
    }

    bool isLoading() { return _loading; }

    // Get the collider for the current chunk
    //reactphysics3d::Collider *getChunkCollider() { return _collider; }
};