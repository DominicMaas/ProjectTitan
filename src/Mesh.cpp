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

void Mesh::build(VmaAllocator &allocator, vk::Device &device, vk::CommandPool commandPool, vk::Queue graphicsQueue) {
    // If the mesh has already been built, we need to destory it first
    if (_built) {
        destroy(allocator);
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
    vmaCreateBuffer(allocator, &vbInfo, &vbAllocCreateInfo, &stagingVertexBuffer, &stagingVertexBufferAlloc, &stagingVertexBufferAllocInfo);

    memcpy(stagingVertexBufferAllocInfo.pMappedData, Vertices.data(), (size_t) vbInfo.size);

    vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vbAllocCreateInfo.flags = 0;

    VkBuffer tempVertexBuffer;
    vmaCreateBuffer(allocator, &vbInfo, &vbAllocCreateInfo, &tempVertexBuffer, &_vertexAllocation, nullptr);
    _vertexBuffer = tempVertexBuffer;

    // ------------------ Create Index Buffer ------------------ //
    VkBufferCreateInfo iInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    iInfo.size = sizeof(unsigned int) * Indices.size();
    iInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    iInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo iAllocCreateInfo = {};
    iAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    iAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer stagingIndexBuffer = VK_NULL_HANDLE;
    VmaAllocation stagingIndexBufferAlloc = VK_NULL_HANDLE;
    VmaAllocationInfo stagingIndexBufferAllocInfo = {};

    if (!Indices.empty()) {
        vmaCreateBuffer(allocator, &iInfo, &iAllocCreateInfo, &stagingIndexBuffer, &stagingIndexBufferAlloc, &stagingIndexBufferAllocInfo);

        memcpy(stagingIndexBufferAllocInfo.pMappedData, Indices.data(), (size_t) iInfo.size);

        iInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        iAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        iAllocCreateInfo.flags = 0;

        VkBuffer tempIndexBuffer;
        vmaCreateBuffer(allocator, &iInfo, &iAllocCreateInfo, &tempIndexBuffer, &_indexAllocation, nullptr);
        _indexBuffer = tempIndexBuffer;
    }

    // ------------------ Copy Buffers ------------------ //

    // Copy data from staging CPU buffer to actual GPU buffer. Memory transfer operations are executed using command
    // pools, so we need to create a new temporary command pool and execute it.

    // Allocate a command buffer to use
    vk::CommandBufferAllocateInfo allocInfo = {
            .commandPool = commandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1 };
    vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(allocInfo)[0];

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
                .size = iInfo.size };
        commandBuffer.copyBuffer(stagingIndexBuffer, _indexBuffer, 1, &indexCopyRegion);
    }

    // End
    commandBuffer.end();

    vk::SubmitInfo submitInfo = {
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer
    };

    // Submit and wait
    graphicsQueue.submit(1, &submitInfo, nullptr);
    graphicsQueue.waitIdle();

    // Cleanup
    device.freeCommandBuffers(commandPool, 1, &commandBuffer);

    // ------------------ Destroy Staging Buffers ------------------ //

    vmaDestroyBuffer(allocator, stagingVertexBuffer, stagingVertexBufferAlloc);

    if (!Indices.empty()) {
        vmaDestroyBuffer(allocator, stagingIndexBuffer, stagingIndexBufferAlloc);
    }





    //VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    //bufferInfo.size = sizeof(Vertex) * Vertices.size();
    //bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    //bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    //VmaAllocationCreateInfo allocInfo = {};
    //allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    //allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    //VkBuffer tempBuffer;
    //if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &tempBuffer, &_allocation, nullptr) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to allocate vertex buffer!");
    //}

    // Convert to the c++ object
    //_vertexBuffer = tempBuffer;

    // What in the actual fuck is Vulkan
    //void* data;
    //vmaMapMemory(allocator, _allocation, &data);
    //memcpy(data, Vertices.data(), (size_t) bufferInfo.size);
    //vmaUnmapMemory(allocator, _allocation);

    // Bind to build
    //GLCall(glBindVertexArray(_vao));

    // Bind vertex buffer data
    //GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
    //GLCall(glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW));

    // Bind index buffer data
    //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo));
    //GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW));

    // Enable shader attributes
    //GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Position)));
    //GLCall(glEnableVertexAttribArray(0));

    //GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal)));
    //GLCall(glEnableVertexAttribArray(1));

    //GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords)));
    //GLCall(glEnableVertexAttribArray(2));

    // Unbind as built
    //GLCall(glBindVertexArray(0));

    _built = true;
}

void Mesh::destroy(VmaAllocator &allocator) {
    // Always destroy the vertex buffer
    vmaDestroyBuffer(allocator, _vertexBuffer, _vertexAllocation);

    // Only destroy the index buffer if it exists
    if (_indexBuffer) {
        vmaDestroyBuffer(allocator, _indexBuffer, _indexAllocation);
    }
}

void Mesh::render(vk::CommandBuffer commandBuffer) {
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
        commandBuffer.drawIndexed(Indices.size(), 1, 0, 0, 0);
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


