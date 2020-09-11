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

    void addRenderable(std::string name, Renderable* renderable, RenderableData renderableData);

    void render(vk::CommandBuffer &commandBuffer, const std::string &pipelineName);
    void update(RenderableData input, long double deltaTime);

    void destroy(RenderableData renderableData);

    vk::DescriptorSet getDescriptorSet() { return _descriptorSet; }
};