#pragma once

#include <pch.h>
#include "../GraphicsPipeline.h"

struct RenderableData {
    VmaAllocator &allocator;
    vk::Device &device;
    vk::CommandPool &commandPool;
    vk::Queue &graphicsQueue;
};

class GraphicsPipeline;

class Renderable {
public:
    glm::vec3 Position;
    glm::vec3 Rotation;

    virtual void build(RenderableData input) = 0;
    virtual void render(vk::CommandBuffer &commandBuffer, GraphicsPipeline &pipeline) = 0;
    virtual void update(RenderableData input, long double deltaTime) = 0;
    virtual void destroy(RenderableData input) = 0;
};
