#include "Mesh.h"
#include "Renderer.h"

Mesh::Mesh() {
    this->Vertices = std::vector<Vertex>();
    this->Indices = std::vector<unsigned short>();
    this->Textures = std::vector<Texture>();

    this->_built = false;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned short> indices, std::vector<Texture> textures) {
    this->Vertices = vertices;
    this->Indices = indices;
    this->Textures = textures;

    this->_built = false;
}

Mesh::~Mesh() {
    assert(Renderer::Instance->Allocator);

    vmaDestroyBuffer(Renderer::Instance->Allocator, _vertexBuffer, _vertexAllocation);

    if (_hasIndices) {
        vmaDestroyBuffer(Renderer::Instance->Allocator, _indexBuffer, _indexAllocation);
    }
}


void Mesh::rebuild(std::vector<Vertex> vertices, std::vector<unsigned short> indices, std::vector<Texture> textures) {
    this->Vertices = vertices;
    this->Indices = indices;
    this->Textures = textures;

    this->_built = false;

    build();
}

void Mesh::build() {
    assert(Renderer::Instance->Allocator);
    assert(Renderer::Instance->Device);

    // If the mesh has already been built, we need to destroy it first
    if (_built) {
        if (_hasIndices) {
            vmaDestroyBuffer(Renderer::Instance->Allocator, _indexBuffer, _indexAllocation);
        }

        vmaDestroyBuffer(Renderer::Instance->Allocator, _vertexBuffer, _vertexAllocation);
        _built = false;
    }

    // Update this flag
    _hasIndices = !Indices.empty();

    // ------------------ Create Vertex Buffer ------------------ //

    auto vertexSize = sizeof(Vertex) * Vertices.size();

    // ON CPU
    vk::Buffer stagingVertexBuffer = nullptr;
    VmaAllocation stagingVertexBufferAlloc = VK_NULL_HANDLE;
    VmaAllocationInfo stagingVertexBufferAllocInfo = {};
    Renderer::Instance->createBuffer(stagingVertexBuffer, stagingVertexBufferAlloc, stagingVertexBufferAllocInfo,
                                     vertexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY,
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT);

    // Copy to buffer
    memcpy(stagingVertexBufferAllocInfo.pMappedData, Vertices.data(), (size_t) vertexSize);

    // On GPU
    VmaAllocationInfo vertexBufferAllocInfo = {};
    Renderer::Instance->createBuffer(_vertexBuffer, _vertexAllocation, vertexBufferAllocInfo, vertexSize,
                                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                     VMA_MEMORY_USAGE_GPU_ONLY);

    // ------------------ Create Index Buffer ------------------ //
    auto indexSize = sizeof(unsigned short) * Indices.size();

    vk::Buffer stagingIndexBuffer = nullptr;
    VmaAllocation stagingIndexBufferAlloc = VK_NULL_HANDLE;

    if (_hasIndices) {
        // ON CPU
        VmaAllocationInfo stagingIndexBufferAllocInfo = {};
        Renderer::Instance->createBuffer(stagingIndexBuffer, stagingIndexBufferAlloc, stagingIndexBufferAllocInfo,
                                         vertexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY,
                                         VMA_ALLOCATION_CREATE_MAPPED_BIT);

        // Copy to buffer
        memcpy(stagingIndexBufferAllocInfo.pMappedData, Indices.data(), (size_t) indexSize);

        // On GPU
        VmaAllocationInfo indexBufferAllocInfo = {};
        Renderer::Instance->createBuffer(_indexBuffer, _indexAllocation, indexBufferAllocInfo, indexSize,
                                         VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                         VMA_MEMORY_USAGE_GPU_ONLY);
    }

    // ------------------ Copy Buffers ------------------ //

    Renderer::Instance->copyBuffer(stagingVertexBuffer, _vertexBuffer, vertexSize);

    if (_hasIndices) {
        Renderer::Instance->copyBuffer(stagingIndexBuffer, _indexBuffer, indexSize);
    }

    // ------------------ Destroy Staging Buffers ------------------ //

    vmaDestroyBuffer(Renderer::Instance->Allocator, stagingVertexBuffer, stagingVertexBufferAlloc);

    if (_hasIndices) {
        vmaDestroyBuffer(Renderer::Instance->Allocator, stagingIndexBuffer, stagingIndexBufferAlloc);
    }

    _built = true;
}

void Mesh::render(vk::CommandBuffer &commandBuffer) {
    // Only render if the mesh has been built
    if (!_built) {
        spdlog::warn("[Mesh] Attempted to render mesh before it was built");
        return;
    }

    // Bind The vertex buffers
    vk::Buffer vertexBuffers[] = { _vertexBuffer };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

    // Draw
    if (_hasIndices) {
        commandBuffer.bindIndexBuffer(_indexBuffer, 0, vk::IndexType::eUint16);
        commandBuffer.drawIndexed(Indices.size(), 1, 0, 0, 0);
    } else {
        commandBuffer.draw(Vertices.size(), 1, 0, 0);
    }
}