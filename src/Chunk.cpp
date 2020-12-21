#include "Chunk.h"
#include "core/managers/ResourceManager.h"
#include "core/managers/PipelineManager.h"
#include "core/Renderer.h"

Chunk::Chunk(glm::vec2 position, World *world) {
    // Set chunk details
    _position = position;
    _world = world;

    // Update the model matrix to the correct position
    _modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(_position, 0));

    // Create a new empty mesh
    _mesh = new Mesh();

    // ------------------ Create Uniform Buffer ------------------ //

    // Create the descriptor set to store the chunk position
    auto* pipeline = PipelineManager::getPipeline("basic");
    if (pipeline == nullptr) {
        throw std::invalid_argument("Unable to retrieve the specified pipeline ('basic')");
    }

    pipeline->createModelUBO(_uniformBuffer, _uniformAllocation, _descriptorSet);

    ModelUBO ubo {};
    ubo.model = _modelMatrix;

    // Copy this data across to the local memory
    // TODO: Maybe move this to the GPU memory?
    void* mappedData;
    vmaMapMemory(Renderer::Instance->Allocator, _uniformAllocation, &mappedData);
    memcpy(mappedData, &ubo, sizeof(ubo));
    vmaUnmapMemory(Renderer::Instance->Allocator, _uniformAllocation);

    // Create the blocks
    _backgroundBlocks.resize(CHUNK_WIDTH * CHUNK_HEIGHT);
    _foregroundBlocks.resize(CHUNK_WIDTH * CHUNK_HEIGHT);
}

Chunk::~Chunk() {
    // Remove the world collider
    if (_collider != nullptr) {
        _world->getWorldBody()->removeCollider(_collider);
    }

    vmaDestroyBuffer(Renderer::Instance->Allocator, _uniformBuffer, _uniformAllocation);

    // Delete the mesh
    delete _mesh;
}

void Chunk::load() {
    _loading = true;

    // Build height map
    for (int x = 0; x < CHUNK_WIDTH; x++)
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            auto material = _world->getWorldGen()->getTheoreticalBlockType(_position.x + x, _position.y + y,0);

            setBlockArrayType(x, y, ChunkLayer::LAYER_BACKGROUND, material);
        }

    for (int x = 0; x < CHUNK_WIDTH; x++)
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            auto material = _world->getWorldGen()->getTheoreticalBlockType(_position.x + x, _position.y + y,1);

            setBlockArrayType(x, y, ChunkLayer::LAYER_FOREGROUND, material);
        }

    _loaded = true;
    _loading = false;
}

void Chunk::render(vk::CommandBuffer &commandBuffer) {
    // Quick check to make sure this chunk is loaded
    if (!_loaded) return;

    // Only render if the mesh is ready to render
    if (!_mesh->isBuilt()) return;

    // Bind the descriptor set for the chunk position
    auto* pipeline = PipelineManager::getPipeline("basic");
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->getPipelineLayout(), 1, 1, &_descriptorSet, 0, nullptr);

    // Render the mesh
    _mesh->render(commandBuffer);
}

bool Chunk::isTransparent(int x, int y, ChunkLayer layer) {
    if (y < 0) return false;

    if (getBlockType(x, y, layer) == BlockManager::BLOCK_AIR)
        return true;

    return false;
}

unsigned char Chunk::getBlockType(int x, int y, ChunkLayer layer) {
    if ((y >= CHUNK_HEIGHT))
        return BlockManager::BLOCK_AIR;

    if ((x < 0) || (x >= CHUNK_WIDTH)) {
        // Calculate world coordinates
        glm::vec2 worldPos(x, y);
        worldPos += _position;

        // Find the chunk that contains these coordinates
        Chunk *c = _world->findChunk(worldPos);

        // This is "air", render the side of the face
        if (c == nullptr || !c->isLoaded())
        {
            switch (layer)
            {
                case LAYER_BACKGROUND:
                    return _world->getWorldGen()->getTheoreticalBlockType(worldPos.x, worldPos.y, 0);
                    break;
                case LAYER_FOREGROUND:
                    return _world->getWorldGen()->getTheoreticalBlockType(worldPos.x, worldPos.y, 1);
                    break;
            }
        }

        // Calculate local space coordinates
        glm::vec2 cLocal = worldPos -= c->getPosition();
        return c->getBlockType(cLocal.x, cLocal.y, layer);
    }

    return getBlockArrayType(x, y, layer);
}

