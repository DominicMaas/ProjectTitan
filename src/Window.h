#pragma once

#include "pch.h"
#include "GraphicsPipeline.h"

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class GraphicsPipeline;

class Window {
public:
    Window(const char* title, unsigned int initialWidth, unsigned int initialHeight);

    void run();

    vk::Device getDevice() { return _device; }

    vk::Extent2D getSwapChainExtent() { return _swapChainExtent; }

    vk::RenderPass getRenderPass() { return _renderPass; }

private:
    const int MAX_FRAMES_IN_FLIGHT = 2;

    GLFWwindow* _window;
    unsigned int _width;
    unsigned int _height;
    const char* _title;

    size_t _currentFrame = 0;

    vk::Instance _instance;
    vk::SurfaceKHR _surface;
    vk::PhysicalDevice _physicalDevice;
    vk::Device _device;
    vk::Queue _graphicsQueue;
    vk::Queue _presentQueue;
    vk::SwapchainKHR _swapChain;
    vk::RenderPass _renderPass;
    vk::DebugUtilsMessengerEXT _debugMessenger;

    std::vector<vk::Image> _swapChainImages;
    std::vector<vk::ImageView> _swapChainImageViews;
    std::vector<vk::Framebuffer> _swapChainFrameBuffers;
    std::vector<vk::CommandBuffer> _commandBuffers;

    std::vector<vk::Semaphore> _imageAvailableSemaphores;
    std::vector<vk::Semaphore> _renderFinishedSemaphores;

    std::vector<vk::Fence> _inFlightFences;
    std::vector<vk::Fence> _imagesInFlight;

    vk::Format _swapChainImageFormat;
    vk::Extent2D _swapChainExtent;

    vk::CommandPool _commandPool;

    GraphicsPipeline* _graphicsPipeline;

    // Vulkan validation layers
    const bool _enableValidationLayers = true;
    const std::vector<const char*> _validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> _deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    bool init();
    void drawFrame();
    void cleanup();

    // Creates a vulkan instance for the window
    bool createVulkanInstance();

    // Set up the debug messenger
    bool setupDebugMessenger();

    // Create a surface to render to
    bool createSurface();

    // Selects a default device for rendering
    bool selectDefaultDevice();

    // Create a logical device for the selected physical device
    bool createLogicalDevice();

    // Create a swap-chain for the application
    bool createSwapChain();

    // Create image views
    bool createImageViews();

    // Create the render pass
    bool createRenderPass();

    // Create the graphics pipeline
    bool createGraphicsPipeline();

    // Create the frame buffers
    bool createFrameBuffers();

    // Create the command pool
    bool createCommandPool();

    // Create the command buffers
    bool createCommandBuffers();

    // Create the sync objects
    bool createSyncObjects();

    // Check that requested validation layers are available
    bool checkValidationLayersSupport();

    // Returns a list of required extensions for the application
    std::vector<const char*> getRequiredExtensions();

    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);

    // Check to see if a device supported the needed extensions
    bool checkDeviceExtensionSupport(vk::PhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

    // Callbacks
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {

        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                spdlog::debug("[Vulkan] {}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                spdlog::info("[Vulkan] {}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                spdlog::warn("[Vulkan] {}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                spdlog::error("[Vulkan] {}", pCallbackData->pMessage);
                break;
        }

        return VK_FALSE;
    }
};