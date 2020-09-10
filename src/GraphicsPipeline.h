#pragma once

#include "pch.h"
#include "core/ResourceManager.h"

struct CreateGraphicsPipelineInfo {
    vk::Device device;
    vk::RenderPass renderPass;
};

struct PipelineInfo {
    std::string shaderName;
};

struct DestroyGraphicsPipelineInfo {
    vk::Device device;
};

class GraphicsPipeline {
private:
    PipelineInfo _info;

    vk::ShaderModule _vertexShader;
    vk::ShaderModule _fragmentShader;

    vk::DescriptorSetLayout _descriptorSetLayout;

    vk::PipelineLayout _pipelineLayout;
    vk::Pipeline _graphicsPipeline;

    vk::ShaderModule createShaderModule(vk::Device device, const std::vector<char>& code);

public:
    GraphicsPipeline(PipelineInfo info);

    void create(CreateGraphicsPipelineInfo createInfo);
    void destroy(DestroyGraphicsPipelineInfo info);

    vk::DescriptorPool DescriptorPool;

    vk::Pipeline getVKPipeline() { return _graphicsPipeline; }
    vk::PipelineLayout getPipelineLayout() { return _pipelineLayout; }
    vk::DescriptorPool getDescriptorSetPool() { return DescriptorPool; }
    vk::DescriptorSetLayout getDescriptorSetLayout() { return _descriptorSetLayout; }
};