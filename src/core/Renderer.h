#pragma once

#include <pch.h>
#include "ImageSet.h"

class Renderer {
public:
    static Renderer* Instance;
    Renderer() {
        Instance = this;
    }

    vk::CommandPool CommandPool;
    vk::Device Device;
    vk::PhysicalDevice PhysicalDevice;
    vk::Queue GraphicsQueue;

    vk::SampleCountFlagBits MSAASamples = vk::SampleCountFlagBits::e1;

    VmaAllocator Allocator;

    vk::CommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

    // Copy a single buffer from source to destination
    void copyBuffer(vk::Buffer source, vk::Buffer destination, uint64_t size);
    void copyBuffer(VkBuffer source, vk::Buffer destination, uint64_t size);

    void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, uint32_t layerCount, uint32_t mipLevels);

    void createBuffer(vk::Buffer &buffer, VmaAllocation &allocation, VmaAllocationInfo &allocationInfo, uint64_t size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage, int memoryFlags = 0, int memoryRequiredFlags = 0);

    void createImage(vk::Image &image, VmaAllocation &allocation, uint32_t width, uint32_t height, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, uint32_t layerCount, uint32_t mipLevels, vk::ImageUsageFlags usage, vk::ImageCreateFlagBits flags);

    vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags, vk::ImageViewType imageViewType, uint32_t layerCount, uint32_t mipLevels);

    // Copy a buffer to an image
    void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount);

    void generateMipmaps(vk::Image image, vk::Format imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

    // Find an appropriate depth format
    vk::Format findDepthFormat() {
        return findSupportedFormat(
                { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
                vk::ImageTiling::eOptimal,
                vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );
    }

    vk::SampleCountFlagBits getMaxUsableSampleCount() {
        assert(PhysicalDevice);

        auto physicalDeviceProperties = PhysicalDevice.getProperties();

        vk::SampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
        if (counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
        if (counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
        if (counts & vk::SampleCountFlagBits::e8) { return vk::SampleCountFlagBits::e8; }
        if (counts & vk::SampleCountFlagBits::e4) { return vk::SampleCountFlagBits::e4; }
        if (counts & vk::SampleCountFlagBits::e2) { return vk::SampleCountFlagBits::e2; }

        return vk::SampleCountFlagBits::e1;
    }

    void destroyImageSet(ImageSet imageSet);
};