void Chunk::rebuild() {
    // Do not rebuild if the chunk has not yet been loaded
    if (!_loaded) return;

    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;

    int currIndex = 0;

    /*for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            // Get the id at this position
            char material = getBlockType(x, y, ChunkLayer::LAYER_BACKGROUND);

            // Don't render Air
            if (material == BlockManager::BLOCK_AIR)
                continue;

            // Check all edges of the block
            int index = 0;

            //if (isTransparent(x, y, z)) index |= 1;
            //if (isTransparent(x + 1, y, z)) index |= 2;
            //if (isTransparent(x + 1, y + 1, z)) index |= 4;
            //if (isTransparent(x, y + 1, z)) index |= 8;
            //if (isTransparent(x, y, z + 1)) index |= 16;
            //if (isTransparent(x + 1, y, z + 1)) index |= 32;
            //if (isTransparent(x + 1, y + 1, z + 1)) index |= 64;
            //if (isTransparent(x, y + 1, z + 1)) index |= 128;

            // Get block data
            glm::vec2 texCoords[BlockManager::BLOCK_FACE_SIZE][BlockManager::TEX_COORD_SIZE];
            BlockManager::getTextureFromId(material, texCoords);

            // Front
            //if (isTransparent(x, y, z - 1)) {
            vertices.push_back(
                    Vertex(1 + x, 1 + y, 0, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::TopRight]));
            vertices.push_back(
                    Vertex(1 + x, 0 + y, 0, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::BottomRight]));
            vertices.push_back(
                    Vertex(0 + x, 0 + y, 0, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::BottomLeft]));
            vertices.push_back(
                    Vertex(0 + x, 1 + y, 0, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::TopLeft]));

            indices.push_back(currIndex + 0);
            indices.push_back(currIndex + 1);
            indices.push_back(currIndex + 3);

            indices.push_back(currIndex + 1);
            indices.push_back(currIndex + 2);
            indices.push_back(currIndex + 3);

            currIndex += 4;
            //}
        }
    }*/

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            // Get the id at this position
            //unsigned char material = getBlockType(x, y, ChunkLayer::LAYER_FOREGROUND);
            char material = BlockManager::BLOCK_DIRT;

            // Don't render Air
            if (material == BlockManager::BLOCK_AIR)
                continue;

            // Check all edges of the block
            int index = 0;

            //if (isTransparent(x, y, z)) index |= 1;
            //if (isTransparent(x + 1, y, z)) index |= 2;
            //if (isTransparent(x + 1, y + 1, z)) index |= 4;
            //if (isTransparent(x, y + 1, z)) index |= 8;
            //if (isTransparent(x, y, z + 1)) index |= 16;
            //if (isTransparent(x + 1, y, z + 1)) index |= 32;
            //if (isTransparent(x + 1, y + 1, z + 1)) index |= 64;
            //if (isTransparent(x, y + 1, z + 1)) index |= 128;

            // Get block data
            glm::vec2 texCoords[BlockManager::BLOCK_FACE_SIZE][BlockManager::TEX_COORD_SIZE];
            BlockManager::getTextureFromId(material, texCoords);

            // Front
            //if (isTransparent(x, y, z - 1)) {
            vertices.push_back(
                    Vertex(1 + x, 1 + y, 1, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::TopRight]));
            vertices.push_back(
                    Vertex(1 + x, 0 + y, 1, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::BottomRight]));
            vertices.push_back(
                    Vertex(0 + x, 0 + y, 1, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::BottomLeft]));
            vertices.push_back(
                    Vertex(0 + x, 1 + y, 1, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::TopLeft]));

            indices.push_back(currIndex + 0);
            indices.push_back(currIndex + 1);
            indices.push_back(currIndex + 3);

            indices.push_back(currIndex + 1);
            indices.push_back(currIndex + 2);
            indices.push_back(currIndex + 3);

            currIndex += 4;
            //}
        }
    }

    // Rebuild the visual mesh
    _mesh->rebuild(vertices, indices, std::vector<Texture>());

    /*if (_collider != nullptr) {
        _world->getWorldBody()->removeCollider(_collider);
    }

    // Create the polygon vertex array
    auto* triangleArray = new reactphysics3d::TriangleVertexArray(
            _mesh->Vertices.size(), _mesh->Vertices.data(), sizeof(Vertex), indices.size() / 3,
            _mesh->Indices.data(), 3 * sizeof(unsigned short),
            reactphysics3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
            reactphysics3d::TriangleVertexArray::IndexDataType::INDEX_SHORT_TYPE);

    // Convert the chunk position into physics coords
    reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
    reactphysics3d::Transform transform(reactphysics3d::Vector3(_position.x, _position.y, _position.z), orientation);

    // Perform the mesh collider rebuild
    auto physicsMesh = _world->getPhysicsCommon()->createTriangleMesh();
    physicsMesh->addSubpart(triangleArray);

    // Create a physics shape based on this mesh
    auto physicsMeshShape = _world->getPhysicsCommon()->createConcaveMeshShape(physicsMesh);

    // Create the collider for this chunk and add it to the world body
    _collider = _world->getWorldBody()->addCollider(physicsMeshShape, transform);*/

    // The chunk has been rebuilt
    _changed = false;
}


