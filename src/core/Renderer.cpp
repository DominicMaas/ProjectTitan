#include "Renderer.h"

Renderer* Renderer::Instance;

vk::CommandBuffer Renderer::beginSingleTimeCommands() {
    assert(Device);
    assert(CommandPool);

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
    assert(Device);
    assert(GraphicsQueue);
    assert(CommandPool);

    // End
    commandBuffer.end();

    auto submitInfo = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setCommandBuffers(commandBuffer);

    // Submit and wait
    GraphicsQueue.submit(1, &submitInfo, nullptr);
    GraphicsQueue.waitIdle();

    // Cleanup
    Device.freeCommandBuffers(CommandPool, 1, &commandBuffer);
}

void Renderer::copyBuffer(vk::Buffer source, vk::Buffer destination, uint64_t size) {
    // Copy data from staging buffer to actual buffer. Memory transfer operations are executed using command
    // pools, so we need to create a new temporary command pool and execute it.
    auto commandBuffer = beginSingleTimeCommands();

    vk::BufferCopy copyRegion = { .srcOffset = 0, .dstOffset = 0, .size = size };
    commandBuffer.copyBuffer(source, destination, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

void Renderer::copyBuffer(VkBuffer source, vk::Buffer destination, uint64_t size) {
    // Marshal into the correct format
    vk::Buffer tempBuffer = source;
    copyBuffer(tempBuffer, destination, size);
}

void Renderer::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout,
                                     vk::ImageLayout newLayout, uint32_t layerCount) {
    auto commandBuffer = beginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier = {
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = layerCount
            }
    };

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;

    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, 0, nullptr, 0, nullptr, 1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

void Renderer::createBuffer(vk::Buffer &buffer, VmaAllocation &allocation, VmaAllocationInfo &allocationInfo, uint64_t size, VkBufferUsageFlags bufferUsage,
        VmaMemoryUsage memoryUsage, int memoryFlags, int memoryRequiredFlags) {
    assert(Allocator);

    VkBufferCreateInfo ubInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    ubInfo.size = size;
    ubInfo.usage = bufferUsage;
    ubInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo ubAllocCreateInfo = {};
    ubAllocCreateInfo.usage = memoryUsage;
    ubAllocCreateInfo.requiredFlags = memoryRequiredFlags;
    ubAllocCreateInfo.flags = memoryFlags;

    auto res = vmaCreateBuffer(Allocator, &ubInfo, &ubAllocCreateInfo, reinterpret_cast<VkBuffer *>(&buffer), &allocation, &allocationInfo);
    if (res != VK_SUCCESS) {
        spdlog::error("[Renderer] Could not create buffer: {}", res);
    }
}

void Renderer::createImage(vk::Image &image, VmaAllocation &allocation, uint32_t width, uint32_t height, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, uint32_t layerCount, vk::ImageUsageFlags usage, vk::ImageCreateFlagBits flags) {
    assert(Allocator);

    vk::ImageCreateInfo imageInfo = {};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = layerCount;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.flags = flags;

    auto tempCreateInfo = VkImageCreateInfo(imageInfo);

    VmaAllocationCreateInfo imageAllocCreateInfo = {};
    imageAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkImage tempImage;
    vmaCreateImage(Allocator, &tempCreateInfo, &imageAllocCreateInfo, &tempImage, &allocation, nullptr);
    image = tempImage;
}

vk::ImageView Renderer::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags, vk::ImageViewType imageViewType, uint32_t layerCount) {
    assert(Device);

    vk::ImageViewCreateInfo createInfo = {
            .image = image,
            .viewType = imageViewType,
            .format = format,
            .components = {
                    .r = vk::ComponentSwizzle::eIdentity,
                    .g = vk::ComponentSwizzle::eIdentity,
                    .b = vk::ComponentSwizzle::eIdentity,
                    .a = vk::ComponentSwizzle::eIdentity,
            },
            .subresourceRange = {
                    .aspectMask = aspectFlags,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = layerCount
            }
    };

    return Device.createImageView(createInfo);
}

void Renderer::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount) {
    auto commandBuffer = beginSingleTimeCommands();

    vk::BufferImageCopy region = {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = layerCount,
            },
            .imageOffset = {0, 0, 0},
            .imageExtent = { width, height,1 }};

    // Perform the actual copy
    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

vk::Format Renderer::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
    assert(PhysicalDevice);

    for (vk::Format format : candidates) {
        vk::FormatProperties props = PhysicalDevice.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format!");
}

void Renderer::destroyImageSet(ImageSet imageSet) {
    assert(Device);
    assert(Allocator);

    Device.destroyImageView(imageSet.imageView);
    vmaDestroyImage(Allocator, imageSet.image, imageSet.allocation);
}
