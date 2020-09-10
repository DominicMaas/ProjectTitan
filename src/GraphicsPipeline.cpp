#include "GraphicsPipeline.h"
#include "core/Vertex.h"

vk::ShaderModule GraphicsPipeline::createShaderModule(vk::Device device, const std::vector<char> &code) {
    vk::ShaderModuleCreateInfo createInfo = {
            .codeSize = code.size(),
            .pCode = reinterpret_cast<const uint32_t*>(code.data())
    };

    return device.createShaderModule(createInfo);
}

GraphicsPipeline::GraphicsPipeline(PipelineInfo info) {
    _info = info;
}

void GraphicsPipeline::create(CreateGraphicsPipelineInfo createInfo) {
    assert(createInfo.device);
    assert(createInfo.renderPass);

    // Create the descriptor set layout
    vk::DescriptorSetLayoutBinding uboLayoutBinding = {
            .binding = 0,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eVertex,
            .pImmutableSamplers = nullptr
    };

    vk::DescriptorSetLayoutCreateInfo layoutInfo = {
            .bindingCount = 1,
            .pBindings = &uboLayoutBinding
    };

    _descriptorSetLayout = createInfo.device.createDescriptorSetLayout(layoutInfo, nullptr);

    // Create the shaders
    Shader* shader = ResourceManager::getShader(_info.shaderName);
    _vertexShader = createShaderModule(createInfo.device, shader->getVertexSource());
    _fragmentShader = createShaderModule(createInfo.device, shader->getFragmentSource());

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
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
            .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    // How to draw
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {
            .topology = vk::PrimitiveTopology::eTriangleList,
            .primitiveRestartEnable = VK_FALSE
    };

    // Create the viewport state
    vk::PipelineViewportStateCreateInfo viewportState = {
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer = {
            .depthClampEnable = VK_FALSE, // Useful for shadow maps, requires GPU feature
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = vk::PolygonMode::eFill,

            // Enable face culling
            .cullMode = vk::CullModeFlagBits::eBack,
            .frontFace = vk::FrontFace::eCounterClockwise,

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
            vk::DynamicState::eViewport, vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo {
            .dynamicStateCount = 2,
            .pDynamicStates = dynamicStates
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {
            .setLayoutCount = 1,
            .pSetLayouts = &_descriptorSetLayout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
    };

    // Create the pipeline layout
    _pipelineLayout = createInfo.device.createPipelineLayout(pipelineLayoutInfo, nullptr);

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
            .pDynamicState = &dynamicStateInfo,
            .layout = _pipelineLayout,
            .renderPass = createInfo.renderPass,
            .subpass = 0,
            .basePipelineHandle = nullptr, // Optional
            .basePipelineIndex = -1, // Optional
    };

    auto [result, pipeline] = createInfo.device.createGraphicsPipeline(nullptr, pipelineInfo);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    _graphicsPipeline = pipeline;

    // Create the descriptor pool for this pipeline
    vk::DescriptorPoolSize poolSize = {
            .type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = static_cast<uint32_t>(5) };

    vk::DescriptorPoolCreateInfo poolInfo = {
            .maxSets = static_cast<uint32_t>(5),
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize };

    _descriptorPool = createInfo.device.createDescriptorPool(poolInfo);
}

void GraphicsPipeline::destroy(DestroyGraphicsPipelineInfo info) {
    assert(info.device);

    info.device.destroyDescriptorPool(_descriptorPool);

    info.device.destroyPipeline(_graphicsPipeline);

    info.device.destroyPipelineLayout(_pipelineLayout);

    info.device.destroyDescriptorSetLayout(_descriptorSetLayout);

    info.device.destroyShaderModule(_fragmentShader);
    info.device.destroyShaderModule(_vertexShader);
}
