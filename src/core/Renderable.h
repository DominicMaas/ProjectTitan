#pragma once

#include "../pch.h"

struct RenderableData {
    VmaAllocator &allocator;
    vk::Device &device;
    vk::CommandPool &commandPool;
    vk::Queue &graphicsQueue;
};

class Renderable {
public:
    virtual void build(RenderableData input) = 0;
    virtual void render(vk::CommandBuffer &commandBuffer) = 0;
    virtual void destroy(RenderableData input) = 0;
};
