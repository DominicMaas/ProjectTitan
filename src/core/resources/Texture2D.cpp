#include "Texture2D.h"
#include "../Renderer.h"
#include "../managers/PipelineManager.h"

Texture2D::~Texture2D() {
    Renderer::Instance->Device.destroySampler(_textureSampler);
    Renderer::Instance->Device.destroyImageView(_textureImageView);
    vmaDestroyImage(Renderer::Instance->Allocator, _textureImage, _allocation);
}

void Texture2D::load(unsigned char *pixels, int width, int height, LoadTextureInfo info) {
    this->_width = width;
    this->_height = height;

    _pipeline = PipelineManager::getPipeline(info.pipeline);
    if (_pipeline == nullptr) {
        throw std::runtime_error("The specified pipeline provided to the texture does not exist!");
    }

    auto imageSize = width * height * 4;

    // ON CPU
    vk::Buffer stagingBuffer = nullptr;
    VmaAllocation stagingBufferAlloc = VK_NULL_HANDLE;
    VmaAllocationInfo stagingBufferAllocInfo = {};
    Renderer::Instance->createBuffer(stagingBuffer, stagingBufferAlloc, stagingBufferAllocInfo,
                                     imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY,
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT);

    // Copy to buffer
    memcpy(stagingBufferAllocInfo.pMappedData, pixels, imageSize);

    // ON GPU
    Renderer::Instance->createImage(_textureImage, _allocation, width, height, vk::SampleCountFlagBits::e1, info.format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);

    // Transition image for transfer
    Renderer::Instance->transitionImageLayout(_textureImage, info.format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

    // Transfer to GPU
    Renderer::Instance->copyBufferToImage(stagingBuffer, _textureImage, width, height);

    // Transition for shader usage
    Renderer::Instance->transitionImageLayout(_textureImage, info.format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    // Cleanup staging buffers
    vmaDestroyBuffer(Renderer::Instance->Allocator, stagingBuffer, stagingBufferAlloc);

    // Create the texture image view
    _textureImageView = Renderer::Instance->createImageView(_textureImage, info.format, vk::ImageAspectFlagBits::eColor);

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
            .imageView = _textureImageView,
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

void Texture2D::bind(vk::CommandBuffer &commandBuffer) const {
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipeline->getPipelineLayout(), 2, 1, &_descriptorSet, 0, nullptr);
}
