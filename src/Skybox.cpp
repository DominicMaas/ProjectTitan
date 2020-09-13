#include "Skybox.h"
#include "core/managers/ResourceManager.h"
#include "core/managers/PipelineManager.h"
#include "core/Renderer.h"

Skybox::Skybox() {
    // Build the mesh
    std::vector<Vertex> vertices(std::begin(_skyboxVertices), std::end(_skyboxVertices));
    _mesh = new Mesh(vertices, std::vector<unsigned short>(), std::vector<Texture>());
    _mesh->build();

    // Get a reference to the texture
    _texture = ResourceManager::getTexture("skybox");
    if (_texture == nullptr) {
        throw std::runtime_error("The required texture for the skybox could not be found!");
    }

    _pipeline = PipelineManager::getPipeline("skybox");
    if (_pipeline == nullptr) {
        throw std::runtime_error("The required pipeline for the skybox could not be found!");
    }

    // Create the uniform buffer
    _pipeline->createModelUBO(_uniformBuffer, _uniformAllocation, _descriptorSet);

    ModelUBO ubo {};
    ubo.model = glm::translate(glm::mat4(), glm::vec3(0,0,0));

    // Copy this data across to the local memory
    // TODO: Maybe move this to the GPU memory?
    void* mappedData;
    vmaMapMemory(Renderer::Instance->Allocator, _uniformAllocation, &mappedData);
    memcpy(mappedData, &ubo, sizeof(ubo));
    vmaUnmapMemory(Renderer::Instance->Allocator, _uniformAllocation);
}

Skybox::~Skybox() {
    vmaDestroyBuffer(Renderer::Instance->Allocator, _uniformBuffer, _uniformAllocation);

    delete _mesh;
}

void Skybox::render(vk::CommandBuffer &commandBuffer, glm::mat4 viewMatrix, glm::mat4 projMatrix) {
    // Bind the skybox pipeline
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline->getVKPipeline());

    // Bind the texture
    _texture->bind(commandBuffer);

    // Bind the static model position
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipeline->getPipelineLayout(), 1, 1, &_descriptorSet, 0, nullptr);

    // Render the mesh
    _mesh->render(commandBuffer);
}

