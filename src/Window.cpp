#include "Window.h"
#include "core/managers/PipelineManager.h"

Window::Window(const char *title, unsigned int initialWidth, unsigned int initialHeight) {
    this->_title = title;
    this->_width = initialWidth;
    this->_height = initialHeight;
}

void Window::run() {
    // Setup for per-frame time logic
    long double previousFrameTime = glfwGetTime();
    long double deltaTime = 0;
    long double deltaTimeAccum = 0;

    long double timeStep = 1.0/60.0; // Constant physics time step

    int frames = 0; // The framerate to display
    long double lastFramesTime = glfwGetTime();
    long double frameTime = 0;
    int fps;

    // Enter game loop
    while(!glfwWindowShouldClose(_window)) {
        // ---------- Per-frame time logic ---------- //
        long double currentFrameTime = glfwGetTime(); // Current system time
        deltaTime = currentFrameTime - previousFrameTime; // Time difference between frames
        previousFrameTime = currentFrameTime; // Update the previous time

        // Add the time difference in the accumulator
        deltaTimeAccum += deltaTime;

        // Calculate frames
        frames++;
        if ((currentFrameTime - lastFramesTime ) >= 1.0) {
            frameTime = 1000.0 / double(frames);
            fps = frames;
            frames = 0;
            lastFramesTime += 1.0f; // Reset timer
        }

        // ---------- Run update events ---------- //

        // Run manual updates
        if (onUpdate) {
            onUpdate(deltaTime);
        }

        // Update the current scene
        if (Renderer::Instance->CurrentScene != nullptr) {
            Renderer::Instance->CurrentScene->update(deltaTime);
        }

        // ---------- Process Physics ---------- //

        // While there is enough accumulated time to take
        // one or several physics steps
        while (deltaTimeAccum >= timeStep) {
            // Update the physics world with a constant time step
            // TODO: currentWorld->getPhysicsWorld()->update(timeStep);

            // Decrease the accumulated time
            deltaTimeAccum -= timeStep;
        }

        // Update all objects within the world
        // TODO: currentWorld->updatePhysics(timeStep, deltaTimeAccum);

        // ---------- Render ---------- //

        drawFrame();

        // ---------- End Frame ---------- //

        glfwPollEvents();
    }

    _renderer->Device.waitIdle();

    // Clean up user resources

    // Clean up resources
    cleanup();
}

bool Window::init() {
    spdlog::info("[Window] Initializing GLFW...");

    _renderer = new Renderer();

    // Initialise GLFW
    if (!glfwInit()) {
        spdlog::error("[Window] Failed to Initialise GLFW");
        return false;
    }

    // This is a vulkan window, so don't setup with an API
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Create the window
    _window = glfwCreateWindow(_width, _height, _title, nullptr, nullptr);
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
    glfwSetMouseButtonCallback(_window, mouseButtonCallback);
    glfwSetCursorPosCallback(_window, cursorPosCallback);

    // Attempt to create the vulkan instance
    if (!createVulkanInstance()) {
        spdlog::error("[Window] Failed to create Vulkan instance");
        return false;
    }

    // Setup the debug messenger
    if (!setupDebugMessenger()) {
        spdlog::error("[Window] Failed to setup debug messenger");
        return false;
    }

    // Create the surface
    if (!createSurface()) {
        spdlog::error("[Window] Failed to create a surface to render to");
        return false;
    }

    // Select a default device
    if (!selectDefaultDevice()) {
        spdlog::error("[Window] Failed to select a default device");
        return false;
    }

    // Create a logical device
    if (!createLogicalDevice()) {
        spdlog::error("[Window] Failed to create a logical device");
        return false;
    }

    if (!createMemoryAllocator()) {
        spdlog::error("[Window] Failed to create memory allocator");
        return false;
    }

    if (!createSwapChain()) {
        spdlog::error("[Window] Failed to create the swap-chain");
        return false;
    }

    if (!createImageViews()) {
        spdlog::error("[Window] Failed to create vk image views");
        return false;
    }

    if (!createRenderPass()) {
        spdlog::error("[Window] Failed to create render pass");
        return false;
    }

    if (!createGraphicsPipeline()) {
        spdlog::error("[Window] Failed to create the graphics pipeline");
        return false;
    }

    if (!createDepthResources()) {
        spdlog::error("[Window] Failed to create depth resources");
        return false;
    }

    if (!createFrameBuffers()) {
        spdlog::error("[Window] Failed to create the frame buffers");
        return false;
    }

    if (!createCommandPool()) {
        spdlog::error("[Window] Failed to create the command pool");
        return false;
    }

    if (!createCommandBuffers()) {
        spdlog::error("[Window] Failed to create the command buffers");
        return false;
    }

    if (!createSyncObjects()) {
        spdlog::error("[Window] Failed to sync objects");
        return false;
    }

    // Successfully created!
    return true;
}

