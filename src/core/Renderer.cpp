#include "Renderer.h"

Renderer* Renderer::Instance;

vk::CommandBuffer Renderer::beginSingleTimeCommands() {
    // Allocate a command buffer to use
    vk::CommandBufferAllocateInfo allocInfo = {
            .commandPool = CommandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1 };
    vk::CommandBuffer commandBuffer = Device.allocateCommandBuffers(allocInfo)[0];

    // We are only running this once
    commandBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

    return commandBuffer;
}

void Renderer::endSingleTimeCommands(vk::CommandBuffer commandBuffer) {
    // End
    commandBuffer.end();

    vk::SubmitInfo submitInfo = {
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer };

    // Submit and wait
    GraphicsQueue.submit(1, &submitInfo, nullptr);
    GraphicsQueue.waitIdle();

    // Cleanup
    Device.freeCommandBuffers(CommandPool, 1, &commandBuffer);
}

void Renderer::copyBuffer(vk::Buffer source, vk::Buffer destination, uint64_t size) {
    // Copy data from staging buffer to actual buffer. Memory transfer operations are executed using command
    // pools, so we need to create a new temporary command pool and execute it.
    auto commandBuffer = Renderer::Instance->beginSingleTimeCommands();

    vk::BufferCopy copyRegion = { .srcOffset = 0, .dstOffset = 0, .size = size };
    commandBuffer.copyBuffer(source, destination, 1, &copyRegion);

    Renderer::Instance->endSingleTimeCommands(commandBuffer);
}

void Renderer::copyBuffer(VkBuffer source, vk::Buffer destination, uint64_t size) {
    // Marshal into the correct format
    vk::Buffer tempBuffer = source;
    copyBuffer(tempBuffer, destination, size);
}

void Renderer::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout,
                                     vk::ImageLayout newLayout) {
    auto commandBuffer = Renderer::Instance->beginSingleTimeCommands();
    endSingleTimeCommands(commandBuffer);
}

void Renderer::createBuffer(vk::Buffer &buffer, VmaAllocation &allocation, VmaAllocationInfo &allocationInfo, uint64_t size, VkBufferUsageFlags bufferUsage,
    VmaMemoryUsage memoryUsage, int memoryFlags, int memoryRequiredFlags) {
    VkBufferCreateInfo ubInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    ubInfo.size = size;
    ubInfo.usage = bufferUsage;
    ubInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo ubAllocCreateInfo = {};
    ubAllocCreateInfo.usage = memoryUsage;
    ubAllocCreateInfo.requiredFlags = memoryRequiredFlags;
    ubAllocCreateInfo.flags = memoryFlags;

    VkBuffer tempBuffer;
    vmaCreateBuffer(Allocator, &ubInfo, &ubAllocCreateInfo, &tempBuffer, &allocation, &allocationInfo);

    buffer = tempBuffer;
}
