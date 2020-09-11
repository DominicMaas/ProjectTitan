#include "Mesh.h"
#include "core/managers/PipelineManager.h"
#include "core/Renderer.h"

Mesh::Mesh(const std::string& pipelineName) {
    this->_pipelineName = pipelineName;

    this->Vertices = std::vector<Vertex>();
    this->Indices = std::vector<unsigned short>();
    this->Textures = std::vector<Texture>();

    this->_built = false;
}

Mesh::Mesh(const std::string& pipelineName, std::vector<Vertex> vertices, std::vector<unsigned short> indices, std::vector<Texture> textures) {
    this->_pipelineName = pipelineName;

    this->Vertices = vertices;
    this->Indices = indices;
    this->Textures = textures;

    this->_built = false;
}

void Mesh::rebuild(std::vector<Vertex> vertices, std::vector<unsigned short> indices, std::vector<Texture> textures, RenderableData input) {
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

    // If the mesh has already been built, we need to destroy it first
    if (_built) {
        destroy(input);
    }

    // ------------------ Create Uniform Buffer ------------------ //
    // This will be done on local memory for now, May copy over to GPU later
    // on, since the mesh model should not be updated too often.
    VmaAllocationInfo uniformBufferAllocInfo = {};
    Renderer::Instance->createBuffer(_uniformBuffer, _uniformAllocation,uniformBufferAllocInfo,
         sizeof(ModelUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU,
         VMA_ALLOCATION_CREATE_MAPPED_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

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

    // ON CPU
    vk::Buffer stagingIndexBuffer = nullptr;
    VmaAllocation stagingIndexBufferAlloc = VK_NULL_HANDLE;
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

    // ------------------ Copy Buffers ------------------ //

    Renderer::Instance->copyBuffer(stagingVertexBuffer, _vertexBuffer, vertexSize);
    Renderer::Instance->copyBuffer(stagingIndexBuffer, _indexBuffer, indexSize);

    // ------------------ Destroy Staging Buffers ------------------ //

    vmaDestroyBuffer(input.allocator, stagingVertexBuffer, stagingVertexBufferAlloc);
    vmaDestroyBuffer(input.allocator, stagingIndexBuffer, stagingIndexBufferAlloc);

    // ------------------ Create the descriptor set ------------------ //

    auto* pipeline = PipelineManager::getPipeline(_pipelineName);
    if (pipeline == nullptr) {
        throw std::invalid_argument("Unable to retrieve the specified pipeline ("+_pipelineName+")");
    }

    // Allocate the descriptor set
    _descriptorSet = pipeline->createUBODescriptorSet();

    // Bind the uniform buffer
    vk::DescriptorBufferInfo bufferInfo = {
        .buffer = _uniformBuffer,
        .offset = 0,
        .range = sizeof(ModelUBO)
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

void Mesh::render(vk::CommandBuffer &commandBuffer, const std::string &pipelineName) {
    // Only render if the mesh has been built
    if (!_built) {
        spdlog::warn("[Mesh] Attempted to render mesh before it was built");
        return;
    }

    auto* pipeline = PipelineManager::getPipeline(pipelineName);

    // Bind the square texture
    auto* basicTexture = ResourceManager::getTexture("square");
    basicTexture->bind(commandBuffer, pipeline->getPipelineLayout());

    // Bind
    vk::Buffer vertexBuffers[] = { _vertexBuffer };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

    // Bind the descriptor set
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->getPipelineLayout(), 1, 1, &_descriptorSet, 0, nullptr);

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
    }*/
}

void Mesh::update(RenderableData input, long double deltaTime) {
    assert(input.allocator);

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    ModelUBO ubo {};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Copy this data across to the local memory
    // TODO: Maybe move this to the GPU memory?
    void* mappedData;
    vmaMapMemory(input.allocator, _uniformAllocation, &mappedData);
    memcpy(mappedData, &ubo, sizeof(ubo));
    vmaUnmapMemory(input.allocator, _uniformAllocation);
}