void Window::drawFrame() {
    _renderer->Device.waitForFences(1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);




    // Acquire the next image
    unsigned int imageIndex;
    try {
        imageIndex = _renderer->Device.acquireNextImageKHR(_swapChain, UINT64_MAX, _imageAvailableSemaphores[_currentFrame], nullptr);
    } catch (vk::OutOfDateKHRError const &e) {
        _framebufferResized = false;
        recreateSwapchain();
        return;
    }

    // Run the command buffer
    recordCommandBuffers(imageIndex);


    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (VkFence(_imagesInFlight[imageIndex]) != VK_NULL_HANDLE) {
        _renderer->Device.waitForFences(1, &_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    // Mark the image as now being in use by this frame
    _imagesInFlight[imageIndex] = _inFlightFences[_currentFrame];

    // Submit the command buffer
    vk::Semaphore waitSemaphores[] = { _imageAvailableSemaphores[_currentFrame] };
    vk::Semaphore signalSemaphores[] = { _renderFinishedSemaphores[_currentFrame] };

    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::SubmitInfo submitInfo = {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores,
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &_commandBuffers[imageIndex],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signalSemaphores
    };

    _renderer->Device.resetFences(1, &_inFlightFences[_currentFrame]);

    // Submit to the graphics queue
    _renderer->GraphicsQueue.submit(1, &submitInfo, _inFlightFences[_currentFrame]);

    vk::SwapchainKHR swapChains[] = { _swapChain };
    vk::PresentInfoKHR presentInfo = {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = signalSemaphores,
            .swapchainCount = 1,
            .pSwapchains = swapChains,
            .pImageIndices = &imageIndex,
            .pResults = nullptr
    };

    // Present to the GPU
    try {
        auto result = _renderer->GraphicsQueue.presentKHR(presentInfo);
        if (result == vk::Result::eSuboptimalKHR || _framebufferResized) {
            _framebufferResized = false;
            recreateSwapchain();
        }
    } catch (vk::OutOfDateKHRError const &e) {
        _framebufferResized = false;
        recreateSwapchain();
        return;
    }

    // Change the frame
    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Window::recordCommandBuffers(int i) {
    // Record
    //for (size_t i = 0; i < _commandBuffers.size(); i++) {
        _commandBuffers[i].reset({});

        _commandBuffers[i].begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

        vk::ClearValue clearValues[2];
        clearValues[0].color = { std::array<float, 4>({ 0.0f, 0.0f, 0.0f, 1.0f })};
        clearValues[1].depthStencil = {1.0f, 0};

        vk::RenderPassBeginInfo renderPassInfo = {
                .renderPass = _renderPass,
                .framebuffer = _swapChainFrameBuffers[i],
                .renderArea.extent = _swapChainExtent,
                .renderArea.offset = {0, 0},
                .clearValueCount = 2,
                .pClearValues = clearValues
        };

        _commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

        // Attempt to get the pipeline
        auto* pipeline = PipelineManager::getPipeline("basic");
        if (pipeline != nullptr) {
            _commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->getVKPipeline());

            // Set the viewport
            vk::Viewport viewport = { 0, 0, (float)_swapChainExtent.width,
                                      (float)_swapChainExtent.height, 0.0f, 1.0f };

            // We want to render to the entire framebuffer, so don't worry about this
            vk::Rect2D scissor = { {0,0}, _swapChainExtent };

            // Set the viewport
            _commandBuffers[i].setViewport(0, 1, &viewport);
            _commandBuffers[i].setScissor(0, 1, &scissor);

            // Render the current scene
            if (Renderer::Instance->CurrentScene != nullptr) {
                Renderer::Instance->CurrentScene->render(_commandBuffers[i], "basic");
            }
        }

        _commandBuffers[i].endRenderPass();
        _commandBuffers[i].end();
    //}
}

bool Window::recreateSwapchain() {
    // If the game has been minimised
    int width = 0, height = 0;
    glfwGetFramebufferSize(_window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(_window, &width, &height);
        glfwWaitEvents();
    }

    // Wait until idle
    _renderer->Device.waitIdle();

    // Clean up
    cleanupSwapchain();

    // Recreate
    if (!createSwapChain()) {
        spdlog::error("[Window] Failed to create the swap-chain");
        return false;
    }

    if (!createImageViews()) {
        spdlog::error("[Window] Failed to create vk image views");
        return false;
    }

    if (!createRenderPass()) {
        spdlog::error("[Window] Failed to create render pass");
        return false;
    }

    if (!createFrameBuffers()) {
        spdlog::error("[Window] Failed to create the frame buffers");
        return false;
    }

    if (!createCommandBuffers()) {
        spdlog::error("[Window] Failed to create the command buffers");
        return false;
    }

    return true;
}

void Window::cleanupSwapchain() {
    // Destroy the frame buffers
    for (auto frameBuffer : _swapChainFrameBuffers) {
        _renderer->Device.destroyFramebuffer(frameBuffer);
    }

    // Free the command buffers
    _renderer->Device.freeCommandBuffers(_renderer->CommandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());

    // Destroy the render pass
    _renderer->Device.destroyRenderPass(_renderPass);

    // Destroy the image views
    for (auto imageView : _swapChainImageViews) {
        _renderer->Device.destroyImageView(imageView);
    }

    // Destroy the swapchain
    _renderer->Device.destroySwapchainKHR(_swapChain);
}

void Window::cleanup() {
    // Clean up the swap chain
    cleanupSwapchain();

    // Destroy the sync objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        _renderer->Device.destroySemaphore(_imageAvailableSemaphores[i]);
        _renderer->Device.destroySemaphore(_renderFinishedSemaphores[i]);
        _renderer->Device.destroyFence(_inFlightFences[i]);
    }

    // Destroy the command pool
    _renderer->Device.destroyCommandPool(_renderer->CommandPool);

    // Delete the current scene
    delete Renderer::Instance->CurrentScene;
    Renderer::Instance->CurrentScene = nullptr;

    ResourceManager::cleanup();

    // Destroy the pipelines
    PipelineManager::cleanup({ .device = _renderer->Device });

    // Destroy the memory allocator
    vmaDestroyAllocator(_renderer->Allocator);

    // Destroy the device
    _renderer->Device.destroy();

    // Destroy the surface
    _instance.destroySurfaceKHR(_surface);

    // Cleanup debug logger
    if (_enableValidationLayers) {
        vk::DispatchLoaderDynamic instanceLoader(_instance, vkGetInstanceProcAddr);
        _instance.destroyDebugUtilsMessengerEXT(_debugMessenger, nullptr, instanceLoader);
    }

    // Clean up vulkan
    _instance.destroy();

    // Clean up GLFW
    glfwDestroyWindow(_window);
    glfwTerminate();
}

bool Window::createVulkanInstance() {
    spdlog::info("[Window] Initializing Vulkan...");

    // Ensure validation layers are good
    if (_enableValidationLayers && !checkValidationLayersSupport()) {
        spdlog::info("[Window] Validation layers requested, but not available!");
        return false;
    }

    // Information about the vulkan application
    vk::ApplicationInfo applicationInfo {
        .pApplicationName = _title,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };

    vk::InstanceCreateInfo createInfo {
        .pApplicationInfo = & applicationInfo
    };

    // Desired global extensions
    auto extensions = getRequiredExtensions();

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Include validation layers
    if (_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
        createInfo.ppEnabledLayerNames = _validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    // Actually create the instance
    auto result = vk::createInstance(&createInfo, nullptr, &_instance);
    if (result != vk::Result::eSuccess) {
        return false;
    }

    // Created successfully!
    return true;
}

bool Window::setupDebugMessenger() {
    vk::DebugUtilsMessengerCreateInfoEXT createInfo = {
            .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
                    | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
                    | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                    | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                    | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                    | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            .pfnUserCallback = debugCallback,
            .pUserData = nullptr
    };

    vk::DispatchLoaderDynamic instanceLoader(_instance, vkGetInstanceProcAddr);
    auto result = _instance.createDebugUtilsMessengerEXT(&createInfo, nullptr, &_debugMessenger, instanceLoader);
    if (result != vk::Result::eSuccess) {
        spdlog::error("[Window] Failed to set up the debug messenger: {}", result);
        return false;
    }

    // Setup successfully!
    return true;
}

bool Window::createSurface() {
    auto vkSurface = VkSurfaceKHR(_surface);
    auto result = glfwCreateWindowSurface(_instance, _window, nullptr, &vkSurface);
    if (result != VK_SUCCESS) {
        spdlog::error("[Window] Failed to create window surface: {}", result);
        return false;
    }

    // Since we wrapped before, we must unwrap
    _surface = vkSurface;

    return true;
}

bool Window::selectDefaultDevice() {
    // Get a list of physical devices
    auto devices = _instance.enumeratePhysicalDevices();
    if (devices.empty()) {
        spdlog::error("[Window] Failed to find any GPUs with vulkan support!");
        return false;
    }

    // TODO: Select the appropriate device
    auto selectedDevice = devices[0];

    // Queue families
    auto indices = findQueueFamilies(selectedDevice);
    if (!indices.isComplete()) {
        spdlog::error("[Window] The selected device does not support the wanted queue family!");
        return false;
    }

    // Extensions
    bool extensionsSupported = checkDeviceExtensionSupport(selectedDevice);
    if (!extensionsSupported) {
        spdlog::error("[Window] The selected device does not support the wanted device extensions!");
        return false;
    }

    // Check swap-chain support
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(selectedDevice);
    bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    if (!swapChainAdequate) {
        spdlog::error("[Window] The selected device does not support the wanted swap-chain features!");
        return false;
    }

    _renderer->PhysicalDevice = selectedDevice;

    // Log information about the selected device
    auto properties = _renderer->PhysicalDevice.getProperties();
    spdlog::info("[Window] Selected GPU: {}", properties.deviceName);

    return true;
}

bool Window::createLogicalDevice() {
    auto indices = findQueueFamilies(_renderer->PhysicalDevice);
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    float queuePriority = 1.0f;
    for (auto queueFamily : uniqueQueueFamilies) {
        // Device queue creation info
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo {
                .queueFamilyIndex = queueFamily,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority
        };

        queueCreateInfos.push_back(deviceQueueCreateInfo);
    }

    // The wanted device features
    vk::PhysicalDeviceFeatures deviceFeatures { .samplerAnisotropy = VK_TRUE };

    // Device creation info
    vk::DeviceCreateInfo deviceCreateInfo {
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size()),
        .ppEnabledExtensionNames = _deviceExtensions.data(),
        .pEnabledFeatures = &deviceFeatures
    };

    // Validation Layers
    if (_enableValidationLayers) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = _validationLayers.data();
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    // Create the device
    auto result = _renderer->PhysicalDevice.createDevice(&deviceCreateInfo, nullptr, &_renderer->Device);
    if (result != vk::Result::eSuccess) {
        spdlog::error("[Window] Could not create logical device: {}", result);
        return false;
    }

    // Get the queues
    _renderer->Device.getQueue(indices.graphicsFamily.value(), 0, &_renderer->GraphicsQueue);
    _renderer->Device.getQueue(indices.presentFamily.value(), 0, &_presentQueue);

    return true;
}

bool Window::createMemoryAllocator() {
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = _renderer->PhysicalDevice;
    allocatorInfo.device = _renderer->Device;
    allocatorInfo.instance = _instance;
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;

    if (vmaCreateAllocator(&allocatorInfo, &_renderer->Allocator) != VK_SUCCESS) {
        return false;
    }

    return true;
}

bool Window::createSwapChain() {
    // Get the wanted details
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_renderer->PhysicalDevice);

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    // Recommended to be one above the minimum image count
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    // Don't exceed the maximum number of images
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR swapchainCreateInfoKhr = {
            .surface = _surface,
            .minImageCount = imageCount,
            .imageFormat = surfaceFormat.format,
            .imageColorSpace = surfaceFormat.colorSpace,
            .imageExtent = extent,
            .imageArrayLayers = 1,
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment
    };

    // Handle the different queues
    QueueFamilyIndices indices = findQueueFamilies(_renderer->PhysicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        swapchainCreateInfoKhr.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfoKhr.queueFamilyIndexCount = 2;
        swapchainCreateInfoKhr.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapchainCreateInfoKhr.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCreateInfoKhr.queueFamilyIndexCount = 0; // Optional
        swapchainCreateInfoKhr.pQueueFamilyIndices = nullptr; // Optional
    }

    // We don't want any transformation, set to default
    swapchainCreateInfoKhr.preTransform = swapChainSupport.capabilities.currentTransform;

    // Ignore alpha
    swapchainCreateInfoKhr.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

    swapchainCreateInfoKhr.presentMode = presentMode;
    swapchainCreateInfoKhr.clipped = VK_TRUE;

    // TODO: Handle this later when we resize the window
    swapchainCreateInfoKhr.oldSwapchain = nullptr;

    // Create the swapchain
    auto result = _renderer->Device.createSwapchainKHR(&swapchainCreateInfoKhr, nullptr, &_swapChain);
    if (result != vk::Result::eSuccess) {
        spdlog::error("[Window] Could not create logical device: {}", result);
        return false;
    }

    // TODO: This might not work
    _swapChainImages = _renderer->Device.getSwapchainImagesKHR(_swapChain);

    // We need these later
    _swapChainImageFormat = surfaceFormat.format;
    _swapChainExtent = extent;

    return true;
}

