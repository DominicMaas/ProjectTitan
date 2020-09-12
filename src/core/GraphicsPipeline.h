#pragma once

#include <pch.h>

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

    vk::DescriptorSetLayout _uboDescriptorSetLayout;
    vk::DescriptorSetLayout _texSamplerDescriptorSetLayout;
    vk::DescriptorPool _descriptorPool;

    vk::PipelineLayout _pipelineLayout;
    vk::Pipeline _graphicsPipeline;

    static vk::ShaderModule createShaderModule(vk::Device device, const std::vector<char>& code);

public:
    GraphicsPipeline(PipelineInfo info);

    void create(CreateGraphicsPipelineInfo createInfo);
    void destroy(DestroyGraphicsPipelineInfo info);

    vk::Pipeline getVKPipeline() { return _graphicsPipeline; }
    vk::PipelineLayout getPipelineLayout() { return _pipelineLayout; }
    vk::DescriptorPool getDescriptorPool() { return _descriptorPool; }

    vk::DescriptorSet createUBODescriptorSet();
    vk::DescriptorSet createTexSamplerDescriptorSet();
};