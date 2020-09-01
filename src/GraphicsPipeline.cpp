#include "GraphicsPipeline.h"

vk::ShaderModule GraphicsPipeline::createShaderModule(const std::vector<char> &code) {
    vk::ShaderModuleCreateInfo createInfo = {
            .codeSize = code.size(),
            .pCode = reinterpret_cast<const uint32_t*>(code.data())
    };

    return _window->getDevice().createShaderModule(createInfo);
}

GraphicsPipeline::GraphicsPipeline(Window *window, Shader* shader) {
    this->_window = window;

    // Create the shaders
    _vertexShader = createShaderModule(shader->getVertexSource());
    _fragmentShader = createShaderModule(shader->getFragmentSource());

    // Create the shader pipelines
    vk::PipelineShaderStageCreateInfo vertexCreateInfo = {
            .stage = vk::ShaderStageFlagBits::eVertex,
            .module = _vertexShader,
            .pName = "main"
    };

    vk::PipelineShaderStageCreateInfo fragmentCreateInfo = {
            .stage = vk::ShaderStageFlagBits::eFragment,
            .module = _fragmentShader,
            .pName = "main"
    };

    vk::PipelineShaderStageCreateInfo shaderStages[] = { vertexCreateInfo, fragmentCreateInfo };

    // The format of the vertex data that will be passed to the vertex shader
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr
    };

    // How to draw
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {
            .topology = vk::PrimitiveTopology::eTriangleList,
            .primitiveRestartEnable = VK_FALSE
    };

    // Set the viewport
    vk::Viewport viewport = { 0, 0, (float)_window->getSwapChainExtent().width,
                              (float)_window->getSwapChainExtent().height, 0.0f, 1.0f };

    // We want to render to the entire framebuffer, so don't worry about this
    vk::Rect2D scissor = { {0,0}, _window->getSwapChainExtent() };

    // Create the viewport state
    vk::PipelineViewportStateCreateInfo viewportState = {
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer = {
            .depthClampEnable = VK_FALSE, // Useful for shadow maps, requires GPU feature
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = vk::PolygonMode::eFill,

            // Enable face culling
            .cullMode = vk::CullModeFlagBits::eBack,
            .frontFace = vk::FrontFace::eClockwise,

            // Sometimes used for shadow mapping
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f, // Optional
            .depthBiasClamp = 0.0f, // Optional
            .depthBiasSlopeFactor = 0.0f, // Optional

            .lineWidth = 1.0f,
    };

    // Multisampling (disable for now)
    vk::PipelineMultisampleStateCreateInfo multisampling = {
            .rasterizationSamples = vk::SampleCountFlagBits::e1,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
    };

    // TODO: Depth

    // Color blending
    vk::PipelineColorBlendAttachmentState colorBlendAttachment = {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
            .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
            .colorBlendOp = vk::BlendOp::eAdd,
            .srcAlphaBlendFactor = vk::BlendFactor::eOne,
            .dstAlphaBlendFactor = vk::BlendFactor::eZero,
            .alphaBlendOp = vk::BlendOp::eAdd,
            .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                    | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    };

    vk::PipelineColorBlendStateCreateInfo colorBlending = {
            .logicOpEnable = VK_FALSE,
            .logicOp = vk::LogicOp::eCopy,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment,
            .blendConstants = std::array<float, 4>({ 0.0f, 0.0f, 0.0f, 0.0f })
    };

    // These states can be changed without recreating the pipeline
    vk::DynamicState dynamicStates[] = {
            vk::DynamicState::eViewport, vk::DynamicState::eLineWidth
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo {
            .dynamicStateCount = 2,
            .pDynamicStates = dynamicStates
    };

    // TODO: Specify Uniforms from input shader
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
    };

    // Create the pipeline layout
    _pipelineLayout = _window->getDevice().createPipelineLayout(pipelineLayoutInfo, nullptr);

    vk::GraphicsPipelineCreateInfo pipelineInfo = {
            // Shader stages
            .stageCount = 2,
            .pStages = shaderStages,

            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = nullptr, // Optional
            .pColorBlendState = &colorBlending,
            .pDynamicState = nullptr, // Optional
            .layout = _pipelineLayout,
            .renderPass = _window->getRenderPass(),
            .subpass = 0,
            .basePipelineHandle = nullptr, // Optional
            .basePipelineIndex = -1, // Optional
    };

    auto [result, pipeline] = _window->getDevice().createGraphicsPipeline(nullptr, pipelineInfo);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    _graphicsPipeline = pipeline;
}

GraphicsPipeline::~GraphicsPipeline() {
    _window->getDevice().destroyPipeline(_graphicsPipeline);

    _window->getDevice().destroyPipelineLayout(_pipelineLayout);

    _window->getDevice().destroyShaderModule(_fragmentShader);
    _window->getDevice().destroyShaderModule(_vertexShader);
}