bool Window::createImageViews() {
    // Resize list to the correct size
    _swapChainImageViews.resize(_swapChainImages.size());

    // Iterate over images
    for (int i = 0; i < _swapChainImages.size(); i++) {
        _swapChainImageViews[i] = Renderer::Instance->createImageView(_swapChainImages[i], _swapChainImageFormat, vk::ImageAspectFlagBits::eColor);
    }

    return true;
}

bool Window::createRenderPass() {
    vk::AttachmentDescription depthAttachment = {
            .format = Renderer::Instance->findDepthFormat(),
            .samples = vk::SampleCountFlagBits::e1,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eDontCare,
            .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout = vk::ImageLayout::eUndefined,
            .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal
    };

    vk::AttachmentDescription colorAttachment = {
            .format = _swapChainImageFormat,
            .samples = vk::SampleCountFlagBits::e1,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout = vk::ImageLayout::eUndefined,
            .finalLayout = vk::ImageLayout::ePresentSrcKHR
    };

    vk::AttachmentReference colorAttachmentRef = { 0, vk::ImageLayout::eColorAttachmentOptimal };
    vk::AttachmentReference depthAttachmentRef = { 1, vk::ImageLayout::eDepthStencilAttachmentOptimal };

    vk::SubpassDescription subpass = {
            .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentRef,
            .pDepthStencilAttachment = &depthAttachmentRef
    };

    vk::SubpassDependency dependency = {
           .srcSubpass = VK_SUBPASS_EXTERNAL,
           .dstSubpass = 0,

           .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
           .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,

           .srcAccessMask = {},
           .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
    };

    vk::AttachmentDescription attachments[] = { colorAttachment, depthAttachment };
    vk::RenderPassCreateInfo renderPassInfo = {
            .attachmentCount = 2,
            .pAttachments = attachments,
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &dependency
    };

    try {
        _renderPass = _renderer->Device.createRenderPass(renderPassInfo, nullptr);
    } catch (std::exception const &e) {
        spdlog::error("[Window] Could not create render pass: {}", e.what());
        return false;
    }

    return true;
}

