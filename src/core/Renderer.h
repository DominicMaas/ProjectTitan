#pragma once

#include <pch.h>

class Renderer {
public:
    static Renderer* Instance;
    Renderer() {
        Instance = this;
    }

    vk::CommandPool CommandPool;
    vk::Device Device;
    vk::Queue GraphicsQueue;

    VmaAllocator Allocator;

    vk::CommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

    // Copy a single buffer from source to destination
    void copyBuffer(vk::Buffer source, vk::Buffer destination, uint64_t size);
    void copyBuffer(VkBuffer source, vk::Buffer destination, uint64_t size);

    void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

    void createBuffer(vk::Buffer &buffer, VmaAllocation &allocation, VmaAllocationInfo &allocationInfo, uint64_t size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage, int memoryFlags = 0, int memoryRequiredFlags = 0);

    void createImage(vk::Image &image, VmaAllocation &allocation, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);

    vk::ImageView createImageView(vk::Image image, vk::Format format);

    // Copy a buffer to an image
    void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
};
