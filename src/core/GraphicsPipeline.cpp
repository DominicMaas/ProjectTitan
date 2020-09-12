#include "GraphicsPipeline.h"
#include "Vertex.h"
#include "Renderer.h"
#include "resources/Shader.h"
#include "managers/ResourceManager.h"

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

    vk::DescriptorSetLayoutBinding samplerLayoutBinding = {
            .binding = 0,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eFragment,
            .pImmutableSamplers = nullptr
    };

    vk::DescriptorSetLayoutCreateInfo uboLayoutInfo = {
            .bindingCount = 1,
            .pBindings = &uboLayoutBinding
    };

    vk::DescriptorSetLayoutCreateInfo texSamplerLayoutInfo = {
            .bindingCount = 1,
            .pBindings = &samplerLayoutBinding
    };

    _uboDescriptorSetLayout = createInfo.device.createDescriptorSetLayout(uboLayoutInfo);
    _texSamplerDescriptorSetLayout = createInfo.device.createDescriptorSetLayout(texSamplerLayoutInfo);

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

    // Multisampling
    vk::PipelineMultisampleStateCreateInfo multisampling = {
            .rasterizationSamples = Renderer::Instance->MSAASamples,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
    };

    // Depth
    vk::PipelineDepthStencilStateCreateInfo depthStencil = {
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = vk::CompareOp::eLess,
            .depthBoundsTestEnable = VK_FALSE,
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f,
            .stencilTestEnable = VK_FALSE,
            .front = {},
            .back = {}
    };

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

    vk::DescriptorSetLayout descriptorSetLayouts[] = { _uboDescriptorSetLayout, _uboDescriptorSetLayout, _texSamplerDescriptorSetLayout };
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {
            .setLayoutCount = 3,
            .pSetLayouts = descriptorSetLayouts,
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
            .pDepthStencilState = &depthStencil,
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
    vk::DescriptorPoolSize poolSizes[2];

    poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(1000);
    poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(1000);

    vk::DescriptorPoolCreateInfo poolInfo = {
            .maxSets = static_cast<uint32_t>(1000),
            .poolSizeCount = 2,
            .pPoolSizes = poolSizes };

    _descriptorPool = createInfo.device.createDescriptorPool(poolInfo);
}

void GraphicsPipeline::destroy(DestroyGraphicsPipelineInfo info) {
    assert(info.device);

    info.device.destroyDescriptorPool(_descriptorPool);

    info.device.destroyPipeline(_graphicsPipeline);

    info.device.destroyPipelineLayout(_pipelineLayout);

    info.device.destroyDescriptorSetLayout(_texSamplerDescriptorSetLayout);
    info.device.destroyDescriptorSetLayout(_uboDescriptorSetLayout);

    info.device.destroyShaderModule(_fragmentShader);
    info.device.destroyShaderModule(_vertexShader);
}

vk::DescriptorSet GraphicsPipeline::createUBODescriptorSet() {
    vk::DescriptorSetLayout descriptorSetLayout[] = { _uboDescriptorSetLayout };
    vk::DescriptorSetAllocateInfo descriptorAllocInfo = {
            .descriptorPool = _descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = descriptorSetLayout };

    return Renderer::Instance->Device.allocateDescriptorSets(descriptorAllocInfo)[0];
}

vk::DescriptorSet GraphicsPipeline::createTexSamplerDescriptorSet() {
    vk::DescriptorSetLayout descriptorSetLayout[] = { _texSamplerDescriptorSetLayout };
    vk::DescriptorSetAllocateInfo descriptorAllocInfo = {
            .descriptorPool = _descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = descriptorSetLayout };

    return Renderer::Instance->Device.allocateDescriptorSets(descriptorAllocInfo)[0];
}
