#pragma once

#include "pch.h"
#include "Shader.h"

struct CreateGraphicsPipelineInfo {
    Shader* shader;
    vk::Device device;
    vk::RenderPass renderPass;
};

struct DestroyGraphicsPipelineInfo {
    vk::Device device;
};

class GraphicsPipeline {
private:
    vk::ShaderModule _vertexShader;
    vk::ShaderModule _fragmentShader;

    vk::DescriptorSetLayout _descriptorSetLayout;
    vk::PipelineLayout _pipelineLayout;
    vk::Pipeline _graphicsPipeline;

    vk::ShaderModule createShaderModule(vk::Device device, const std::vector<char>& code);

public:
    GraphicsPipeline(CreateGraphicsPipelineInfo info);
    void destroy(DestroyGraphicsPipelineInfo info);

    vk::Pipeline getVKPipeline() { return _graphicsPipeline; }
    vk::PipelineLayout getPipelineLayout() { return _pipelineLayout; }
    vk::DescriptorSetLayout getDescriptorSetLayout() { return _descriptorSetLayout; }
};