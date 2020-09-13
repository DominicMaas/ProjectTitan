#pragma once

#include <pch.h>
#include "../GraphicsPipeline.h"
#include "../ImageSet.h"

struct LoadTextureInfo {
    std::string pipeline = "basic";
    vk::Filter filter = vk::Filter::eLinear;
    vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat;
    vk::SamplerMipmapMode mipmapMode = vk::SamplerMipmapMode::eLinear;
    vk::Format format = vk::Format::eR8G8B8A8Srgb;
    bool flipTexture = true;
    bool cubeMap = false;
};

class Texture2D {
private:
    ImageSet _textureImageSet;

    vk::Sampler _textureSampler;

    vk::DescriptorSet _descriptorSet;

    GraphicsPipeline *_pipeline;

    int _width;
    int _height;

public:
    Texture2D() {}
    ~Texture2D();

    void load(std::vector<unsigned char*> data, int width, int height, LoadTextureInfo info);
    void bind(vk::CommandBuffer &commandBuffer) const;

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
};
