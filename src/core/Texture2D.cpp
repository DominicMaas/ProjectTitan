#include "Texture2D.h"
#include "Renderer.h"
#include "managers/PipelineManager.h"

Texture2D::~Texture2D() {
    Renderer::Instance->Device.destroySampler(_textureSampler);
    Renderer::Instance->Device.destroyImageView(_textureImageView);
    vmaDestroyImage(Renderer::Instance->Allocator, _textureImage, _allocation);
}

void Texture2D::load(unsigned char *pixels, int width, int height) {
    this->_width = width;
    this->_height = height;

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
    Renderer::Instance->createImage(_textureImage, _allocation, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    // Transition image for transfer
    Renderer::Instance->transitionImageLayout(_textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

    // Transfer to GPU
    Renderer::Instance->copyBufferToImage(stagingBuffer, _textureImage, width, height);

    // Transition for shader usage
    Renderer::Instance->transitionImageLayout(_textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    // Cleanup staging buffers
    vmaDestroyBuffer(Renderer::Instance->Allocator, stagingBuffer, stagingBufferAlloc);

    // Create the texture image view
    _textureImageView = Renderer::Instance->createImageView(_textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);

    // Setup sampling
    vk::SamplerCreateInfo samplerInfo = {
            .magFilter = vk::Filter::eLinear,
            .minFilter = vk::Filter::eLinear,
            .addressModeU = vk::SamplerAddressMode::eRepeat,
            .addressModeV = vk::SamplerAddressMode::eRepeat,
            .addressModeW = vk::SamplerAddressMode::eRepeat,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = 16.0f,
            .borderColor = vk::BorderColor::eIntOpaqueBlack,
            .unnormalizedCoordinates = VK_FALSE,
            .compareEnable = VK_FALSE,
            .compareOp = vk::CompareOp::eAlways,
            .mipmapMode = vk::SamplerMipmapMode::eLinear,
            .mipLodBias = 0.0f,
            .minLod = 0.0f,
            .maxLod = 0.0f
    };

    _textureSampler = Renderer::Instance->Device.createSampler(samplerInfo);

    auto* pipeline = PipelineManager::getPipeline("basic");

    // Setup the descriptor set
    _descriptorSet = pipeline->createTexSamplerDescriptorSet();

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

void Texture2D::bind(vk::CommandBuffer &commandBuffer, vk::PipelineLayout pipelineLayout) const {
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 2, 1, &_descriptorSet, 0, nullptr);
}
