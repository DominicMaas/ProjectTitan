#pragma once

#include <pch.h>

class Texture2D {
private:
    VmaAllocation _allocation;

    vk::Image _textureImage;
    vk::ImageView _textureImageView;
    vk::Sampler _textureSampler;

    vk::DescriptorSet _descriptorSet;

    int _width;
    int _height;

public:
    Texture2D() {}
    ~Texture2D();

    void load(unsigned char *data, int width, int height);
    void bind(vk::CommandBuffer &commandBuffer, vk::PipelineLayout pipelineLayout) const;

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
};
