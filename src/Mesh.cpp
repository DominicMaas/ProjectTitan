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

void Mesh::rebuild(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, RenderableData input) {
    this->Vertices = vertices;
    this->Indices = indices;
    this->Textures = textures;

    this->_built = false;

    build(input);
}

void Mesh::build(RenderableData input) {
    assert(input.allocator);
    assert(input.device);
    assert(input.commandPool);
    assert(input.graphicsQueue);
    assert(input.descriptorPool);

    // If the mesh has already been built, we need to destroy it first
    if (_built) {
        destroy(input);
    }

    // ------------------ Create Uniform Buffer ------------------ //
    // This will be done on local memory for now, May copy over to GPU later
    // on, since the mesh model should not be updated too often.
    VkBufferCreateInfo ubInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    ubInfo.size = sizeof(UniformBufferObject);
    ubInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    ubInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo ubAllocCreateInfo = {};
    ubAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_COPY;
    ubAllocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    ubAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer tempUniformBuffer;
    vmaCreateBuffer(input.allocator, &ubInfo, &ubAllocCreateInfo, &tempUniformBuffer, &_uniformAllocation, nullptr);
    _uniformBuffer = tempUniformBuffer;

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

    // ------------------ Create the descriptor set ------------------ //

    vk::DescriptorSetLayout descriptorSetLayout[] = { input.graphicsPipeline.getDescriptorSetLayout() };
    vk::DescriptorSetAllocateInfo descriptorAllocInfo = {
            .descriptorPool = input.descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = descriptorSetLayout };

    // Allocate the descriptor set
    _descriptorSet = input.device.allocateDescriptorSets(descriptorAllocInfo)[0];

    // Bind the uniform buffer
    vk::DescriptorBufferInfo bufferInfo = {
        .buffer = _uniformBuffer,
        .offset = 0,
        .range = sizeof(UniformBufferObject)
    };

    vk::WriteDescriptorSet descriptorWrite = {
            .dstSet = _descriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pBufferInfo = &bufferInfo };

    input.device.updateDescriptorSets(descriptorWrite, nullptr);

    _built = true;
}

void Mesh::destroy(RenderableData input) {
    assert(input.allocator);

    // Destroy the uniform buffer
    vmaDestroyBuffer(input.allocator, _uniformBuffer, _uniformAllocation);

    // Always destroy the vertex buffer
    vmaDestroyBuffer(input.allocator, _vertexBuffer, _vertexAllocation);

    // Only destroy the index buffer if it exists
    if (_indexBuffer) {
        vmaDestroyBuffer(input.allocator, _indexBuffer, _indexAllocation);
    }
}

void Mesh::render(vk::CommandBuffer &commandBuffer, GraphicsPipeline &pipeline) {
    // Only render if the mesh has been built
    if (!_built) {
        spdlog::warn("[Mesh] Attempted to render mesh before it was built");
        return;
    }

    // Bind
    vk::Buffer vertexBuffers[] = { _vertexBuffer };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

    // Bind the descriptor set
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.getPipelineLayout(), 0, 1, &_descriptorSet, 0, nullptr);

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

void Mesh::update(RenderableData input, long double deltaTime) {
    assert(input.allocator);

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 10.0f);

    // Copy this data across to the local memory
    // TODO: Maybe move this to the GPU memory?
    void* mappedData;
    vmaMapMemory(input.allocator, _uniformAllocation, &mappedData);
    memcpy(mappedData, &ubo, sizeof(ubo));
    vmaUnmapMemory(input.allocator, _uniformAllocation);
}


