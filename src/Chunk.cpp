#include "Chunk.h"
#include "core/managers/ResourceManager.h"
#include "core/managers/PipelineManager.h"
#include "core/Renderer.h"

Chunk::Chunk(glm::vec3 position, World *world) {
    // Set chunk details
    _position = position;
    _world = world;

    // Update the model matrix to the correct position
    _modelMatrix = glm::translate(glm::mat4(1.0f), _position);

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
    _blocks = new Block **[CHUNK_WIDTH];
    for (int i = 0; i < CHUNK_WIDTH; i++) {
        _blocks[i] = new Block *[CHUNK_HEIGHT];

        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            _blocks[i][j] = new Block[CHUNK_WIDTH];
        }
    }
}

Chunk::~Chunk() {
    // Delete the blocks
    for (int i = 0; i < CHUNK_WIDTH; ++i) {
        for (int j = 0; j < CHUNK_HEIGHT; ++j) {
            delete[] _blocks[i][j];
        }

        delete[] _blocks[i];
    }
    delete[] _blocks;

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
        for (int y = 0; y < CHUNK_HEIGHT; y++)
            for (int z = 0; z < CHUNK_WIDTH; z++) {
                _blocks[x][y][z].setMaterial(
                        _world->getWorldGen()->getTheoreticalBlockType(_position.x + x, _position.y + y,
                                                                       _position.z + z));
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

bool Chunk::isTransparent(int x, int y, int z) {
    if (y < 0) return false;

    if (getBlockType(x, y, z) == BlockManager::BLOCK_AIR)
        return true;

    return false;
}

unsigned int Chunk::getBlockType(int x, int y, int z) {
    if ((y >= CHUNK_HEIGHT))
        return BlockManager::BLOCK_AIR;

    if ((x < 0) || (z < 0) || (x >= CHUNK_WIDTH) || (z >= CHUNK_WIDTH)) {
        // Calculate world coordinates
        glm::vec3 worldPos(x, y, z);
        worldPos += _position;

        // Find the chunk that contains these coordinates
        Chunk *c = _world->findChunk(worldPos);

        // This is "air", render the side of the face
        if (c == nullptr || !c->isLoaded())
            return _world->getWorldGen()->getTheoreticalBlockType(worldPos.x, worldPos.y, worldPos.z);

        // Calculate local space coordinates
        glm::vec3 cLocal = worldPos -= c->getPosition();
        return c->getBlockType(cLocal.x, cLocal.y, cLocal.z);
    }

    return _blocks[x][y][z].getMaterial();
}

void Chunk::rebuild() {
    // Do not rebuild if the chunk has not yet been loaded
    if (!_loaded) return;

    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;

    int currIndex = 0;

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_WIDTH; z++) {
                // Get the id at this position
                Block b = _blocks[x][y][z];

                // Don't render Air
                if (b.getMaterial() == BlockManager::BLOCK_AIR)
                    continue;

                // Check all edges of the block
                int index = 0;

                if (isTransparent(x, y, z)) index |= 1;
                if (isTransparent(x + 1, y, z)) index |= 2;
                if (isTransparent(x + 1, y + 1, z)) index |= 4;
                if (isTransparent(x, y + 1, z)) index |= 8;
                if (isTransparent(x, y, z + 1)) index |= 16;
                if (isTransparent(x + 1, y, z + 1)) index |= 32;
                if (isTransparent(x + 1, y + 1, z + 1)) index |= 64;
                if (isTransparent(x, y + 1, z + 1)) index |= 128;

                // Get block data
                glm::vec2 texCoords[BlockManager::BLOCK_FACE_SIZE][BlockManager::TEX_COORD_SIZE];
                BlockManager::getTextureFromId(b.getMaterial(), texCoords);

                // Front
                if (isTransparent(x, y, z - 1)) {
                    vertices.push_back(Vertex(1 + x, 1 + y, 0 + z, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::TopRight]));
                    vertices.push_back(Vertex(1 + x, 0 + y, 0 + z, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::BottomRight]));
                    vertices.push_back(Vertex(0 + x, 0 + y, 0 + z, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::BottomLeft]));
                    vertices.push_back(Vertex(0 + x, 1 + y, 0 + z, 0, 0, -1, texCoords[BlockManager::Front][BlockManager::TopLeft]));

                    indices.push_back(currIndex + 0);
                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 3);

                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 2);
                    indices.push_back(currIndex + 3);

                    currIndex += 4;
                }

                // Back
                if (isTransparent(x, y, z + 1)) {
                    vertices.push_back(Vertex(0 + x, 0 + y, 1 + z, 0, 0, 1, texCoords[BlockManager::Back][BlockManager::BottomLeft]));
                    vertices.push_back(Vertex(1 + x, 0 + y, 1 + z, 0, 0, 1, texCoords[BlockManager::Back][BlockManager::BottomRight]));
                    vertices.push_back(Vertex(1 + x, 1 + y, 1 + z, 0, 0, 1, texCoords[BlockManager::Back][BlockManager::TopRight]));
                    vertices.push_back(Vertex(0 + x, 1 + y, 1 + z, 0, 0, 1, texCoords[BlockManager::Back][BlockManager::TopLeft]));

                    indices.push_back(currIndex + 0);
                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 3);

                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 2);
                    indices.push_back(currIndex + 3);

                    currIndex += 4;
                }

                // Right
                if (isTransparent(x - 1, y, z)) {
                    vertices.push_back(Vertex(0 + x, 1 + y, 1 + z, -1, 0, 0, texCoords[BlockManager::Right][BlockManager::TopRight]));
                    vertices.push_back(Vertex(0 + x, 1 + y, 0 + z, -1, 0, 0, texCoords[BlockManager::Right][BlockManager::TopLeft]));
                    vertices.push_back(Vertex(0 + x, 0 + y, 0 + z, -1, 0, 0, texCoords[BlockManager::Right][BlockManager::BottomLeft]));
                    vertices.push_back(Vertex(0 + x, 0 + y, 1 + z, -1, 0, 0, texCoords[BlockManager::Right][BlockManager::BottomRight]));

                    indices.push_back(currIndex + 0);
                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 3);

                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 2);
                    indices.push_back(currIndex + 3);

                    currIndex += 4;
                }

                // Left
                if (isTransparent(x + 1, y, z)) {
                    vertices.push_back(Vertex(1 + x, 0 + y, 0 + z, 1, 0, 0, texCoords[BlockManager::Left][BlockManager::BottomLeft]));
                    vertices.push_back(Vertex(1 + x, 1 + y, 0 + z, 1, 0, 0, texCoords[BlockManager::Left][BlockManager::TopLeft]));
                    vertices.push_back(Vertex(1 + x, 1 + y, 1 + z, 1, 0, 0, texCoords[BlockManager::Left][BlockManager::TopRight]));
                    vertices.push_back(Vertex(1 + x, 0 + y, 1 + z, 1, 0, 0, texCoords[BlockManager::Left][BlockManager::BottomRight]));

                    indices.push_back(currIndex + 0);
                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 3);

                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 2);
                    indices.push_back(currIndex + 3);

                    currIndex += 4;
                }

                // Down
                if (isTransparent(x, y - 1, z)) {
                    vertices.push_back(Vertex(0 + x, 0 + y, 0 + z, 0, -1, 0, texCoords[BlockManager::Bottom][BlockManager::TopLeft]));
                    vertices.push_back(Vertex(1 + x, 0 + y, 0 + z, 0, -1, 0, texCoords[BlockManager::Bottom][BlockManager::TopRight]));
                    vertices.push_back(Vertex(1 + x, 0 + y, 1 + z, 0, -1, 0, texCoords[BlockManager::Bottom][BlockManager::BottomRight]));
                    vertices.push_back(Vertex(0 + x, 0 + y, 1 + z, 0, -1, 0, texCoords[BlockManager::Bottom][BlockManager::BottomLeft]));

                    indices.push_back(currIndex + 0);
                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 3);

                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 2);
                    indices.push_back(currIndex + 3);

                    currIndex += 4;
                }

                // Up
                if (isTransparent(x, y + 1, z)) {
                    vertices.push_back(Vertex(1 + x, 1 + y, 1 + z, 0, 1, 0, texCoords[BlockManager::Top][BlockManager::BottomRight]));
                    vertices.push_back(Vertex(1 + x, 1 + y, 0 + z, 0, 1, 0, texCoords[BlockManager::Top][BlockManager::TopRight]));
                    vertices.push_back(Vertex(0 + x, 1 + y, 0 + z, 0, 1, 0, texCoords[BlockManager::Top][BlockManager::TopLeft]));
                    vertices.push_back(Vertex(0 + x, 1 + y, 1 + z, 0, 1, 0, texCoords[BlockManager::Top][BlockManager::BottomLeft]));

                    indices.push_back(currIndex + 0);
                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 3);

                    indices.push_back(currIndex + 1);
                    indices.push_back(currIndex + 2);
                    indices.push_back(currIndex + 3);

                    currIndex += 4;
                }
            }
        }
    }

    // Rebuild the visual mesh
    _mesh->rebuild(vertices, indices, std::vector<Texture>());

    if (_collider != nullptr) {
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
    _collider = _world->getWorldBody()->addCollider(physicsMeshShape, transform);

    // The chunk has been rebuilt
    _changed = false;
}