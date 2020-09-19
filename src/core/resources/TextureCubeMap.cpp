/*#include "TextureCubeMap.h"
#include "../Renderer.h"
#include "../managers/PipelineManager.h"

TextureCubeMap::~TextureCubeMap() {
    Renderer::Instance->Device.destroySampler(_textureSampler);
    Renderer::Instance->Device.destroyImageView(_textureImageSet.imageView);
    vmaDestroyImage(Renderer::Instance->Allocator, _textureImageSet.image, _textureImageSet.allocation);
}

void TextureCubeMap::load(std::vector<unsigned char*> data, int width, int height, LoadTextureInfo info) {
    this->_width = width;
    this->_height = height;

    this->_mipmapLevels = static_cast<unsigned int>(std::floor(std::log2(std::max(_width, _height)))) + 1;

    _pipeline = PipelineManager::getPipeline(info.pipeline);
    if (_pipeline == nullptr) {
        throw std::runtime_error("The specified pipeline provided to the texture does not exist!");
    }

    vk::ImageCreateFlagBits createFlags = {};
    vk::ImageViewType imageViewType = vk::ImageViewType::e2D;

    // Can only pass in multiple textures if creating a cube map
    if (data.size() > 1 && !info.cubeMap) {
        throw std::runtime_error("You can only specify multiple textures if creating a cubemap!");
    }

    if (info.cubeMap) {
        createFlags = vk::ImageCreateFlagBits::eCubeCompatible;
        imageViewType = vk::ImageViewType::eCube;
    }

    auto imageSize = width * height * 4 * data.size();
    auto layerSize = imageSize / data.size();

    // ON CPU
    vk::Buffer stagingBuffer = nullptr;
    VmaAllocation stagingBufferAlloc = VK_NULL_HANDLE;
    VmaAllocationInfo stagingBufferAllocInfo = {};
    Renderer::Instance->createBuffer(stagingBuffer, stagingBufferAlloc, stagingBufferAllocInfo,
                                     imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY,
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT);

    // Copy the into the staging buffer.
    for (int i = 0; i < data.size(); ++i)
    {
        memcpy(static_cast<char*>(stagingBufferAllocInfo.pMappedData) + (layerSize * i), data[i], layerSize);
    }

    // ON GPU
    Renderer::Instance->createImage(_textureImageSet.image, _textureImageSet.allocation, width, height, vk::SampleCountFlagBits::e1, info.format, vk::ImageTiling::eOptimal, data.size(), vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, createFlags);

    // Transition image for transfer
    Renderer::Instance->transitionImageLayout(_textureImageSet.image, info.format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, data.size());

    // Transfer to GPU
    Renderer::Instance->copyBufferToImage(stagingBuffer, _textureImageSet.image, width, height, data.size());

    // Transition for shader usage
    Renderer::Instance->transitionImageLayout(_textureImageSet.image, info.format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, data.size());

    // Cleanup staging buffers
    vmaDestroyBuffer(Renderer::Instance->Allocator, stagingBuffer, stagingBufferAlloc);

    // Create the texture image view
    _textureImageSet.imageView = Renderer::Instance->createImageView(_textureImageSet.image, info.format, vk::ImageAspectFlagBits::eColor, imageViewType, data.size());

    // Setup sampling
    vk::SamplerCreateInfo samplerInfo = {
            .magFilter = info.filter,
            .minFilter = info.filter,
            .mipmapMode = info.mipmapMode,
            .addressModeU = info.addressMode,
            .addressModeV = info.addressMode,
            .addressModeW = info.addressMode,
            .mipLodBias = 0.0f,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = 16.0f,
            .compareEnable = VK_FALSE,
            .compareOp = vk::CompareOp::eAlways,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .borderColor = vk::BorderColor::eIntOpaqueBlack,
            .unnormalizedCoordinates = VK_FALSE };

    _textureSampler = Renderer::Instance->Device.createSampler(samplerInfo);

    // Setup the descriptor set
    _descriptorSet = _pipeline->createTexSamplerDescriptorSet();

    // Bind the uniform buffer
    vk::DescriptorImageInfo imageInfo = {
            .sampler = _textureSampler,
            .imageView = _textureImageSet.imageView,
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal };

    vk::WriteDescriptorSet descriptorWrite = {
            .dstSet = _descriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = &imageInfo };

    Renderer::Instance->Device.updateDescriptorSets(descriptorWrite, nullptr);
}

void TextureCubeMap::bind(vk::CommandBuffer &commandBuffer) const {
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipeline->getPipelineLayout(), 2, 1, &_descriptorSet, 0, nullptr);
}
*/