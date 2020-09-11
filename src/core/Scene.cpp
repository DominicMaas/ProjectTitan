#include "Scene.h"
#include "Renderer.h"
#include "managers/PipelineManager.h"

Scene::Scene(Camera *camera) {
    this->_mainCamera = camera;

    // Create a uniform buffer for the view projection matrix within the scene
    VmaAllocationInfo uniformBufferAllocInfo = {};
    Renderer::Instance->createBuffer(_sceneUboBuffer, _sceneUboAllocation, uniformBufferAllocInfo,
                                     sizeof(SceneUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU,
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Allocate the descriptor set
    auto* pipeline = PipelineManager::getPipeline("basic");
    _descriptorSet = pipeline->createUBODescriptorSet();

    // Bind the uniform buffer
    vk::DescriptorBufferInfo bufferInfo = {
            .buffer = _sceneUboBuffer,
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

    Renderer::Instance->Device.updateDescriptorSets(descriptorWrite, nullptr);
}

Scene::~Scene() {
    vmaDestroyBuffer(Renderer::Instance->Allocator, _sceneUboBuffer, _sceneUboAllocation);
}

void Scene::addRenderable(std::string name, Renderable* renderable) {
    if (_renderables.find(name) == _renderables.end()) {
        _renderables.insert(name, renderable);
        renderable->build();
    } else {
        throw std::invalid_argument("Could not add the renderable, a renderable of this name already exists.");
    }
}

void Scene::render(vk::CommandBuffer &commandBuffer, const std::string &pipelineName) {
    // Bind the scene descriptor set
    auto* pipeline = PipelineManager::getPipeline(pipelineName);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->getPipelineLayout(), 0, 1, &_descriptorSet, 0, nullptr);

    // Render all the renderables
    for (boost::ptr_map<std::string, Renderable>::iterator e = _renderables.begin(); e != _renderables.end(); e++) {
        e->second->render(commandBuffer, pipelineName);
    }
}

void Scene::update(long double deltaTime) {
    // Write updates for the camera
    SceneUBO ubo {};
    ubo.view = _mainCamera->getViewMatrix();
    ubo.proj = _mainCamera->getProjectionMatrix();
    ubo.proj[1][1] *= -1; // Adjust for Vulkan coords, vs OpenGL coords

    // Copy to the correct memory location
    void* mappedData;
    vmaMapMemory(Renderer::Instance->Allocator, _sceneUboAllocation, &mappedData);
    memcpy(mappedData, &ubo, sizeof(ubo));
    vmaUnmapMemory(Renderer::Instance->Allocator, _sceneUboAllocation);

    for (boost::ptr_map<std::string, Renderable>::iterator e = _renderables.begin(); e != _renderables.end(); e++) {
        e->second->update(deltaTime);
    }
}