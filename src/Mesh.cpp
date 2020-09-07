#include "Mesh.h"

Mesh::Mesh() {
    this->Vertices = std::vector<Vertex>();
    this->Indices = std::vector<unsigned int>();
    this->Textures = std::vector<Texture>();

    this->_built = false;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->Vertices = vertices;
    this->Indices = indices;
    this->Textures = textures;

    this->_built = false;
}

Mesh::~Mesh() {
    // Delete the arrays and buffers
    //GLCall(glDeleteVertexArrays(1, &_vao));
    //GLCall(glDeleteBuffers(1, &_ebo));
    //GLCall(glDeleteBuffers(1, &_vbo));
}

void Mesh::rebuild(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->Vertices = vertices;
    this->Indices = indices;
    this->Textures = textures;

    this->_built = false;

    //build(nullptr);
}

void Mesh::build(RenderableData input) {
    assert(input.allocator);
    assert(input.device);
    assert(input.commandPool);
    assert(input.graphicsQueue);

    // If the mesh has already been built, we need to destroy it first
    if (_built) {
        destroy(input);
    }

    // ------------------ Create Vertex Buffer ------------------ //
    VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    vbInfo.size = sizeof(Vertex) * Vertices.size();
    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo vbAllocCreateInfo = {};
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    vbAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer stagingVertexBuffer = VK_NULL_HANDLE;
    VmaAllocation stagingVertexBufferAlloc = VK_NULL_HANDLE;
    VmaAllocationInfo stagingVertexBufferAllocInfo = {};
    vmaCreateBuffer(input.allocator, &vbInfo, &vbAllocCreateInfo, &stagingVertexBuffer, &stagingVertexBufferAlloc, &stagingVertexBufferAllocInfo);

    memcpy(stagingVertexBufferAllocInfo.pMappedData, Vertices.data(), (size_t) vbInfo.size);

    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vbAllocCreateInfo.flags = 0;

    VkBuffer tempVertexBuffer;
    vmaCreateBuffer(input.allocator, &vbInfo, &vbAllocCreateInfo, &tempVertexBuffer, &_vertexAllocation, nullptr);
    _vertexBuffer = tempVertexBuffer;

    // ------------------ Create Index Buffer ------------------ //
    VkBufferCreateInfo ibInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    ibInfo.size = sizeof(unsigned int) * Indices.size();
    ibInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    ibInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo ibAllocCreateInfo = {};
    ibAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    ibAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer stagingIndexBuffer = VK_NULL_HANDLE;
    VmaAllocation stagingIndexBufferAlloc = VK_NULL_HANDLE;
    VmaAllocationInfo stagingIndexBufferAllocInfo = {};

    if (!Indices.empty()) {
        if (vmaCreateBuffer(input.allocator, &ibInfo, &ibAllocCreateInfo, &stagingIndexBuffer, &stagingIndexBufferAlloc, &stagingIndexBufferAllocInfo) != VK_SUCCESS) {
            throw std::runtime_error("Could not create the temporary index buffer");
        }

        memcpy(stagingIndexBufferAllocInfo.pMappedData, Indices.data(), (size_t) ibInfo.size);

        ibInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        ibAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        ibAllocCreateInfo.flags = 0;

        VkBuffer tempIndexBuffer;
        if (vmaCreateBuffer(input.allocator, &ibInfo, &ibAllocCreateInfo, &tempIndexBuffer, &_indexAllocation, nullptr) != VK_SUCCESS) {
            throw std::runtime_error("Could not create the index buffer");
        }
        _indexBuffer = tempIndexBuffer;
    }

    // ------------------ Copy Buffers ------------------ //

    // Copy data from staging CPU buffer to actual GPU buffer. Memory transfer operations are executed using command
    // pools, so we need to create a new temporary command pool and execute it.

    // Allocate a command buffer to use
    vk::CommandBufferAllocateInfo allocInfo = {
            .commandPool = input.commandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1 };
    vk::CommandBuffer commandBuffer = input.device.allocateCommandBuffers(allocInfo)[0];

    // We are only running this once
    commandBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

    // Copy the vertex buffer
    vk::BufferCopy vertexCopyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = vbInfo.size };
    commandBuffer.copyBuffer(stagingVertexBuffer, _vertexBuffer, 1, &vertexCopyRegion);

    if (!Indices.empty()) {
        // Copy the index buffer
        vk::BufferCopy indexCopyRegion = {
                .srcOffset = 0,
                .dstOffset = 0,
                .size = ibInfo.size };
        commandBuffer.copyBuffer(stagingIndexBuffer, _indexBuffer, 1, &indexCopyRegion);
    }

    // End
    commandBuffer.end();

    vk::SubmitInfo submitInfo = {
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer
    };

    // Submit and wait
    input.graphicsQueue.submit(1, &submitInfo, nullptr);
    input.graphicsQueue.waitIdle();

    // Cleanup
    input.device.freeCommandBuffers(input.commandPool, 1, &commandBuffer);

    // ------------------ Destroy Staging Buffers ------------------ //

    vmaDestroyBuffer(input.allocator, stagingVertexBuffer, stagingVertexBufferAlloc);

    if (!Indices.empty()) {
        vmaDestroyBuffer(input.allocator, stagingIndexBuffer, stagingIndexBufferAlloc);
    }

    _built = true;
}

void Mesh::destroy(RenderableData input) {
    assert(input.allocator);

    // Always destroy the vertex buffer
    vmaDestroyBuffer(input.allocator, _vertexBuffer, _vertexAllocation);

    // Only destroy the index buffer if it exists
    if (_indexBuffer) {
        vmaDestroyBuffer(input.allocator, _indexBuffer, _indexAllocation);
    }
}

void Mesh::render(vk::CommandBuffer &commandBuffer) {
    // Only render if the mesh has been built
    if (!_built) {
        spdlog::warn("[Mesh] Attempted to render mesh before it was built");
        return;
    }

    // Bind
    vk::Buffer vertexBuffers[] = { _vertexBuffer };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

    // Draw
    if (Indices.empty()) {
        commandBuffer.draw(Vertices.size(), 1, 0, 0);
    } else {
        commandBuffer.bindIndexBuffer(_indexBuffer, 0, vk::IndexType::eUint16);
        commandBuffer.drawIndexed(static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0);
    }




    // Textures
    /*unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < Textures.size(); i++) {
        GLCall(glActiveTexture(GL_TEXTURE0 + i)); // activate proper texture unit before binding

        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = Textures[i].type;

        if (name == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        } else if (name == "texture_specular") {
            number = std::to_string(specularNr++);
        }

        shader.setFloat(("material." + name + number).c_str(), i);
        GLCall(glBindTexture(GL_TEXTURE_2D, Textures[i].id));
    }

    // Draw mesh
    GLCall(glBindVertexArray(_vao));

    // Draw from the element buffer
    if (!Indices.empty()) {
        GLCall(glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0));
    } else { // Draw from the array buffer
        GLCall(glDrawArrays(GL_TRIANGLES, 0, Vertices.size()));
    }

    // Cleanup
    GLCall(glActiveTexture(GL_TEXTURE0));*/
}