bool Window::createGraphicsPipeline() {
    CreateGraphicsPipelineInfo createInfo = {
            .device = _renderer->Device,
            .renderPass = _renderPass };

    PipelineManager::init(createInfo);
    return true;
}

bool Window::createFrameBuffers() {
    // Resize the vector
    _swapChainFrameBuffers.resize(_swapChainImageViews.size());

    for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
        vk::ImageView attachments[] = { _swapChainImageViews[i], _depthImageView };

        vk::FramebufferCreateInfo framebufferInfo = {
                .renderPass = _renderPass,
                .attachmentCount = 2,
                .pAttachments = attachments,
                .width = _swapChainExtent.width,
                .height = _swapChainExtent.height,
                .layers = 1
        };

        try {
            _swapChainFrameBuffers[i] = _renderer->Device.createFramebuffer(framebufferInfo);
        } catch (std::exception const &e) {
            spdlog::error("[Window] Could not create frame buffer: {}", e.what());
            return false;
        }
    }

    return true;
}

bool Window::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(_renderer->PhysicalDevice);

    vk::CommandPoolCreateInfo poolInfo = {
            .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value() };

    try {
        _renderer->CommandPool = _renderer->Device.createCommandPool(poolInfo);
    } catch (std::exception const &e) {
        spdlog::error("[Window] Failed to create command pool: {}", e.what());
        return false;
    }

    return true;
}

