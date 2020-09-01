#pragma once

#include "Window.h"
#include "Shader.h"

class Window;
class Shader;

class GraphicsPipeline {
private:
    vk::ShaderModule _vertexShader;
    vk::ShaderModule _fragmentShader;

    vk::PipelineLayout _pipelineLayout;
    vk::Pipeline _graphicsPipeline;

    // Used to get access to vulkan APIs
    Window* _window;

    vk::ShaderModule createShaderModule(const std::vector<char>& code);

public:
    GraphicsPipeline(Window* window, Shader* shader);
    ~GraphicsPipeline();

    vk::Pipeline getVKPipeline() { return _graphicsPipeline; }
};