#pragma once

#include <pch.h>

class Renderable {
public:
    glm::vec3 Position;
    glm::vec3 Rotation;

    virtual void build() = 0;
    virtual void render(vk::CommandBuffer &commandBuffer, const std::string &pipelineName) = 0;
    virtual void update(long double deltaTime) = 0;
};