bool Window::createDepthResources() {
    vk::Format depthFormat = Renderer::Instance->findDepthFormat();

    Renderer::Instance->createImage(_depthImage, _depthAllocation, _swapChainExtent.width, _swapChainExtent.height, VkFormat(depthFormat), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    _depthImageView = Renderer::Instance->createImageView(_depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);

    return true;
}

bool Window::createCommandBuffers() {
    // Resize for data
    _commandBuffers.resize(_swapChainFrameBuffers.size());

    vk::CommandBufferAllocateInfo allocInfo = {
            .commandPool = _renderer->CommandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = (uint32_t) _commandBuffers.size()
    };

    try {
        _commandBuffers = _renderer->Device.allocateCommandBuffers(allocInfo);
    } catch (std::exception const &e) {
        spdlog::error("[Window] Failed to create command buffers: {}", e.what());
        return false;
    }

    return true;
}

bool Window::createSyncObjects() {
    _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    _imagesInFlight.resize(_swapChainImages.size(), {});

    vk::SemaphoreCreateInfo semaphoreInfo = {};
    vk::FenceCreateInfo fenceInfo = { .flags = vk::FenceCreateFlagBits::eSignaled };

    try {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            _imageAvailableSemaphores[i] = _renderer->Device.createSemaphore(semaphoreInfo);
            _renderFinishedSemaphores[i] = _renderer->Device.createSemaphore(semaphoreInfo);
            _inFlightFences[i] = _renderer->Device.createFence(fenceInfo);
        }
    } catch (std::exception const &e) {
        spdlog::error("[Window] Failed to create sync objects: {}", e.what());
        return false;
    }

    return true;
}

