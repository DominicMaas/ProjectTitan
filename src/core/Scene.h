#pragma once

#include <pch.h>
#include "../Camera.h"
#include "Renderable.h"
#include <boost/concept_check.hpp>
#include <boost/ptr_container/ptr_map.hpp>

class Scene {
private:
    Camera* _mainCamera;
    boost::ptr_map<std::string, Renderable> _renderables;

    vk::Buffer _sceneUboBuffer;
    VmaAllocation _sceneUboAllocation;
    vk::DescriptorSet _descriptorSet;

public:
    Scene(Camera* camera);
    ~Scene();

    void addRenderable(std::string name, Renderable* renderable);

    void render(vk::CommandBuffer &commandBuffer, const std::string &pipelineName);
    void update(long double deltaTime);

    vk::DescriptorSet getDescriptorSet() { return _descriptorSet; }
};