bool Window::checkValidationLayersSupport() {
    // Get a list of validation layers
    auto availableLayers = vk::enumerateInstanceLayerProperties();

    // Loop through all the validation layers
    for (auto layerName : _validationLayers) {
        bool layerFound = false;

        // Compare
        for (auto layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char *> Window::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (_enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

QueueFamilyIndices Window::findQueueFamilies(vk::PhysicalDevice device) {
    QueueFamilyIndices indices;

    auto queueFamilies = device.getQueueFamilyProperties();
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        device.getSurfaceSupportKHR(i, _surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        i++;
    }

    return indices;
}

bool Window::checkDeviceExtensionSupport(vk::PhysicalDevice device) {
    auto availableExtensions = device.enumerateDeviceExtensionProperties();
    std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails Window::querySwapChainSupport(vk::PhysicalDevice device) {
    SwapChainSupportDetails details;

    device.getSurfaceCapabilitiesKHR(_surface, &details.capabilities);
    details.formats = device.getSurfaceFormatsKHR(_surface);
    details.presentModes = device.getSurfacePresentModesKHR(_surface);

    return details;
}

vk::SurfaceFormatKHR Window::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }

    // Fallback
    return availableFormats[0];
}

vk::PresentModeKHR Window::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Window::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        vk::Extent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}
